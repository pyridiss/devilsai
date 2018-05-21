
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

#include <SFML/Graphics/Color.hpp>
#include <tinyxml2.h>

#include "tools/debug.h"
#include "tools/math.h"
#include "imageManager/imageManager.h"

#include "devilsai-resources/manager.h"
#include "Jeu/options.h"

#include "ElementsCarte.h"

using namespace tinyxml2;

/** FONCTIONS DE LA CLASSE Paysage **/

Paysage::Paysage()
  : Element_Carte(),
    extent(1, 1)
{
	TypeClassement = CLASSEMENT_NORMAL;
    inert = true;
    size.setOrigin(&position());
}

Paysage::Paysage(string id)
  : Paysage()
{
    Type = std::move(id);
}

int Paysage::Collision([[maybe_unused]] Individu *elem, [[maybe_unused]] bool apply)
{
	return COLL_PRIM;
}

void Paysage::saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle)
{
    XMLElement* root = handle.ToElement();

    XMLElement* inertItem = doc.NewElement("inertItem");
    inertItem->SetAttribute("design", Type.c_str());
    inertItem->SetAttribute("x", position().x);
    inertItem->SetAttribute("y", position().y);
    inertItem->SetAttribute("tag", Liste.c_str());

    XMLElement* shape = doc.NewElement("shape");
    inertItem->InsertEndChild(shape);
    XMLHandle shapeHandle(shape);
    size.saveToXML(doc, shapeHandle);

    XMLElement* properties = doc.NewElement("properties");
    properties->SetAttribute("id", Id);
    properties->SetAttribute("lifetime", lifetime);
    properties->SetAttribute("ignoreCollision", ignoreCollision);
    properties->SetAttribute("classement", TypeClassement);
    properties->SetAttribute("xExtent", extent.x);
    properties->SetAttribute("yExtent", extent.y);
    inertItem->InsertEndChild(properties);

    root->InsertEndChild(inertItem);
}

void Paysage::Disp(RenderTarget& target)
{
    if (options::option<bool>(tools::hash("displayShapes")))
        size.display(target, Color(255, 255, 255, 50));

    if (extent.x == 1 && extent.y == 1)
        imageManager::display(target, tools::hash("paysage"), Type, position().x, position().y, true);
    else
        imageManager::displayRepeatedly(target, "paysage"_hash, Type, position().x, position().y, extent.x, extent.y, true);
}

void Paysage::loadFromXML(tinyxml2::XMLHandle &handle)
{
    XMLElement *elem = handle.ToElement();

    if (elem->Attribute("design"))
    {
        Type = elem->Attribute("design");

        Paysage* design = devilsai::getResource<Paysage>(Type);
        if (design == nullptr)
        {
            tools::debug::error("Cannot initiate Environment '" + Type + "': design does not exist.", "resources", __FILENAME__, __LINE__);
            return;
        }

        size = design->size;
        //Restore size origin, as the copy breaks the link.
        size.setOrigin(&position());

        TypeClassement = design->TypeClassement;
        ignoreCollision = design->ignoreCollision;
        Diplomatie = 0;
    }

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
        if (elemName == "image")
        {
            int xAlignment = 0, yAlignment = 0;
            string archive = "";
            if (elem->Attribute("archive")) archive = elem->Attribute("archive");
            string path = elem->Attribute("imageFile");
            elem->QueryAttribute("xAlignment", &xAlignment);
            elem->QueryAttribute("yAlignment", &yAlignment);

            if (!archive.empty()) imageManager::addArchiveFile(archive);
            imageManager::addImage(tools::hash("paysage"), Type, path, Vector2i(xAlignment, yAlignment));
            if (!archive.empty()) imageManager::removeArchiveFile(archive);
        }
        if (elemName == "properties")
        {
            elem->QueryAttribute("id", &Id);
            elem->QueryAttribute("lifetime", &lifetime);
            elem->QueryAttribute("ignoreCollision", &ignoreCollision);
            elem->QueryAttribute("classement", &TypeClassement);
            elem->QueryAttribute("xExtent", &extent.x);
            elem->QueryAttribute("yExtent", &extent.y);
        }

        elem = elem->NextSiblingElement();
    }
}
