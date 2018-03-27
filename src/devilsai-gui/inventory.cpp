
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

#include "ElementsCarte/ElementsCarte.h"
#include "devilsai-resources/wearableItem.h"
#include "devilsai-resources/pack.h"

#include "gamedata.h"


void manageInventoryScreen(gui::Window& window, RenderWindow& target, Event& event, WearableItem& selectedObject)
{
    auto& inventory = gamedata::player()->inventory;
    auto& objects = gamedata::player()->inventory.objects;
    WearableItem* currentObject = nullptr;

    const auto& slots = window.getWidgets();

    for (const auto& slot : slots)
    {
        if (slot.second->activated(event))
        {
            if (!selectedObject.valid())
            {
                currentObject = inventory.at(slot.first);
                if (currentObject != nullptr)
                {
                    if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left)
                    {
                        selectedObject = std::move(*currentObject);
                        inventory.deleteObject(*currentObject);
                    }
                    if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Right)
                    {
                        //TODO: Erase temporary objects instead of refusing the use of the object.
                        if (inventory.at(currentObject->requiredSlot()) != nullptr)
                            continue;

                        if (currentObject->stackable() && currentObject->quantity() > 1)
                        {
                            inventory.addObject(currentObject->name(), currentObject->requiredSlot());
                            currentObject->setQuantity(currentObject->quantity() - 1);
                        }
                        else
                        {
                            currentObject->setSlot(currentObject->requiredSlot());
                        }
                    }
                }
            }
            else if (inventory.at(slot.first) == nullptr &&
                     (slot.second->embeddedData<string>("allowed-object") == "all" ||
                      slot.second->embeddedData<string>("allowed-object") == selectedObject.requiredSlot()))
            {
                selectedObject.setSlot(slot.first);
                objects.push_back(std::move(selectedObject));
            }
            break;
        }
    }

    window.checkTriggers();
}

void displayInventoryScreen(gui::Window& window, RenderWindow& target, WearableItem& selectedObject)
{
    window.display(target);

    WearableItem* hovering = nullptr;

    const auto& slots = window.getWidgets();

    for (const auto& slot : slots)
    {
        auto obj = gamedata::player()->inventory.at(slot.first);
        if (obj != nullptr)
        {
            const string& objectName = obj->name();
            const string& imageContainer = slot.second->embeddedData<string>("image-container");
            imageManager::display(target, imageContainer, objectName, slot.second->left(), slot.second->top());

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

    if (selectedObject.valid())
    {
        string objectName = selectedObject.name();
        imageManager::display(target, "objects", objectName, gui::mousePosition().x, gui::mousePosition().y, true);
    }
}
