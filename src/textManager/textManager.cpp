
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

#include <map>
#include <sstream>

#include <tinyxml2.h>

#include "tools/debug.h"
#include "tools/filesystem.h"

#include "textManager/plainText.h"
#include "textManager/textManager.h"

using namespace tinyxml2;

namespace textManager{

typedef map < string, textManager::PlainText > Container;
typedef map < string, Container > Database;

Database texts;
textManager::PlainText emptyString;


void loadFile(string container, string path, const string& language)
{
    Database::iterator c = texts.find(container);

    if (c == texts.end())
    {
        Container ctnr;
        texts.insert(Database::value_type(container, ctnr));
    }

    c = texts.find(container);

    XMLDocument file;
    file.LoadFile((tools::filesystem::dataDirectory() + path).c_str());

    XMLHandle hdl(file);
    XMLElement *elem = hdl.FirstChildElement().FirstChildElement().ToElement();

    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "loadFile")
        {
            string lng = elem->Attribute("language");
            string f = elem->Attribute("file");
            if (lng == language)
                loadFile(container, f, language);
        }
        if (elemName == "text")
        {
            string id = "", text = "";
            id = elem->Attribute("id");
            if (elem->GetText())
                text = elem->GetText();

            c->second.emplace(id, text);
        }

        elem = elem->NextSiblingElement();
    }
}

textManager::PlainText& getText(string file, string id)
{
    Database::iterator c = texts.find(file);

    if (c == texts.end())
    {
        tools::debug::error("File has not been loaded: " + file, "files", __FILENAME__, __LINE__);
        return emptyString;
    }

    Container::iterator i = c->second.find(id);

    if (i == c->second.end())
    {
        tools::debug::error("Text not found: " + file + "::" + id, "languages", __FILENAME__, __LINE__);
        return emptyString;
    }

    return i->second;
}

PlainText fromStdString(const string& original)
{
    return PlainText(original);
}

int toInt(const string& original)
{
    istringstream str(original);
    int i;
    str >> i;

    return i;
}

string toString(double n, int stringLength)
{
    stringstream out;
    out.precision(8);
    if (stringLength != -1)
    {
        out.width(stringLength);
        out.fill('0');
    }
    out << n;
    return out.str();
}

} //namespace textManager
