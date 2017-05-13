
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

#include <iostream>
#include <fstream>

#include "tools/debug.h"

namespace tools{

namespace debug{

void fatal(string str, string category)
{
    error(str, category);
}

void error(string str, string category)
{
    ofstream file("Devilsai_Errors", ios_base::out | ios_base::app);

    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    file << "On " << asctime (timeinfo) << " (EE) " << category << " – " << str << endl << endl;

    file.close();
}

void warning(string str, string category)
{
    error(str, category);
}

void message(string str, string category)
{
    cout << category << " – " << str << endl;
}

} //namespace debug

} //namespace tools
