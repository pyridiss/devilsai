
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
    _highlight = false;
    _closeWhenEmpty = false;
}

int Coffre::Gestion()
{
    Element_Carte::Gestion();

    if (lifetime == 0) return ETAT_MORT;

	return ETAT_NORMAL;
}

int Coffre::Collision(Individu* elem, int TypeCollision)
{
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

    if (!imageContainer.empty() && !Type.empty())
    {
        if (!_highlight)
            imageManager::display(target, imageContainer, Type, position().x, position().y, true);
        else
            imageManager::display(target, imageContainer, Type, position().x, position().y, true, gui::style::getContrastShader(1.8, 1.8, 1.8));
    }

    _highlight = false;
}

void Coffre::highlight(RenderTarget& target)
{
    _highlight = true;
}

bool Coffre::empty()
{
    return objects.objects.empty();
}

void Coffre::close()
{
    if (_closeWhenEmpty && empty())
    {
        inert = true;
        if (imageContainer == "individuals")
            lifetime = 10000;
    }
}

void Coffre::Set_Individu(string species, string key)
{
    Nom = tools::textManager::getFormattedText("devilsai", "CADAVRE", tools::textManager::getText("species", species));
    imageContainer = "individuals";
    Type = key;
    TypeClassement = CLASSEMENT_CADAVRE;
    _closeWhenEmpty = true;
}
