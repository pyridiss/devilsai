
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

#include "ElementsCarte.h"

#include "tools/timeManager.h"
#include "textManager/textManager.h"
#include "imageManager/imageManager.h"

#include "devilsai-gui/console.h"
#include "gamedata.h"

#define PI 3.1415926


using namespace tinyxml2;


double ToSegment(double x, int min, int max)
{
	return min + 2*(max-min)/PI * atan(x*PI/(2*(max-min)));
}

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
    return (selectedIndividual == item && intersection(interactionField, selectedIndividual->size));
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
	if (_currentSkill->Id == behavior(Behaviors::Random))
        modifyHealthStatus(Energy, tools::timeManager::I(currentHealthStatus(Life)/25000) /*+ (LieuVillage == "village")*tools::timeManager::I(currentHealthStatus(Statistiques::Life)/10000)*/);
	if (_currentSkill->Id == behavior(Behaviors::Hunting))
        modifyHealthStatus(Energy, -tools::timeManager::I(0.05/currentHealthStatus(Constitution)));

	//6. Durée d'éveil
	DureeEveil += tools::timeManager::I(0.0005);

	//7. Fatigue extrême
	if (currentHealthStatus(Energy) < 70 && currentHealthStatus(Energy) > 10)
	{
		//Agilite-
        if (_attributes[Agility] > 1)
            _attributes.add(Agility, -(70.0-currentHealthStatus(Energy))/100000.0*tools::timeManager::I(1));

		//Intelligence-
        if (_attributes[Intellect] > 1)
            _attributes.add(Intellect, -(70.0-currentHealthStatus(Energy))/100000.0*tools::timeManager::I(1));

		//Application
		if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_FATIGUE), Color(255, 128, 128, 255));
            addConsoleEntry(textManager::getText("devilsai", "FATIGUE"));
	}

    //Update interactionField of skills
    if (_skills != nullptr) for (auto& s : *_skills)
    {
        s.second.interactionField.updateDirection(angle);
    }
}

void Joueur::CoupCritique(Individu* ennemi)
{
	//Agilité+, selon Agilité de l'ennemi
    if (!(rand()%(int)(5 + 10 + ToSegment(currentHealthStatus(Agility) - ennemi->currentHealthStatus(Agility), 0, 10) + 1)))
        _attributes.add(Agility, ToSegment(1.1*ennemi->currentHealthStatus(Agility)/currentHealthStatus(Agility), 0, 10));

	//Charisme+, selon Charisme de l'ennemi
    if (!(rand()%(int)(5 + 10 + ToSegment(currentHealthStatus(Charisma) - ennemi->currentHealthStatus(Charisma), 0, 10) + 1)))
        _attributes.add(Charisma, ToSegment(1.1*ennemi->currentHealthStatus(Charisma)/currentHealthStatus(Charisma), 0, 10));

	//Application
	if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_CRITIQUE), Color(128, 255, 128, 255));
        addConsoleEntry(textManager::getText("devilsai", "CRITIQUE"));
}

void Joueur::BlessureGrave(Individu* ennemi)
{
	//Constitution-, selon Puissance de l'ennemi
    if (!(rand()%(int)(5 + 10 + ToSegment(currentHealthStatus(Constitution) - ennemi->currentHealthStatus(Power), 0, 10) + 1)))
        _attributes.add(Constitution, -min(2.0, ToSegment(1.1*ennemi->currentHealthStatus(Power)/currentHealthStatus(Constitution), 0, 10)));

	//Charisme-, selon Charisme de l'ennemi
    if (!(rand()%(int)(5 + 10 + ToSegment(currentHealthStatus(Charisma) - ennemi->currentHealthStatus(Charisma), 0, 10) + 1)))
        _attributes.add(Charisma, -min(2.0, ToSegment(1.1*ennemi->currentHealthStatus(Charisma)/currentHealthStatus(Charisma), 0, 10)));

	//Application
	if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_BLESSURE), Color(255, 128, 128, 255));
        addConsoleEntry(textManager::getText("devilsai", "BLESSURE"));
}

