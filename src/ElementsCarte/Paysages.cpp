
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

#include <tinyxml2.h>

#include "../Bibliotheque/Bibliotheque.h"
#include "../Bibliotheque/Constantes.h"
#include "../Jeu/Jeu.h"
#include "../Carte/Carte.h"
#include "ElementsCarte.h"

#include "imageManager/imageManager.h"
#include "imageManager/image.h"

using namespace tinyxml2;

/** FONCTIONS DE LA CLASSE Paysage **/

Paysage::Paysage() : Element_Carte()
{
	TypeClassement = CLASSEMENT_NORMAL;
    inert = true;
    extent.x = 1;
    extent.y = 1;
    size.setOrigin(&position());
}

Paysage::~Paysage()
{
}

int Paysage::Gestion()
{
	//Test de proximité au joueur
	int Retour = Element_Carte::Gestion();
	if (Retour != ETAT_CONTINUER) return Retour;

	return ETAT_NORMAL;
}

int Paysage::Collision(Individu *elem, int TypeCollision)
{
	if (TypeCollision == COLL_VIS) return COMPORTEMENT_ALEATOIRE;
	return COLL_PRIM;
}

void Paysage::Disp(RenderTarget& target)
{
    if (Options.displayShapes)
        size.display(target, Color(255, 255, 255, 50));

    if (extent.x == 1 && extent.y == 1)
        imageManager::display(target, "paysage", Type, position().x, position().y, true);

    else
    {
        Vector2u imageDimension = imageManager::getImage("paysage", Type)->getSize();
        if (extent.x > 1)
        {
            if (extent.x % 2 == 0)
                for (float i = -extent.x/2 ; i < extent.x/2 ; ++i)
                    imageManager::display(target, "paysage", Type, position().x + (i+0.5)*imageDimension.x, position().y, true);
            else
                for (float i = -(extent.x-1)/2 ; i <= (extent.x-1)/2 ; ++i)
                    imageManager::display(target, "paysage", Type, position().x + i*imageDimension.x, position().y, true);
        }
        else if (extent.y > 1)
        {
            if (extent.y % 2 == 0)
                for (float i = -extent.y/2 ; i < extent.y/2 ; ++i)
                    imageManager::display(target, "paysage", Type, position().x, position().y + (i+0.5)*imageDimension.y, true);
            else
                for (float i = -(extent.y-1)/2 ; i <= (extent.y-1)/2 ; ++i)
                    imageManager::display(target, "paysage", Type, position().x, position().y + i*imageDimension.y, true);
        }
    }
}

/** Class Door **/

Door::Door() : Element_Carte()
{
	TypeClassement = CLASSEMENT_NORMAL;
}
Door::~Door()
{
}

int Door::Gestion()
{
	//Test de proximité au joueur
	int Retour = Element_Carte::Gestion();
	if (Retour != ETAT_CONTINUER) return Retour;

	return ETAT_NORMAL;
}

int Door::Collision(Individu *elem, int TypeCollision)
{
	if (TypeCollision == COLL_VIS) return COMPORTEMENT_ALEATOIRE;
	if (deniedDiplomacy == elem->Diplomatie) return COLL_PRIM;

	return COLL_OK;
}

void Door::Disp(RenderTarget& target)
{
    if (Options.displayShapes)
        size.display(target, Color(0, 255, 255, 50));

	return;
}


void Paysage::loadFromXML(tinyxml2::XMLHandle &handle)
{
    XMLElement *elem = handle.ToElement();

    if (elem->Attribute("design"))
    {
        Type = elem->Attribute("design");
        copyInertItemFromTemplate(Type, this);
    }

    double x = 0, y = 0;
    elem->QueryAttribute("x", &x);
    elem->QueryAttribute("y", &y);
    move(x, y);

    elem = handle.FirstChildElement().ToElement();
    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "shape")
        {
            size.loadFromXML(elem);
        }
        if (elemName == "image")
        {
            int xAlignment = 0, yAlignment = 0;
            string archive = "";
            if (elem->Attribute("archive")) archive = elem->Attribute("archive");
            string path = elem->Attribute("imageFile");
            elem->QueryAttribute("xAlignment", &xAlignment);
            elem->QueryAttribute("yAlignment", &yAlignment);

            if (!archive.empty()) imageManager::addArchiveFile(archive);
            imageManager::addImage("paysage", Type, path, Vector2i(xAlignment, yAlignment));
            if (!archive.empty()) imageManager::removeArchiveFile(archive);
        }
        if (elemName == "properties")
        {
            elem->QueryAttribute("classement", &TypeClassement);
            elem->QueryAttribute("xExtent", &extent.x);
            elem->QueryAttribute("yExtent", &extent.y);
        }

        elem = elem->NextSiblingElement();
    }
}
