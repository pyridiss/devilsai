
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2014  Quentin Henriet

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <tinyxml2.h>

#include <cmath>

#include "tools/math.h"
#include "tools/signals.h"

#include "ElementsCarte.h"

#include "tools/timeManager.h"
#include "textManager/textManager.h"
#include "imageManager/imageManager.h"

#include "devilsai-gui/console.h"
#include "gamedata.h"


using namespace tinyxml2;


/** FONCTIONS DE LA CLASSE Joueur **/

Joueur::Joueur()
  : Individu(),
    _automoveEndpoint(),
    _hunted(nullptr),
    _fakeIndividual(),
    _skillForHunted(),
     selectedIndividual(nullptr),
    _displayedAttributes()
{
    _fakeIndividual.size.circle(tools::math::Vector2d(0, 0), 10);
}

void Joueur::otherItemDeleted(Element_Carte* other)
{
    if (_targetedItem == other)
        _targetedItem = nullptr;

    if (selectedIndividual == other)
        selectedIndividual = nullptr;
}

bool Joueur::interact(Element_Carte* item)
{
    return (_hunted == item && intersection(interactionField, _hunted->size));
}

void Joueur::Repos()
{
    setHealthStatus(Life, 1000);
    setHealthStatus(Energy, 1000);
	DureeEveil = 0;
}

void Joueur::Gestion_Statistiques()
{
	//3. Perte d'énergie selon durée depuis repos
    if (DureeEveil > currentHealthStatus(Constitution)) modifyHealthStatus(Energy, -tools::timeManager::I(DureeEveil-currentHealthStatus(Constitution))/10000);

	//4. Gain & Perte d'énergie par activité
	if (_currentSkill == skill(behavior(Behaviors::Random)))
        modifyHealthStatus(Energy, tools::timeManager::I(currentHealthStatus(Life)/25000) /*+ (LieuVillage == "village")*tools::timeManager::I(currentHealthStatus(Statistiques::Life)/10000)*/);
	if (_currentSkill == skill(behavior(Behaviors::Hunting)))
        modifyHealthStatus(Energy, -tools::timeManager::I(0.05/currentHealthStatus(Constitution)));

	//6. Durée d'éveil
	DureeEveil += tools::timeManager::I(0.0005);

	//7. Fatigue extrême
	if (currentHealthStatus(Energy) < 70 && currentHealthStatus(Energy) > 10)
	{
        improveAttribute(Agility, -1, nullptr);
        improveAttribute(Intellect, -1, nullptr);
	}

    //Update interactionField of skills
    for (auto& s : _skills)
    {
        s.second->interactionField.setOrigin(&position());
        s.second->interactionField.updateDirection(angle);
    }
}

void Joueur::improveAttribute(Attribute a, int chance, Individu* enemy)
{
    Individu::improveAttribute(a, chance, enemy);

    switch (a)
    {
        case Strength:
            [[fallthrough]]
        case Constitution:
            if (chance < 0) addConsoleEntry(textManager::getText("devilsai", "BLESSURE"));
            break;
        case Agility:
            if (chance > 0) addConsoleEntry(textManager::getText("devilsai", "CRITIQUE"));
            break;
        case Dodge:
            if (chance > 0) addConsoleEntry(textManager::getText("devilsai", "ESQUIVE"));
            break;
        default:
            break;
    }

    if (int diff = round(_attributes[a]) - _displayedAttributes[a] ; diff != 0)
    {
        char text[50] = "console-attributeImprovement-";
        strcat(text, attributeToString(a));
        textManager::PlainText t(textManager::getText("devilsai", text));

        if (diff < 0)
        {
            string v = "@c[225,128,128]" + textManager::toString(diff) + " @d";
            t.addParameter(v);
        }
        else
        {
            string v = "@c[128,255,128]+" + textManager::toString(diff) + " @d";
            t.addParameter(v);
        }

        tools::signals::addSignal("player-information", t.toStdString());
        addConsoleEntry(t);
    }

    _displayedAttributes.set(a, round(_attributes[a]));
}

void Joueur::resetDisplayedAttributes()
{
    for (int a = Strength ; a != numberOfAttributes ; ++a)
    {
        Attribute att = static_cast<Attribute>(a);
        _displayedAttributes.set(att, floor(_attributes[att]));
    }
}

void Joueur::automove(const tools::math::Vector2d& p)
{
    _automove = true;
    _automoveEndpoint = p;
}

void Joueur::hunt(Element_Carte* item, const string& skill, bool force)
{
    Individu* ind = dynamic_cast<Individu*>(item);

    if (ind != nullptr)
    {
        _hunting = true;
        _hunted = ind;
        if (ind->Diplomatie == DIPLOM_NEUTRE)
            _skillForHunted = behavior(Behaviors::Random);
        else if (force || ind->Diplomatie != Diplomatie)
            _skillForHunted = skill;
        else
            _skillForHunted = behavior(Behaviors::Random);
        angle = tools::math::angle(ind->position().x - position().x, ind->position().y - position().y);
    }
}

void Joueur::stopHunting()
{
    _hunting = false;
}

void Joueur::stopAutomoving()
{
    _automove = false;
}

void Joueur::loadFromXML(tinyxml2::XMLHandle &handle)
{
    Individu::loadFromXML(handle);
    resetDisplayedAttributes();
}

void Joueur::saveToXML(XMLDocument& doc, XMLHandle& handle)
{
    Individu::saveToXML(doc, handle);
}

textManager::PlainText Joueur::characterDescription()
{
    textManager::PlainText d = textManager::getText("devilsai", "player-description");

    d.addParameter(currentHealthStatus(Life));
    d.addParameter(currentHealthStatus(Energy));
    d.addParameter(currentHealthStatus(Healing));
    d.addParameter(experience());

    d.addParameter(currentHealthStatus(Strength));
    d.addParameter(attributes()[Strength]);
    d.addParameter(currentHealthStatus(Power));
    d.addParameter(attributes()[Power]);
    d.addParameter(currentHealthStatus(Agility));
    d.addParameter(attributes()[Agility]);
    d.addParameter(currentHealthStatus(Intellect));
    d.addParameter(attributes()[Intellect]);

    d.addParameter(currentHealthStatus(Constitution));
    d.addParameter(attributes()[Constitution]);
    d.addParameter(currentHealthStatus(Charisma));
    d.addParameter(attributes()[Charisma]);
    d.addParameter(currentHealthStatus(Dodge));
    d.addParameter(attributes()[Dodge]);
    d.addParameter(currentHealthStatus(HealingPower));

    return d;
}
