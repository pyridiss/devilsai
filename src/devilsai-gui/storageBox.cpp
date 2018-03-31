
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

#include "tools/math.h"
#include "textManager/textManager.h"
#include "gui/widget.h"
#include "gui/window.h"
#include "imageManager/imageManager.h"

#include "ElementsCarte/ElementsCarte.h"
#include "devilsai-resources/wearableItem.h"
#include "devilsai-resources/pack.h"

#include "gamedata.h"


void manageStorageBoxScreen(gui::Window& window, RenderWindow& target, Event& event, Coffre* box)
{
    auto& playerInventory = gamedata::player()->inventory;
    auto& storageBox = box->objects;

    WearableItem* currentObject = nullptr;

    const auto& slots = window.getWidgets();

    for (const auto& slot : slots)
    {
        if (!slot.second->activated(event)) continue;

        //Click founded in the player inventory; we will try to transfer the object in the storage box
        if (slot.second->embeddedData<string>("owner") == "player")
        {
            currentObject = playerInventory.at(slot.first);
            if (currentObject == nullptr) break;

            //First, we check is the same object exists in the storage box and can be accumulated
            for (auto& i : storageBox.objects)
            {
                if (i.stackable() && i.name() == currentObject->name())
                {
                    i.setQuantity(i.quantity() + currentObject->quantity());
                    playerInventory.deleteObject(*currentObject);
                    currentObject = nullptr;
                    break;
                }
            }

            if (currentObject == nullptr) break;

            //Try to find an empty slot
            for (const auto& slot2 : slots)
            {
                if (slot2.second->embeddedData<string>("owner") == "storage-box" && storageBox.at(slot2.first) == nullptr)
                {
                    currentObject->setSlot(slot2.first);
                    storageBox.objects.push_back(std::move(*currentObject));
                    playerInventory.deleteObject(*currentObject);
                    break;
                }
            }
        }

        //Click founded in the storage box; we will try to transfer the object in the player inventory
        if (slot.second->embeddedData<string>("owner") == "storage-box")
        {
            currentObject = storageBox.at(slot.first);
            if (currentObject == nullptr) break;

            //First, we check is the same object exists in the player inventory and can be accumulated.
            for (auto& i : playerInventory.objects)
            {
                if (i.stackable() && i.name() == currentObject->name())
                {
                    i.setQuantity(i.quantity() + currentObject->quantity());
                    storageBox.deleteObject(*currentObject);
                    currentObject = nullptr;
                    break;
                }
            }

            if (currentObject == nullptr) break;

            //Try to find an empty slot
            for (const auto& slot2 : slots)
            {
                if (slot2.second->embeddedData<string>("owner") == "player" && playerInventory.at(slot2.first) == nullptr)
                {
                    currentObject->setSlot(slot2.first);
                    playerInventory.objects.push_back(std::move(*currentObject));
                    storageBox.deleteObject(*currentObject);
                    break;
                }
            }
        }
        break;
    }

    window.checkTriggers();
}

void displayStorageBoxScreen(gui::Window& window, RenderWindow& target, Coffre* box)
{
    gui::Widget* boxName = window.widget("storagebox-name");
    gui::optionType o;
    o.set<textManager::PlainText>(box->Nom);
    boxName->setValue(o);

    window.display(target);

    auto& playerInventory = gamedata::player()->inventory;
    auto& storageBox = box->objects;

    WearableItem* hovering = nullptr;

    const auto& slots = window.getWidgets();

    for (const auto& slot : slots)
    {
        WearableItem* obj = nullptr;
        if (slot.second->embeddedData<string>("owner") == "player") obj = playerInventory.at(slot.first);
        if (slot.second->embeddedData<string>("owner") == "storage-box") obj = storageBox.at(slot.first);
        if (obj != nullptr)
        {
            imageManager::display(target, tools::hash("objectsIcons"), obj->name(), slot.second->left(), slot.second->top());

            if (obj->stackable())
            {
                textManager::RichText number;
                number.setSize(50, 0);
                number.setDefaultProperties("liberation", 12, Color(255, 255, 255));
                number.create(obj->quantity());
                number.displayFullText(target, slot.second->left() + 30, slot.second->top() + 30);
            }

            if (slot.second->mouseHovering())
                hovering = obj;
        }
    }

    if (hovering != nullptr)
        hovering->displayDescription(target);
}
