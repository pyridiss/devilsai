
/*
    devilsai - An Action-RPG game
    Copyright (C) 2009-2018  Quentin Henriet

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

#include "imageManager/imageManager.h"

#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "tools/filesystem.h"

using namespace std;
using namespace sf;


namespace multimedia{

static unordered_map<string, Font> fonts;


void addFont(const string& name, const string& file)
{
    auto i = fonts.try_emplace(name);
    i.first->second.loadFromFile(tools::filesystem::dataDirectory() + file);
}

Font* font(const string& name)
{
    auto i = fonts.find(name);
    if (i == fonts.end()) return nullptr;

    return &(i->second);
}

}  // namespace multimedia
