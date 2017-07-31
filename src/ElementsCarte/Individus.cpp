
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

#include <cmath>

#include <lua5.2/lua.hpp>

#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/Bibliotheque.h"
#include "../Jeu/Jeu.h"
#include "ElementsCarte.h"

#include "tools/timeManager.h"
#include "tools/math.h"
#include "imageManager/imageManager.h"

#include "Jeu/options.h"


Individu::Individu() : Element_Carte()
{
	TypeClassement = CLASSEMENT_NORMAL;
    angle = tools::math::randomNumber(0, 2 * M_PI);
    inert = false;
    interactionField.setOrigin(&position());
    viewField.setOrigin(&position());
}

void Individu::Gestion_Recuperation()
{
	if (currentHealthStatus(Statistiques::Healing) > 0)
	{
		if (100*buf_rec <= 2*currentHealthStatus(Statistiques::Healing))
		{
			modifyHealthStatus(Statistiques::Life, 1);
			if (currentHealthStatus(Statistiques::Healing) > 80) modifyHealthStatus(Statistiques::Life, 3);
			if (currentHealthStatus(Statistiques::Healing) > 90) modifyHealthStatus(Statistiques::Life, 6);
			if (currentHealthStatus(Statistiques::Healing) > 95) modifyHealthStatus(Statistiques::Life, 6);
			modifyHealthStatus(Statistiques::Energy, 1);
		}
	}
	if (currentHealthStatus(Statistiques::Healing) < 0)
	{
		if (100*buf_rec <= -2*currentHealthStatus(Statistiques::Healing))
		{
			modifyHealthStatus(Statistiques::Life, -1);
			modifyHealthStatus(Statistiques::Energy, -1);
		}
	}

	//Évolution de la récupération :

	//Test de récupération forcée (potion, …)
	if (abs(currentHealthStatus(Caracteristiques::HealingPower)) >= 95)
        setHealthStatus(Statistiques::Healing, currentHealthStatus(Caracteristiques::HealingPower));

	//Évolution spontanée
	if (buf_rec >= 10)
	{
		//Récuperation trop faible par rapport à la vitalité :
		if (currentHealthStatus(Statistiques::Healing) < currentHealthStatus(Caracteristiques::HealingPower) + (currentHealthStatus(Statistiques::Life)-800)/20)
            modifyHealthStatus(Statistiques::Healing, 1);

		//Récupération trop importante par rapport à la vitalité :
		if (currentHealthStatus(Statistiques::Healing) > currentHealthStatus(Caracteristiques::HealingPower) + (currentHealthStatus(Statistiques::Life)-800)/20)
            modifyHealthStatus(Statistiques::Healing, -1);

		//Augmentation de la Récupération automatique en fonction du niveau d'énergie
		if (currentHealthStatus(Statistiques::Energy) > 200) if (!rand()%20)
            modifyHealthStatus(Statistiques::Healing, 1);
		if (currentHealthStatus(Statistiques::Energy) > 900) if (!rand()%15)
            modifyHealthStatus(Statistiques::Healing, 1);
		buf_rec = 0;
	}
    else buf_rec += tools::timeManager::I(1);
}

int Individu::currentHealthStatus(Statistiques::Attribute a)
{
    return Stats[a];
}

int Individu::currentHealthStatus(Caracteristiques::Attribute a)
{
    //Update values
    {
        _currentHealthStatus = attributes();

        for (int i = 0 ; i != Caracteristiques::Attribute::enumSize ; ++i)
        {
            Caracteristiques::Attribute att = static_cast<Caracteristiques::Attribute>(i);

            if (att != Caracteristiques::HealingPower)
                _currentHealthStatus.set(att, _currentHealthStatus[att] / 2.0 * (1 + 1.2*currentHealthStatus(Statistiques::Life)/1000.0));

            for (auto& o : inventory.objects)
            {
                lua_getglobal(o.second, "getCurrentObjectEffect");
                lua_pushstring(o.second, Caracteristiques::toString(att).c_str());
                lua_call(o.second, 1, 1);
                _currentHealthStatus.add(att, lua_tonumber(o.second, -1));
                lua_pushstring(o.second, (Caracteristiques::toString(att) + "Factor").c_str());
                _currentHealthStatus.add(att, attributes()[att] * lua_tonumber(o.second, -1) / 100.0);
            }
        }
    }

    //Return asked value
    return floor(_currentHealthStatus[a]);
}

void Individu::setHealthStatus(Statistiques::Attribute a, double value)
{
    if (a == Statistiques::Life || a == Statistiques::Energy)
        value = min(max(0.0, value), 1000.0);
    else //Healing
        value = min(max(-100.0, value), 100.0);

    Stats.set(a, value);
}

void Individu::modifyHealthStatus(Statistiques::Attribute a, double value)
{
    if (a == Statistiques::Life || a == Statistiques::Energy)
        value = min(max(-Stats[a], value), 1000.0 - Stats[a]);
    else //Healing
        value = min(max(-100.0 - Stats[a], value), 100.0 - Stats[a]);

    Stats.add(a, value);
}

void Individu::Disp(RenderTarget& target)
{
    if (options::displayShapes())
    {
        viewField.display(target, Color(255, 255, 0, 20));
        interactionField.display(target, Color(255, 0, 0, 30));
        size.display(target, Color(255, 255, 255, 50));
    }

	Activite* act = Get_Activite(Act);
    imageManager::display(target, "individuals", act->getImageKey(angle, Num), position().x, position().y, true);
}

void Individu::displayLifeGauge()
{
    if (Diplomatie == DIPLOM_NEUTRE) return;

    //x and y are used to convert position to integers; otherwise, the text may be blurred
    int x = position().x;
    int y = position().y;

    Color color;
    if (Diplomatie == DIPLOM_ALLIE) color = Color(128, 255, 128, 255);
    if (Diplomatie == DIPLOM_ENNEMI) color = Color(255, 255, 255, 255);
    Disp_TexteCentre(Get_Nom(), x, y + 25, color, 10.f);

    RectangleShape background(Vector2f(50, 4));
    background.setPosition(x - 25, y + 35);
    background.setFillColor(Color(0, 0, 0, 175));
    Jeu.App.draw(background);

    RectangleShape foreground(Vector2f(currentHealthStatus(Statistiques::Life)/20, 4));
    foreground.setPosition(x - 25, y + 35);
    foreground.setFillColor(Color(228, 0, 0, 255));
    Jeu.App.draw(foreground);
}
