
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

class PlainText;
class RichText;

enum Flags
{
    HAlignRight = 1 << 0,
    HAlignCenter = 1 << 1,
    HAlignJustify = 1 << 2,
    OriginRight = 1 << 3,
    OriginXCenter = 1 << 4,
    OriginTop = 1 << 5,
    OriginBottom = 1 << 6,
    OriginYCenter = 1 << 7,
    FixedHeight = 1 << 8,
    VAlignCenter = 1 << 9
};

void initLibrary();
String32& FORM();
String32& EOL();
String32& SPACE();
void loadFile(string container, string path);
String32& getText(string file, string id);
String32 getFormattedText(string file, string id, const String32& arg);
String32 getFormattedText(string file, string id, double arg);
String32 fromStdString(const string& original);
string toStdString(const String32& original);
int toInt(const string& original);

} //namespace textManager

} //namespace tools

#endif // TOOLS_TEXTMANAGER_H
