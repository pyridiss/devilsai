
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

#ifndef DEVILSAI_GUI_INVENTORY_H
#define DEVILSAI_GUI_INVENTORY_H

namespace sf
{
    class RenderWindow;
    class Event;
}

namespace gui
{
    class Window;
}

class WearableItem;

void manageInventoryScreen(gui::Window& window, sf::RenderWindow& target, sf::Event& event, WearableItem& selectedObject);
void displayInventoryScreen(gui::Window& window, sf::RenderWindow& target, WearableItem& selectedObject);

#endif // DEVILSAI_GUI_INVENTORY_H
