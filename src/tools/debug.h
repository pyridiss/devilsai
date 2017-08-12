
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

#ifndef DEBUG_H
#define DEBUG_H

#include <string>
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

using namespace std;

namespace tools{

namespace debug{

void openDebugFile(const string& appName, const string& version);
void addDebugCategory(string category);
void fatal(const string& str, const string& category, const string& file, int line);
void error(const string& str, const string& category, const string& file, int line);
void warning(const string& str, const string& category, const string& file, int line);
void message(const string& str, const string& category, const string& file, int line);

} //namespace debug

} //namespace tools

#endif // DEBUG_H

