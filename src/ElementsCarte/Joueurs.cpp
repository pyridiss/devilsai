
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
#include "textManager/richText.h"
#include "imageManager/imageManager.h"

#include "devilsai-resources/shaders.h"

#include "devilsai-gui/console.h"


using namespace tinyxml2;


/** FONCTIONS DE LA CLASSE Joueur **/

Joueur::Joueur()
  : Individu(),
    _displayedAttributes(),
    _automoveEndpoint(),
    _hunted(nullptr),
    _fakeIndividual(),
    _skillForHunted(),
    _wakingTime(0),
    _tired(false),
    selectedIndividual(nullptr)
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
    _wakingTime = 0;
}

void Joueur::Gestion_Statistiques()
{
	//3. Perte d'énergie selon durée depuis repos
    if (_wakingTime > currentHealthStatus(Constitution)) modifyHealthStatus(Energy, -tools::timeManager::I(_wakingTime-currentHealthStatus(Constitution))/10000);

	//4. Gain & Perte d'énergie par activité
	if (_currentSkill == skill(behavior(Behaviors::Random)))
        modifyHealthStatus(Energy, tools::timeManager::I(currentHealthStatus(Life)/25000) /*+ (LieuVillage == "village")*tools::timeManager::I(currentHealthStatus(Statistiques::Life)/10000)*/);
	if (_currentSkill == skill(behavior(Behaviors::Hunting)))
        modifyHealthStatus(Energy, -tools::timeManager::I(0.05/currentHealthStatus(Constitution)));

	//6. Durée d'éveil
    _wakingTime += tools::timeManager::I(0.0005);

	//7. Fatigue extrême
    if (currentHealthStatus(Energy) > 150)
    {
        _tired = false;
    }
    else if (!_tired)
    {
        addConsoleEntry(textManager::getText("devilsai", "FATIGUE"));
        _tired = true;
    }
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
    d.addParameter(round(attributes()[Strength]));
    d.addParameter(currentHealthStatus(Power));
    d.addParameter(round(attributes()[Power]));
    d.addParameter(currentHealthStatus(Agility));
    d.addParameter(round(attributes()[Agility]));
    d.addParameter(currentHealthStatus(Intellect));
    d.addParameter(round(attributes()[Intellect]));

    d.addParameter(currentHealthStatus(Constitution));
    d.addParameter(round(attributes()[Constitution]));
    d.addParameter(currentHealthStatus(Charisma));
    d.addParameter(round(attributes()[Charisma]));
    d.addParameter(currentHealthStatus(Dodge));
    d.addParameter(round(attributes()[Dodge]));
    d.addParameter(currentHealthStatus(HealingPower));

    return d;
}

void Joueur::displayHealthStatus(RenderTarget& target, int x, int y)
{
    // Health gauges - Life, Energy and Healing

    Shader* lifeShader = (currentHealthStatus(Life) < 100)
        ? multimedia::shader("blink", devilsai::warnLifeShaderInstance)
        : multimedia::shader("contrast", devilsai::lifeGaugeShaderInstance);

    imageManager::display(target, "gui"_hash, "healthgauge-life", x, y + 20, false, lifeShader);
    imageManager::fillArea(target, "gui"_hash, "healthgauge-life", x, y + 20, ceil(currentHealthStatus(Life) / 6.67), 10, x, y + 20);

    Shader* energyShader = (currentHealthStatus(Energy) < 100)
        ? multimedia::shader("blink", devilsai::warnEnergyShaderInstance)
        : multimedia::shader("contrast", devilsai::energyGaugeShaderInstance);

    imageManager::display(target, "gui"_hash, "healthgauge-energy", x, y + 32, false, energyShader);
    imageManager::fillArea(target, "gui"_hash, "healthgauge-energy", x, y + 32, ceil(currentHealthStatus(Energy) / 6.67), 10, x, y + 32);

    imageManager::fillArea(target, "gui"_hash, "healthgauge-healing",
        (int)(x + 75 - max(0.0, -currentHealthStatus(Healing)/1.33)), y + 46, (int)(abs(currentHealthStatus(Healing))/1.33), 10, x, y + 46);

    // Text (name and state)

    textManager::PlainText playerStateText;

    playerStateText += "@s[14]@c[128,255,128]";
    playerStateText += displayedName();
    playerStateText += " @d@n[44]"; // Make place for the gauges.

    int l = currentHealthStatus(Life) + currentHealthStatus(Healing) * 10;

    if (currentHealthStatus(Life) == 0)
        playerStateText += textManager::getText("devilsai", "player-health-dead");
    else if (l >= 900)
        playerStateText += textManager::getText("devilsai", "player-health-1");
    else if (l >= 650)
        playerStateText += textManager::getText("devilsai", "player-health-2");
    else if (l >= 300)
        playerStateText += textManager::getText("devilsai", "player-health-3");
    else if (l >= 100)
        playerStateText += textManager::getText("devilsai", "player-health-4");
    else
        playerStateText += textManager::getText("devilsai", "player-health-5");

    if (_tired)
    {
        playerStateText += " @n";
        playerStateText += textManager::getText("devilsai", "player-health-tired");
    }

    for (auto& i : inventory.objects)
    {
        if (i.active() && i.temporary())
        {
            playerStateText += " @n";
            playerStateText += textManager::getText("devilsai", "SOUS_EFFET");
            playerStateText.addParameter(textManager::getText("objects", i.name()));
        }
    }

    textManager::RichText playerState;
    playerState.setSize(200, 0);
    playerState.setDefaultProperties("liberation", 11, Color(255, 255, 255));
    playerState.addFlags(textManager::HAlignCenter | textManager::OriginXCenter);
    playerState.create(playerStateText);
    playerState.displayFullText(target, x + 75, y);
}
