
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

#if defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#elif defined(linux) || defined(__linux)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <fstream>

#include "tools/debug.h"
#include "tools/filesystem.h"

namespace tools{

namespace filesystem{

string _dataDirectory;

bool createDirectory(string str)
{
#if defined(_WIN32) || defined(__WIN32__)
    bool result = CreateDirectory(str.c_str(), NULL);
#else
    int err = mkdir(str.c_str(), S_IRWXU);
    bool result = (err == 0) || (errno == EEXIST);
#endif

    if (result)
    {
        tools::debug::message("The directory " + str + " has been created", "files", __FILENAME__, __LINE__);
        return true;
    }

    tools::debug::fatal("Failed to create directory: " + str, "files", __FILENAME__, __LINE__);
    return false;
}

bool removeDirectory(string str)
{
#if defined(_WIN32) || defined(__WIN32__)
    bool result = RemoveDirectory(str.c_str());
#else
    bool result = !(rmdir(str.c_str()));
#endif

    if (result)
    {
        tools::debug::message("The directory " + str + " has been removed", "files", __FILENAME__, __LINE__);
        return true;
    }

    tools::debug::fatal("Failed to remove directory: " + str, "files", __FILENAME__, __LINE__);
    return false;
}

bool removeFile(string str)
{
#if defined(_WIN32) || defined(__WIN32__)
    bool result = DeleteFile(str.c_str());
#else
    bool result = !(unlink(str.c_str()));
#endif

    if (result)
    {
        tools::debug::message("The file " + str + " has been removed", "files", __FILENAME__, __LINE__);
        return true;
    }

    tools::debug::fatal("Failed to remove file: " + str, "files", __FILENAME__, __LINE__);
    return false;
}

bool checkFile(string file)
{
    ifstream f(file);
    return f.good();
}

string getSaveDirectoryPath()
{
#if defined(_WIN32) || defined(__WIN32__)
    return "save/";
#else
    return ".devilsai/";
#endif
    return "";
}

void setDataDirectory(const string& d)
{
    _dataDirectory = d;
}

const string& dataDirectory()
{
    return _dataDirectory;
}

} //namespace filesystem

} //namespace tools
