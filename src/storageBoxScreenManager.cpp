
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

#include "tools/textManager.h"
#include "gui/widget.h"
#include "gui/window.h"
#include "imageManager/imageManager.h"

#include "Attributs/Attributs.h"
#include "ElementsCarte/ElementsCarte.h"
#include "Bibliotheque/Bibliotheque.h"

#include "gamedata.h"

#include "inventoryScreenManager.h"

//Defined in Attributs/Objets.cpp
void Disp_Caracs_Objet(RenderWindow& target, lua_State* obj);

void manageStorageBoxScreen(gui::Window& window, RenderWindow& target, Event& event, Coffre* box)
{
    mapObjects* playerInventory = &(gamedata::player()->inventory.objects);
    mapObjects* storageBox = &(box->objects.objects);

    mapObjects::iterator currentObject;

    const auto& slots = window.getWidgets();

    for (const auto& slot : slots)
    {
        if (!slot.second->activated(target, event)) continue;

        //Click founded in the player inventory; we will try to transfer the object in the storage box
        if (slot.second->embeddedData("owner") == "player")
        {
            currentObject = playerInventory->find(slot.first);
            if (currentObject == playerInventory->end()) break;

            //First, we check is the same object exists in the storage box and can be accumulated
            for (auto& i : *storageBox)
            {
                if (getBoolFromLUA(i.second, "getCumul") && getIntFromLUA(i.second, "getInternalNumber") == getIntFromLUA(currentObject->second, "getInternalNumber"))
                {
                    setIntToLUA(i.second, "setQuantite", getIntFromLUA(i.second, "getQuantite") + getIntFromLUA(currentObject->second, "getQuantite"));
                    gamedata::player()->inventory.deleteObject(currentObject->second);
                    playerInventory->erase(currentObject);
                    currentObject = playerInventory->end();
                    break;
                }
            }

            if (currentObject == playerInventory->end()) break;

            //Try to find an empty slot
            for (const auto& slot2 : slots)
            {
                if (slot2.second->embeddedData("owner") == "storage-box" && storageBox->find(slot2.first) == storageBox->end())
                {
                    storageBox->emplace(slot2.first, currentObject->second);
                    setStringToLUA(currentObject->second, "setKey", slot2.first);
                    playerInventory->erase(currentObject);
                    break;
                }
            }
        }

        //Click founded in the storage box; we will try to transfer the object in the player inventory
        if (slot.second->embeddedData("owner") == "storage-box")
        {
            currentObject = storageBox->find(slot.first);
            if (currentObject == storageBox->end()) break;

            //First, we check is the same object exists in the player inventory and can be accumulated.
            for (auto& i : *playerInventory)
            {
                if (getBoolFromLUA(i.second, "getCumul") && getIntFromLUA(i.second, "getInternalNumber") == getIntFromLUA(currentObject->second, "getInternalNumber"))
                {
                    setIntToLUA(i.second, "setQuantite", getIntFromLUA(i.second, "getQuantite") + getIntFromLUA(currentObject->second, "getQuantite"));
                    box->objects.deleteObject(currentObject->second);
                    storageBox->erase(currentObject);
                    currentObject = storageBox->end();
                    break;
                }
            }

            if (currentObject == storageBox->end()) break;

            //Try to find an empty slot
            for (const auto& slot2 : slots)
            {
                if (slot2.second->embeddedData("owner") == "player" && playerInventory->find(slot2.first) == playerInventory->end())
                {
                    playerInventory->emplace(slot2.first, currentObject->second);
                    setStringToLUA(currentObject->second, "setKey", slot2.first);
                    storageBox->erase(currentObject);
                    break;
                }
            }
        }
        break;
    }
}

void displayStorageBoxScreen(gui::Window& window, RenderWindow& target, Coffre* box)
{
    gui::Widget* boxName = window.widget("storagebox-name");
    boxName->setValue(box->Nom.toStdString());

    window.display(target);

    mapObjects* playerInventory = &(gamedata::player()->inventory.objects);
    mapObjects* storageBox = &(box->objects.objects);

    lua_State* hovering = nullptr;

    const auto& slots = window.getWidgets();

    for (const auto& slot : slots)
    {
        mapObjects::iterator obj = playerInventory->end();
        if (slot.second->embeddedData("owner") == "player") obj = playerInventory->find(slot.first);
        if (slot.second->embeddedData("owner") == "storage-box") obj = storageBox->find(slot.first);
        if (obj != playerInventory->end() && obj != storageBox->end())
        {
            string objectName = getStringFromLUA(obj->second, "getFileName");
            imageManager::display(target, "objectsIcons", objectName, slot.second->left(), slot.second->top());

            if (getBoolFromLUA(obj->second, "getCumul"))
            {
                string StrNombre = intToString(getIntFromLUA(obj->second, "getQuantite"));
                Disp_Texte(StrNombre, slot.second->left() + 30, slot.second->top() + 30, Color(255,255,255), 12.);
            }

            if (slot.second->mouseHovering(target))
                hovering = obj->second;
        }
    }

    if (hovering != nullptr)
        Disp_Caracs_Objet(target, hovering);
}
