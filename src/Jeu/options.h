
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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

using namespace std;

namespace gui{
    class Window;
}

namespace options{

void Load_Options();
void Save_Options();
void changeOption(string option, string value);
void updateCurrentSavedGamePack();
void deleteSavedGamePack(string directory);
void initLoadGameWindow(gui::Window& window);
void initOptionsWindow(gui::Window& window);

} //namespace options

#endif // OPTIONS_H
