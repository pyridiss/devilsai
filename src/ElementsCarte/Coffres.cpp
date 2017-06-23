
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

#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/Templates.h"
#include "../Carte/Carte.h"
#include "ElementsCarte.h"

#include "tools/timeManager.h"
#include "imageManager/imageManager.h"

/** FONCTIONS DE LA CLASSE Coffre **/

int Coffre::Gestion()
{
	return ETAT_NORMAL;
}

int Coffre::Collision(Individu* elem, int TypeCollision)
{
	if (TypeCollision == COLL_VIS) return COMPORTEMENT_ALEATOIRE;
	return COLL_INTER;
}

void Coffre::Disp(RenderTarget& target, float RefX, float RefY)
{
	return;
}

/** FONCTIONS DE LA CLASSE Cadavre **/

Cadavre::Cadavre() : Coffre()
{
	TypeClassement = CLASSEMENT_CADAVRE;
}

int Cadavre::Gestion()
{
	if (objects.objects.empty()) Vide = true;
	if (Vide && Duree > 5) Duree = 5;

	if (Vide && !objects.objects.empty()) //Un objet a été reposé alors que le cadavre venait d'être vidé…
	{
		Vide = false;
		Duree += 100;
	}

	if (Duree <= 0)
		return ETAT_MORT;
	else
	{
		Duree -= tools::timeManager::I(1/60.);
		return ETAT_NORMAL;
	}
}

void Cadavre::Set_Individu()
{
	Nom = getFormatedTranslatedMessage(_CADAVRE, getTranslatedNameOfElement(Ind_Id));
}

void Cadavre::Set_Individu(string type, string key)
{
	Ind_Id = type;
    imageKey = key;

	Set_Individu();
}

void Cadavre::Disp(RenderTarget& target, float RefX, float RefY)
{
	if (Controle == AI_IMG_HORSCHAMP) return;

    imageManager::display(target, "individuals", imageKey, target.getSize().x/2 - (RefX - position().x), target.getSize().y/2 - (RefY - position().y), true);
}

