
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

#include "imageManager/imageManager.h"

#include <atomic>
#include <thread>

#include <physfs.h>

#include "tools/debug.h"
#include "tools/filesystem.h"

#include "gui/style.h"

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
bool Colorize = false;
Vector3f ColorizeRed, ColorizeGreen, ColorizeBlue;

std::atomic_flag Mutex_lock = ATOMIC_FLAG_INIT;
std::atomic<int> Mutex_id = 0;

void addContainer(const string& container)
{
    Database::iterator c = images.find(container);

    if (c == images.end())
    {
        Container ctnr;
        images.insert(Database::value_type(container, ctnr));
    }
}

void addImage(const string& container, const string& key, const string& file, Vector2i of)
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
        lockGLMutex(100);

        const auto& result = c->second.emplace(key, Image());
        result.first->second.set(file, of);

        if (Colorize)
            result.first->second.applyShader(gui::style::getColorizeShader(ColorizeRed, ColorizeGreen, ColorizeBlue));

        unlockGLMutex(100);

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

void setColorizeParameters(Vector3f r, Vector3f g, Vector3f b)
{
    ColorizeRed = std::move(r);
    ColorizeGreen = std::move(g);
    ColorizeBlue = std::move(b);
    Colorize = true;
}

void removeColorizeShader()
{
    Colorize = false;
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

imageManager::Image* getImage(const string& container, const string& key)
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

void changeHSL(const string& container, const string& key, double h, double s, double l)
{
    Image* i = getImage(container, key);

    if (i != nullptr)
    {
        HSL hsl(h, s, l);
        i->sprite.setColor(hsl.turnToRGB());
    }
}

void display(RenderTarget& target, const string& container, const string& key, float x, float y, bool atCenter, const Shader* shader)
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

void addAnimation(const string& name, const string& file)
{
    AnimationDatabase::iterator a = animations.find(name);

    if (a == animations.end())
    {
        imageManager::Animation ani;
        const auto& result = animations.insert(AnimationDatabase::value_type(name, ani));
        result.first->second.image.set(file, Vector2i(0, 0));
    }
}

imageManager::Animation* getAnimation(const string& name)
{
    AnimationDatabase::iterator a = animations.find(name);

    if (a == animations.end())
    {
        tools::debug::error("Animation not found: " + name, "images", __FILENAME__, __LINE__);
        return nullptr;
    }

    return &(a->second);
}

void lockGLMutex(int id)
{
    while (Mutex_lock.test_and_set(std::memory_order_acquire))
        std::this_thread::sleep_for(1us);

    Mutex_id.store(id, std::memory_order_release);
}

void unlockGLMutex(int id)
{
    if (Mutex_id.load(std::memory_order_acquire) != id)
        return;

    Mutex_lock.clear(std::memory_order_release);
}

} //namespace imageManager
