
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
#include "tools/textManager.h"

#include "Bibliotheque/Templates.h"

#include "config.h"

using namespace tinyxml2;

namespace tools{

namespace textManager{

typedef map < string, String32 > Container;
typedef map < string, Container > Database;

Database texts;
String32 emptyString;

void loadFile(string container, string path)
{
    Database::iterator c = texts.find(container);

    if (c == texts.end())
    {
        Container ctnr;
        texts.insert(Database::value_type(container, ctnr));
    }

    c = texts.find(container);

    path = INSTALL_DIR + path;

    XMLDocument file;
    file.LoadFile(path.c_str());

    XMLHandle hdl(file);
    XMLElement *elem = hdl.FirstChildElement().FirstChildElement().ToElement();

    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "text")
        {
            string id = "", text = "";
            id = elem->Attribute("id");
            text = elem->GetText();
            stringstream ss;
            ss << text;
            String32 text32;
            ss >> text32;

            c->second.insert(Container::value_type(id, text32));
        }

        elem = elem->NextSiblingElement();
    }
}

String32& getText(string file, string id)
{
    Database::iterator c = texts.find(file);

    if (c == texts.end())
    {
        tools::debug::error("File has not been loaded: " + file, "files");
        return emptyString;
    }

    Container::iterator i = c->second.find(id);

    if (i == c->second.end())
    {
        tools::debug::error("Text not found: " + file + "::" + id, "languages");
        return emptyString;
    }

    return i->second;
}


} //namespace textManager

} //namespace tools