void Joueur::CoupEsquive(Individu* ennemi)
{
	//Esquive+, selon Agilité de l'ennemi
    if (!(rand()%(int)(5 + 10 + ToSegment(currentHealthStatus(Dodge) - ennemi->currentHealthStatus(Agility), 0, 10) + 1)))
        _attributes.add(Dodge, ToSegment(1.1*ennemi->currentHealthStatus(Agility)/currentHealthStatus(Dodge), 0, 3));

	//Application
	if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_ESQUIVE), Color(128, 255, 128, 255));
        addConsoleEntry(textManager::getText("devilsai", "ESQUIVE"));
}

void Individu::GainExperience(Individu* ennemi, float Degats, int Exp)
{
	//Gain d'expérience
	if (ennemi != NULL)
	{
        if (ennemi->currentHealthStatus(Life) > 0) Exp += Degats/5000.*ennemi->experience();
        else Exp += ennemi->experience();

		//Intelligence+, Force+, Puissance+, et Constitution+, si peu fatigué
		for (int i = 0 ; i < 1 + Exp/100 ; ++i)
		{
            if (!(rand()%(int)(50 + ToSegment(-currentHealthStatus(Energy)/100, 0, 30) + 1)))
			{
				short carac = rand()%4;
				switch(carac)
				{
                    case 0 : _attributes.add(Intellect, ToSegment(2*currentHealthStatus(Intellect), 0, 100)/100); break;
                    case 1 : _attributes.add(Strength, ToSegment(10*currentHealthStatus(Intellect), 0, 100)/100); break;
                    case 2 : _attributes.add(Power, ToSegment(10*currentHealthStatus(Intellect), 0, 100)/100); break;
                    case 3 : _attributes.add(Constitution, ToSegment(5*currentHealthStatus(Intellect), 0, 100)/100); break;
				}
			}
		}
	}

    _experience += Exp;
}

void Joueur::BlessuresMultiples(Individu* ennemi)
{
	//Force-, selon sa propre Constitution et Puissance de l'un des ennemis
    if (!(rand()%(int)(5 + 10 + ToSegment(currentHealthStatus(Constitution) - ennemi->currentHealthStatus(Power), 0, 10) + 1)))
        _attributes.add(Strength, -min(2.0, ToSegment(1.1*ennemi->currentHealthStatus(Power)/currentHealthStatus(Constitution), 0, 10)));

	//Puissance-, selon sa propre Constitution et Puissance de l'un des ennemis
    if (!(rand()%(int)(5 + 10 + ToSegment(currentHealthStatus(Constitution) - ennemi->currentHealthStatus(Power), 0, 10) + 1)))
        _attributes.add(Power, -min(2.0, ToSegment(1.1*ennemi->currentHealthStatus(Power)/currentHealthStatus(Constitution), 0, 10)));

	//Esquive-, selon sa propre Constitution et Puissance de l'un des ennemis
    if (!(rand()%(int)(5 + 10 + ToSegment(currentHealthStatus(Constitution) - ennemi->currentHealthStatus(Power), 0, 10) + 1)))
        _attributes.add(Dodge, -min(2.0, ToSegment(1.1*ennemi->currentHealthStatus(Power)/currentHealthStatus(Constitution), 0, 10)));

	//Application
	if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_BLESSURE), Color(255, 128, 128, 255));
        addConsoleEntry(textManager::getText("devilsai", "BLESSURE"));
}

bool Joueur::ApplicationAmeliorations()
{
	//Affichage & enregistrement des améliorations proposées :

	textManager::PlainText Final;
	bool Retour = false;

    for (int a = Strength ; a != numberOfAttributes ; ++a)
    {
        Attribute att = static_cast<Attribute>(a);
        int diff = floor(_attributes[att]) - _displayedAttributes[att];

        if (diff != 0)
        {
            char text[50] = "console-attributeImprovement-";
            strcat(text, attributeToString(att));
            Final = textManager::getText("devilsai", text);
            Final.addParameter(diff);
            addConsoleEntry(Final);
        }

        _displayedAttributes.set(att, floor(_attributes[att]));
    }

	return Retour;
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
