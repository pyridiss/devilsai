
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

#ifndef SKILLBARMANAGER_H
#define SKILLBARMANAGER_H

using namespace std;

namespace sf
{
    class RenderWindow;
    class Event;
}

namespace gui
{
    class Window;
}

void manageSkillbar(gui::Window& window, sf::RenderWindow& target, sf::Event& event);
void displaySkillbar(gui::Window& window, sf::RenderWindow& target);

#endif // SKILLBARMANAGER_H