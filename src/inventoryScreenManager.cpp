
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2017  Quentin Henriet

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

#include "gui/window.h"
#include "imageManager/imageManager.h"

#include "Attributs/Attributs.h"
#include "ElementsCarte/ElementsCarte.h"
#include "Bibliotheque/Bibliotheque.h"

#include "gamedata.h"

#include "inventoryScreenManager.h"

//Defined in Attributs/Objets.cpp
void Disp_Caracs_Objet(RenderWindow& target, lua_State* obj);

void manageInventoryScreen(gui::Window& window, RenderWindow& target, Event& event, lua_State*& selectedObject)
{
    mapObjects* objects = &(gamedata::player()->inventory.objects);
    mapObjects::iterator currentObject = objects->end();

    const auto& slots = window.getWidgets();

    for (const auto& slot : slots)
    {
        if (slot.second->activated(target, event))
        {
            if (selectedObject == nullptr)
            {
                currentObject = objects->find(slot.first);
                if (currentObject != objects->end())
                {
                    if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left)
                    {
                        selectedObject = currentObject->second;
                        objects->erase(currentObject);
                    }
                    if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Right)
                    {
                        //TODO: Erase temporary objects instead of refusing the use of the object.
                        if (objects->find(getStringFromLUA(currentObject->second, "getIdEmplacement")) != objects->end())
                            continue;

                        if (getBoolFromLUA(currentObject->second, "getCumul") && getIntFromLUA(currentObject->second, "getQuantite") > 1)
                        {
                            gamedata::player()->inventory.addObject(getStringFromLUA(currentObject->second, "getFileName"), getStringFromLUA(currentObject->second, "getIdEmplacement"));
                            setIntToLUA(currentObject->second, "setQuantite", getIntFromLUA(currentObject->second, "getQuantite") - 1);
                        }
                        else
                        {
                            setStringToLUA(currentObject->second, "setKey", getStringFromLUA(currentObject->second, "getIdEmplacement"));
                            objects->emplace(getStringFromLUA(currentObject->second, "getIdEmplacement"), currentObject->second);
                            objects->erase(currentObject);
                        }
                    }
                }
            }
            else if (objects->find(slot.first) == objects->end() &&
                     (slot.second->embeddedData("allowed-object") == "all" ||
                      slot.second->embeddedData("allowed-object") == getStringFromLUA(selectedObject, "getTypeObject")))
            {
                setStringToLUA(selectedObject, "setKey", slot.first);
                objects->emplace(slot.first, selectedObject);
                selectedObject = nullptr;
            }
            break;
        }
    }
}

void displayInventoryScreen(gui::Window& window, RenderWindow& target, lua_State*& selectedObject)
{
    window.display(target);

    mapObjects* objects = &(gamedata::player()->inventory.objects);

    lua_State* hovering = nullptr;

    const auto& slots = window.getWidgets();

    for (const auto& slot : slots)
    {
        mapObjects::iterator obj = objects->find(slot.first);
        if (obj != objects->end())
        {
            string objectName = getStringFromLUA(obj->second, "getFileName");
            string imageContainer = slot.second->embeddedData("image-container");
            imageManager::display(target, imageContainer, objectName, slot.second->getXTopLeft(), slot.second->getYTopLeft());

            if (getBoolFromLUA(obj->second, "getCumul"))
            {
                string StrNombre = intToString(getIntFromLUA(obj->second, "getQuantite"));
                Disp_Texte(StrNombre, slot.second->getXTopLeft() + 30, slot.second->getYTopLeft() + 30, Color(255,255,255), 12.);
            }

            if (slot.second->mouseHovering(target))
                hovering = obj->second;
        }
    }

    if (hovering != nullptr)
        Disp_Caracs_Objet(target, hovering);

    if (selectedObject != nullptr)
    {
        string objectName = getStringFromLUA(selectedObject, "getFileName");
        imageManager::display(target, "objects", objectName, Mouse::getPosition(target).x, Mouse::getPosition(target).y, true);
    }
}
