
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

#include "../Bibliotheque/Bibliotheque.h"
#include "Attributs.h"

#include "tools/math.h"


using namespace tinyxml2;

/* Manager for objects */

Objects::Objects()
  : objects(),
    designs()
{
}

Objects::Objects(const Objects& other)
  : objects(),
    designs(other.designs)
{
    for (auto& i : other.objects)
    {
        WearableItem& o = addObject(i.name(), i.currentSlot());
        o.recoverState(i.currentState());
    }
}

Objects& Objects::operator=(const Objects& right)
{
    designs = right.designs;

    for (auto& i : right.objects)
    {
        WearableItem& o = addObject(i.name(), i.currentSlot());
        o.recoverState(i.currentState());
    }

    return *this;
}

Objects::~Objects()
{
	deleteObjects();
}

WearableItem& Objects::addObject(string newObject, string key, int qualityRandomObject)
{
    WearableItem newItem;
    newItem.create(newObject);
    newItem.generateRandomItem(qualityRandomObject);
    newItem.setSlot(key);

    objects.push_back(std::move(newItem));

    return objects.back();
}

void Objects::deleteObject(const WearableItem& w)
{
    for (auto i = objects.begin() ; i != objects.end() ; ++i)
    {
        if ((*i) == w)
        {
            objects.erase(i);
            break;
        }
    }
}

void Objects::deleteObjects()
{
	objects.clear();
}

WearableItem* Objects::at(const string& slot)
{
    for (auto& i : objects)
        if (i.currentSlot() == slot)
            return &i;

    return nullptr;
}

void Objects::loadFromXML(XMLElement* elem)
{
    deleteObjects();

    XMLHandle hdl(elem);
    XMLElement *object = hdl.FirstChildElement().ToElement();
    while (object)
    {
        string objectName = object->Name();

        if (objectName == "addObject")
        {
            string slot = object->Attribute("slot");
            string design = object->Attribute("design");
            int q = 0;
            object->QueryAttribute("quality", &q);
            WearableItem& o = addObject(design, slot, q);

            if (object->Attribute("data"))
                o.recoverState(object->Attribute("data"));
        }
        if (objectName == "objectDesign")
        {
            string design = object->Attribute("design");
            int q = 0, p = 0;
            object->QueryAttribute("quality", &q);
            object->QueryAttribute("probability", &p);
            designs.emplace_back(design, q, p);
        }
        if (objectName == "createRandomObjects")
        {
            int min = 0, max = 0;
            object->QueryAttribute("min", &min);
            object->QueryAttribute("max", &max);
            int n = tools::math::randomNumber(min, max);

            int designsCount = 0;
            XMLElement* currentDesign = object->FirstChildElement();
            while (currentDesign)
            {
                ++designsCount;
                currentDesign = currentDesign->NextSiblingElement();
            }
            for (int i = 0 ; i < n ; ++i)
            {
                int selectedDesign = rand()%designsCount;
                currentDesign = object->FirstChildElement();
                for (int j = 0 ; j < selectedDesign ; ++j)
                    currentDesign = currentDesign->NextSiblingElement();

                string design = currentDesign->Attribute("design");
                int q = 0;
                currentDesign->QueryAttribute("quality", &q);
                designs.emplace_back(design, q, 100);
            }
            createObjectsFromDesigns();
        }

        object = object->NextSiblingElement();
    }
}

void Objects::createObjectsFromDesigns()
{
    int key = objects.size() + 1;

    for (auto& d : designs)
    {
        if (rand()%100 < d.probability)
        {
            addObject(d.file, "storagebox" + intToString(key, 2), d.quality);
            ++key;
        }
    }
    designs.clear();
}

void Objects::saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle)
{
    XMLElement* root = handle.ToElement();

    XMLElement* inventory = doc.NewElement("inventory");

    for (auto& o : objects)
    {
        XMLElement* object = doc.NewElement("addObject");

        object->SetAttribute("slot", o.currentSlot().c_str());
        object->SetAttribute("design", o.name().c_str());
        object->SetAttribute("data", o.currentState().c_str());

        inventory->InsertEndChild(object);
    }

    root->InsertEndChild(inventory);
}
