
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

#include "tools/timeManager.h"
#include "tools/textManager.h"
#include "imageManager/imageManager.h"
#include "gui/style.h"

#include "Jeu/options.h"

#include "ElementsCarte.h"

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

int Coffre::Collision(Individu* elem, bool apply)
{
    if (apply && elem->Type == "intern")
    {
        if (inert) return COLL_OK;
        else return COLL_INTER;
    }

    if (TypeClassement == CLASSEMENT_NORMAL)
        return COLL_PRIM;

    return COLL_OK;
}

void Coffre::loadFromXML(tinyxml2::XMLHandle &handle)
{
    XMLElement *elem = handle.ToElement();

    if (elem->Attribute("design"))
        Type = elem->Attribute("design");

    double x = 0, y = 0;
    elem->QueryAttribute("x", &x);
    elem->QueryAttribute("y", &y);
    move(x, y);

    if (elem->Attribute("tag"))
        Liste = elem->Attribute("tag");

    elem = handle.FirstChildElement().ToElement();
    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "shape")
        {
            size.loadFromXML(elem);
        }
        if (elemName == "properties")
        {
            elem->QueryAttribute("id", &Id);
            elem->QueryAttribute("lifetime", &lifetime);
            elem->QueryAttribute("inert", &inert);
            elem->QueryAttribute("ignoreCollision", &ignoreCollision);
            elem->QueryAttribute("classement", &TypeClassement);
            if (elem->Attribute("imageContainer"))
                imageContainer = elem->Attribute("imageContainer");
            if (elem->Attribute("name"))
                Nom = tools::textManager::getText("species", elem->Attribute("name"));
            if (elem->Attribute("inlineName"))
                Nom = tools::textManager::fromStdString(elem->Attribute("inlineName"));
            elem->QueryAttribute("closeWhenEmpty", &_closeWhenEmpty);
        }
        if (elemName == "inventory")
        {
            objects.loadFromXML(elem);
        }

        elem = elem->NextSiblingElement();
    }
}

void Coffre::saveToXML(XMLDocument& doc, XMLHandle& handle)
{
    XMLElement* root = handle.ToElement();

    XMLElement* storageBox = doc.NewElement("storageBox");
    storageBox->SetAttribute("design", Type.c_str());
    storageBox->SetAttribute("x", position().x);
    storageBox->SetAttribute("y", position().y);
    storageBox->SetAttribute("tag", Liste.c_str());

    XMLElement* shape = doc.NewElement("shape");
    storageBox->InsertEndChild(shape);
    XMLHandle shapeHandle(shape);
    size.saveToXML(doc, shapeHandle);

    XMLElement* properties = doc.NewElement("properties");
    properties->SetAttribute("id", Id);
    properties->SetAttribute("lifetime", lifetime);
    properties->SetAttribute("inert", inert);
    properties->SetAttribute("ignoreCollision", ignoreCollision);
    properties->SetAttribute("classement", TypeClassement);
    properties->SetAttribute("imageContainer", imageContainer.c_str());
    properties->SetAttribute("inlineName", tools::textManager::toStdString(Nom).c_str());
    properties->SetAttribute("closeWhenEmpty", _closeWhenEmpty);
    storageBox->InsertEndChild(properties);

    XMLHandle hdl(storageBox);
    objects.saveToXML(doc, hdl);

    root->InsertEndChild(storageBox);
}

void Coffre::Disp(RenderTarget& target)
{
    if (options::option<bool>("displayShapes"))
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
