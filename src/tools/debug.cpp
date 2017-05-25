
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
#include <unordered_set>

#include <SFML/System.hpp>

#include "config.h"
#include "tools/filesystem.h"
#include "tools/debug.h"

using namespace sf;

namespace tools{

namespace debug{

ofstream logfile;
Clock clock;

unordered_set<string> debugCategories;

void openDebugFile()
{
    logfile.open(tools::filesystem::getSaveDirectoryPath() + "devilsai.log", ios_base::out);
    logfile.precision(4);
    logfile.setf(std::ios::fixed, std::ios::floatfield);

    clock.restart();

    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);

    logfile << "=================================================" << endl;
    logfile << "=== devilsai logfile" << endl;
    logfile << "=== version : " << VERSION << endl;
    logfile << "=== debug file opened on " << asctime(timeinfo);
    logfile << "=================================================" << endl;

    cout.rdbuf(logfile.rdbuf());
    cerr.rdbuf(logfile.rdbuf());
}

void addDebugCategory(string category)
{
    debugCategories.insert(std::move(category));
}

void writeToDebugFile(string s)
{
    logfile << "[" << clock.getElapsedTime().asSeconds() << "] ";
    logfile << s << endl;
}

void fatal(string str, string category)
{
    error(str, category);
}

void error(string str, string category)
{
    writeToDebugFile("(EE) " + category + " – " + str);
}

void warning(string str, string category)
{
    if (debugCategories.empty()) return;
    if (debugCategories.find(category) == debugCategories.end()) return;

    writeToDebugFile("(WW) " + category + " – " + str);
}

void message(string str, string category)
{
    if (debugCategories.empty()) return;
    if (debugCategories.find(category) == debugCategories.end()) return;

    writeToDebugFile("(II) " + category + " – " + str);
}

} //namespace debug

} //namespace tools
