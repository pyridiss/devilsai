
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

#ifndef TOOLS_TEXTMANAGER_H
#define TOOLS_TEXTMANAGER_H

#include <SFML/System/Utf.hpp>

using namespace std;
using namespace sf;

typedef basic_string<Uint32> String32;

namespace tools{

namespace textManager{

void initLibrary();
void loadFile(string container, string path);
String32& getText(string file, string id);
String32 getFormattedText(string file, string id, const String32& arg);
String32 getFormattedText(string file, string id, double arg);

} //namespace textManager

} //namespace tools

#endif // TOOLS_TEXTMANAGER_H
