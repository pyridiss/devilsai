
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2016  Quentin Henriet

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

#ifndef GUI_STYLE_H
#define GUI_STYLE_H

namespace sf{
    template<typename T> class Vector2;
    class RenderWindow;
}

namespace gui{

template<typename T>
void parameterize(unsigned int hash, T value);
template<typename T>
T parameter(unsigned int hash);

bool initLibrary();

void updateMousePosition(sf::RenderWindow& app);
const sf::Vector2<int>& mousePosition();

} //namespace gui

#endif // GUI_STYLE_H

