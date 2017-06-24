
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

#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/Bibliotheque.h"
#include "../Jeu/Jeu.h"
#include "ElementsCarte.h"

#include "tools/timeManager.h"
#include "tools/math.h"
#include "imageManager/imageManager.h"

/** FONCTIONS DE LA CLASSE Individu **/

/* CONSTRUCTEURS / DESTRUCTEURS */

Individu::Individu() : Element_Carte()
{
	TypeClassement = CLASSEMENT_NORMAL;
    angle = tools::math::randomNumber(0, 2 * M_PI);
    inert = false;
}


/* GESTION */

/* La fonction Individu::Gestion() est développée dans le fichier AI.cpp */
/* La fonction Individu::MouvementAleatoire() est développée dans le fichier AI.cpp */
/* La fonction Individu::MouvementChasse() est développée dans le fichier AI.cpp */

void Individu::Gestion_Recuperation()
{
	if (get("Recuperation") > 0)
	{
		if (100*buf_rec <= 2*get("Recuperation"))
		{
			Lag_Vitalite(1);
			if (get("Recuperation") > 80) Lag_Vitalite(3);
			if (get("Recuperation") > 90) Lag_Vitalite(6);
			if (get("Recuperation") > 95) Lag_Vitalite(6);
			Lag_Energie(1);
		}
	}
	if (get("Recuperation") < 0)
	{
		if (100*buf_rec <= -2*get("Recuperation"))
		{
			Lag_Vitalite(-1);
			Lag_Energie(-1);
		}
	}

	//Évolution de la récupération :

	//Test de récupération forcée (potion, …)
	if (get("RecuperationMoyenne") >= 95 || get("RecuperationMoyenne") <= -95) Set_Recuperation(get("RecuperationMoyenne"));

	//Évolution spontanée
	if (buf_rec >= 10)
	{
		//Récuperation trop faible par rapport à la vitalité :
		if (get("Recuperation") < get("RecuperationMoyenne") + (get("Vitalite")-800)/20) Lag_Recuperation(1);
		//Récupération trop importante par rapport à la vitalité :
		if (get("Recuperation") > get("RecuperationMoyenne") + (get("Vitalite")-800)/20) Lag_Recuperation(-1);

		//Augmentation de la Récupération automatique en fonction du niveau d'énergie
		if (get("Energie") > 200) if (!rand()%20) Lag_Recuperation(1);
		if (get("Energie") > 900) if (!rand()%15) Lag_Recuperation(1);
		buf_rec = 0;
	}
    else buf_rec += tools::timeManager::I(1);
}

/* GETTER STATS AND CHARACTERISTICS */

float Individu::get(string field)
{
	float& valueFloat = (*Get_Stats())[field];

	if (valueFloat == Jeu.floatNotFound)
	{
		int valueInt = (*Get_Caracs())[field];
		if (valueInt != Jeu.intNotFound && field != "RecuperationMoyenne")
			valueInt *= 1./2. * (1. + 1.2*get("Vitalite")/1000.);
		if (valueInt != Jeu.intNotFound)
			return (float)valueInt;
		else
			return Jeu.floatNotFound;
	}

	return valueFloat;
}

/* SETTERS DE VITALITÉ, ÉNERGIE ET RÉCUPÉRATION */

void Individu::Set_Vitalite(float vit)
{
	if (vit >= 0 && vit <= 1000) Stats["Vitalite"] = vit;
	if (vit > 1000) Stats["Vitalite"] = 1000;
	if (vit < 0) Stats["Vitalite"] = 0;
}

void Individu::Lag_Vitalite(float lag)
{
	if (lag < 0)
	{
		if (get("Vitalite") >= -lag) Stats["Vitalite"] += lag;
		else
		{
			Set_Vitalite(0);
			Set_Activite(MORT);
		}
	}
	if (lag > 0) Stats["Vitalite"] += lag;

	if (get("Vitalite") > 1000) Set_Vitalite(1000);
}

void Individu::Set_Energie(float ene)
{
	if (ene >= 0 && ene <= 1000) Stats["Energie"] = ene;
}

void Individu::Lag_Energie(float lag)
{
	if (lag < 0)
	{
		if (get("Energie") >= (unsigned)(-lag)) Stats["Energie"] += lag;
		else Set_Energie(0);
	}
	if (lag > 0) Stats["Energie"] += lag;

	if (get("Energie") > 1000) Set_Energie(1000);
}

void Individu::Set_Recuperation(float rec)
{
	if (rec <= -100) Stats["Recuperation"] = -100;
	else if (rec >= 100) Stats["Recuperation"] = 100;
	else Stats["Recuperation"] = rec;
}

void Individu::Lag_Recuperation(float lag)
{
	Stats["Recuperation"] += lag;
	if (get("Recuperation") < -100) Set_Recuperation(-100);
	if (get("Recuperation") > 100) Set_Recuperation(100);
}

void Individu::Disp(RenderTarget& target)
{
	Activite* act = Get_Activite(Act);
    imageManager::display(target, "individuals", act->getImageKey(angle, Num), position().x, position().y, true);
}

void Individu::displayLifeGauge(int x, int y)
{
    Color color;
    if (Diplomatie == DIPLOM_ALLIE) color = Color(128, 255, 128, 255);
    if (Diplomatie == DIPLOM_ENNEMI) color = Color(255, 255, 255, 255);
    Disp_TexteCentre(Get_Nom(), x, y + 25, color, 10.f);

    RectangleShape background(Vector2f(50, 4));
    background.setPosition(x - 25, y + 35);
    background.setFillColor(Color(0, 0, 0, 175));
    Jeu.App.draw(background);

    RectangleShape foreground(Vector2f(get("Vitalite")/20, 4));
    foreground.setPosition(x - 25, y + 35);
    foreground.setFillColor(Color(228, 0, 0, 255));
    Jeu.App.draw(foreground);
}
