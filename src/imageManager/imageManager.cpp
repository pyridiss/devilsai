
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2014  Quentin Henriet

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

#include <physfs.h>

#include "tools/debug.h"
#include "tools/filesystem.h"

#include "imageManager/imageManager.h"
#include "imageManager/image.h"
#include "imageManager/animation.h"
#include "imageManager/HSL.h"

namespace imageManager{

typedef map < string, imageManager::Image > Container;
typedef map < string, Container > Database;
typedef map < string, imageManager::Animation > AnimationDatabase;

Database images;
AnimationDatabase animations;
string currentArchiveFile;

void addContainer(string container)
{
    Database::iterator c = images.find(container);

    if (c == images.end())
    {
        Container ctnr;
        images.insert(Database::value_type(container, ctnr));
    }
}

void addImage(string container, string key, string file, Vector2i of)
{
    Database::iterator c = images.find(container);

    if (c == images.end())
    {
        tools::debug::error("Image container has not been created yet: " + container, "images", __FILENAME__, __LINE__);
        return;
    }

    Container::iterator i = (*c).second.find(key);

    if (i == (*c).second.end())
    {
        imageManager::Image img;
        const auto& result = (*c).second.insert(Container::value_type(key, img));
        result.first->second.set(file, of);
        tools::debug::message("Image " + container + "::" + key + " has been added.", "images", __FILENAME__, __LINE__);
    }
}

void addArchiveFile(string path)
{
    path = tools::filesystem::dataDirectory() + path;

    if (PHYSFS_addToSearchPath(path.c_str(), 1) == 0)
    {
        tools::debug::error("Unable to load file: " + path, "images", __FILENAME__, __LINE__);
    }
    else
    {
        tools::debug::message("File \"" + path + "\" open", "files", __FILENAME__, __LINE__);
        currentArchiveFile = path;
    }
}

string getCurrentArchiveFile()
{
    return currentArchiveFile;
}

void removeArchiveFile(string path)
{
    path = tools::filesystem::dataDirectory() + path;
    PHYSFS_removeFromSearchPath(path.c_str());
    currentArchiveFile = "";
}

imageManager::Image* getImage(string container, string key)
{
    Database::iterator c = images.find(container);

    if (c == images.end())
    {
        tools::debug::error("Container not found: " + container, "images", __FILENAME__, __LINE__);
        return nullptr;
    }

    Container::iterator i = (*c).second.find(key);

    if (i == (*c).second.end())
    {
        tools::debug::error("Image not found: " + container + "::" + key, "images", __FILENAME__, __LINE__);
        return nullptr;
    }

    return &(i->second);
}

void changeHSL(string container, string key, double h, double s, double l)
{
    Image* i = getImage(container, key);

    if (i != nullptr)
    {
        HSL hsl(h, s, l);
        i->sprite.setColor(hsl.turnToRGB());
    }
}

void display(RenderTarget& target, string container, string key, float x, float y, bool atCenter, const Shader* shader)
{
    Database::iterator c = images.find(container);

    if (c == images.end())
    {
        tools::debug::error("Image container has not been added yet: " + container, "images", __FILENAME__, __LINE__);
        return;
    }

    Container::iterator i = (*c).second.find(key);

    if (i == (*c).second.end())
    {
        tools::debug::error("Image '" + container + "::" + key + "' has not been loaded yet, skipping...", "images", __FILENAME__, __LINE__);
        return;
    }

    (*i).second.display(target, x, y, atCenter, shader);

    tools::debug::message("Image '" + container + "::" + key + "' displayed on screen.", "images", __FILENAME__, __LINE__);
}

void addAnimation(string name, string file)
{
    AnimationDatabase::iterator a = animations.find(name);

    if (a == animations.end())
    {
        imageManager::Animation ani;
        const auto& result = animations.insert(AnimationDatabase::value_type(name, ani));
        result.first->second.image.set(file, nullptr, Vector2i(0, 0));
    }
}

imageManager::Animation* getAnimation(string name)
{
    AnimationDatabase::iterator a = animations.find(name);

    if (a == animations.end())
    {
        tools::debug::error("Animation not found: " + name, "images", __FILENAME__, __LINE__);
        return nullptr;
    }

    return &(a->second);
}

} //namespace imageManager
