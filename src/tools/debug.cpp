
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

#include "tools/filesystem.h"
#include "tools/debug.h"

using namespace sf;

namespace tools{

namespace debug{

ofstream logfile;
Clock clock;

unordered_set<string> debugCategories;

void openDebugFile(const string& appName, const string& version)
{
    logfile.open(tools::filesystem::getSaveDirectoryPath() + appName + ".log", ios_base::out);
    logfile.precision(4);
    logfile.setf(std::ios::fixed, std::ios::floatfield);

    clock.restart();

    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);

    logfile << "=================================================" << endl;
    logfile << "=== " << appName << " logfile" << endl;
    logfile << "=== version : " << version << endl;
    logfile << "=== debug file opened on " << asctime(timeinfo);
    logfile << "=================================================" << endl;

    cout.rdbuf(logfile.rdbuf());
    cerr.rdbuf(logfile.rdbuf());
}

void addDebugCategory(string category)
{
    debugCategories.insert(std::move(category));
}

void writeToDebugFile(const string& s, const string& file, int line)
{
    logfile << "[" << clock.getElapsedTime().asSeconds() << "] @" + file + ":" + to_string(line) + " – ";
    logfile << s << endl;
}

void fatal(const string& str, const string& category, const string& file, int line)
{
    error(str, category, file, line);
}

void error(const string& str, const string& category, const string& file, int line)
{
    writeToDebugFile("(EE) " + str, file, line);
}

void warning(const string& str, const string& category, const string& file, int line)
{
    if (debugCategories.empty()) return;
    if (debugCategories.find(category) == debugCategories.end() && debugCategories.find("all") == debugCategories.end()) return;

    writeToDebugFile("(WW) " + str, file, line);
}

void message(const string& str, const string& category, const string& file, int line)
{
    if (debugCategories.empty()) return;
    if (debugCategories.find(category) == debugCategories.end() && debugCategories.find("all") == debugCategories.end()) return;

    writeToDebugFile("(II) " + str, file, line);
}

} //namespace debug

} //namespace tools
