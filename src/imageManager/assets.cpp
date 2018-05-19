
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

#include "tools/debug.h"
#include "tools/filesystem.h"

using namespace std;
using namespace sf;


namespace multimedia{

static unordered_map<string, Font> fonts;
static unordered_map<string, ShaderInstance*> shaders;


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

void addShader(const string& name, ShaderInstance* i)
{
    shaders.emplace(name, i);
}

unsigned int createShaderInstance(const string& name)
{
    auto i = shaders.find(name);
    if (i == shaders.end()) return 0;

    return i->second->createNewInstance();
}

Shader* shader(const string& name, unsigned int instance)
{
    auto i = shaders.find(name);
    if (i == shaders.end()) return nullptr;

    return i->second->shader(instance);
}

void applyShaderOnScreen(RenderWindow& app, const string& shaderName, unsigned int instance, int x, int y, int w, int h)
{
    auto i = shaders.find(shaderName);
    if (i == shaders.end()) return;

    i->second->applyOnScreen(instance, app, x, y, w, h);
}

}  // namespace multimedia
