
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

#include <unordered_map>

#include <physfs.h>

#include "tools/debug.h"
#include "tools/filesystem.h"

#include "imageManager/image.h"
#include "imageManager/animation.h"
#include "imageManager/HSL.h"


namespace imageManager{

typedef unordered_map < string, imageManager::Image > Container;
typedef unordered_map < unsigned int, Container > Database;
typedef map < string, imageManager::Animation > AnimationDatabase;

Database images;
AnimationDatabase animations;
string currentArchiveFile;

std::atomic_flag Mutex_lock = ATOMIC_FLAG_INIT;
std::atomic<int> Mutex_id = 0;

void addContainer(unsigned int container)
{
    Database::iterator c = images.find(container);

    if (c == images.end())
    {
        Container ctnr;
        images.insert(Database::value_type(container, ctnr));
    }
}

void addImage(unsigned int container, const string& key, const string& file, Vector2i of, Shader* sh)
{
    Database::iterator c = images.find(container);

    if (c == images.end())
    {
        tools::debug::error("Image container has not been created yet for the file \"" + file + "\"", "multimedia", __FILENAME__, __LINE__);
        return;
    }

    Container::iterator i = (*c).second.find(key);

    if (i == (*c).second.end())
    {
        lockGLMutex(100);

        const auto& result = c->second.emplace(key, Image());
        result.first->second.set(file, of);

        if (sh != nullptr)
            result.first->second.applyShader(sh);

        unlockGLMutex(100);
    }
}

void addArchiveFile(string path)
{
    path = tools::filesystem::dataDirectory() + path;

    if (PHYSFS_addToSearchPath(path.c_str(), 1) == 0)
    {
        tools::debug::error("Unable to load file: " + path, "multimedia", __FILENAME__, __LINE__);
    }
    else
    {
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

imageManager::Image* getImage(unsigned int container, const string& key)
{
    Database::iterator c = images.find(container);

    if (c == images.end())
    {
        tools::debug::error("Container not found for the image key \"" + key + "\"", "multimedia", __FILENAME__, __LINE__);
        return nullptr;
    }

    Container::iterator i = (*c).second.find(key);

    if (i == (*c).second.end())
    {
        tools::debug::error("Image not found: \"" + key + "\"", "multimedia", __FILENAME__, __LINE__);
        return nullptr;
    }

    return &(i->second);
}

void changeHSL(unsigned int container, const string& key, double h, double s, double l)
{
    Image* i = getImage(container, key);

    if (i != nullptr)
    {
        HSL hsl(h, s, l);
        i->sprite.setColor(hsl.turnToRGB());
    }
}

void display(RenderTarget& target, unsigned int container, const string& key, float x, float y, bool atCenter, const Shader* shader)
{
    Database::iterator c = images.find(container);

    if (c == images.end())
    {
        tools::debug::error("Container not found for the image key \"" + key + "\"", "multimedia", __FILENAME__, __LINE__);
        return;
    }

    Container::iterator i = (*c).second.find(key);

    if (i == (*c).second.end())
    {
        tools::debug::error("Image not found: \"" + key + "\"", "multimedia", __FILENAME__, __LINE__);
        return;
    }

    (*i).second.display(target, x, y, atCenter, shader);
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
