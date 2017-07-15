
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

#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/Bibliotheque.h"
#include "../Jeu/Jeu.h"
#include "../Carte/Carte.h"
#include "ElementsCarte.h"

#include "tools/timeManager.h"
#include "tools/textManager.h"
#include "imageManager/imageManager.h"

using namespace tinyxml2;

/** FONCTIONS DE LA CLASSE Coffre **/

Coffre::Coffre() : Element_Carte()
{
    inert = false;
}

int Coffre::Gestion()
{
	return ETAT_NORMAL;
}

int Coffre::Collision(Individu* elem, int TypeCollision)
{
    if (TypeCollision == COLL_PRIM) return COLL_PRIM;
    return COMPORTEMENT_ALEATOIRE;
}

void Coffre::loadFromXML(tinyxml2::XMLHandle &handle)
{
}

void Coffre::saveToXML(XMLDocument& doc, XMLHandle& handle)
{
}

void Coffre::Disp(RenderTarget& target)
{
    if (Options.displayShapes)
        size.display(target, Color(0, 0, 255, 50));
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
    Nom = tools::textManager::getFormattedText("devilsai", "CADAVRE", tools::textManager::getText("species", Ind_Id));
}

void Cadavre::Set_Individu(string type, string key)
{
	Ind_Id = type;
    imageKey = key;

	Set_Individu();
}

void Cadavre::Disp(RenderTarget& target)
{
    imageManager::display(target, "individuals", imageKey, position().x, position().y, true);
}

