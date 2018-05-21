
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
#include <cmath>
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

void displayStretched(RenderTarget& target, unsigned int container, const string& key, double x, double y, double w, double h, bool atCenter, const Shader* shader)
{
    Image* i = getImage(container, key);

    if (i == nullptr) return;

    i->sprite.setScale(w / (double)i->texture.getSize().x, h / (double)i->texture.getSize().y);
    i->display(target, x, y, atCenter, shader);
    i->sprite.setScale(1, 1);
}

void displayRepeatedly(RenderTarget& target, unsigned int container, const string& key, double x, double y, int xQuantity, int yQuantity, bool atCenter, const Shader* shader)
{
    Image* i = getImage(container, key);

    if (i == nullptr) return;

    Vector2u is = i->texture.getSize();
    Vector2i of = i->offset;

    VertexArray area(Quads, 4 * xQuantity * yQuantity);

    if (atCenter)
    {
        x -= is.x * xQuantity / 2.0;
        y -= is.y * yQuantity / 2.0;
    }

    for (int xTmp = 0 ; xTmp < xQuantity ; ++xTmp)
    {
        for (int yTmp = 0 ; yTmp < yQuantity ; ++yTmp)
        {
            int o = 4*(xTmp * yQuantity + yTmp);

            area[o + 0].position = Vector2f(x + xTmp * is.x + of.x, y + yTmp * is.y + of.y);
            area[o + 1].position = Vector2f(x + (xTmp+1) * is.x + of.x, y + yTmp * is.y + of.y);
            area[o + 2].position = Vector2f(x + (xTmp+1) * is.x + of.x, y + (yTmp+1) * is.y + of.y);
            area[o + 3].position = Vector2f(x + xTmp * is.x + of.x, y + (yTmp+1) * is.y + of.y);

            area[o + 0].texCoords = Vector2f(0, 0);
            area[o + 1].texCoords = Vector2f(is.x, 0);
            area[o + 2].texCoords = Vector2f(is.x, is.y);
            area[o + 3].texCoords = Vector2f(0, is.y);
        }
    }

    RenderStates states;
    states.texture = &(i->texture);
    states.shader = shader;

    target.draw(area, states);
}

void fillArea(RenderTarget& target, unsigned int container, const string& key, double x, double y, double w, double h, int xOrigin, int yOrigin, const Shader* shader)
{
    Image* i = getImage(container, key);

    if (i == nullptr) return;

    Vector2u is = i->texture.getSize();
    while (xOrigin > x) xOrigin -= is.x;
    while (yOrigin > y) yOrigin -= is.y;
    double xOffset = (int)(x - xOrigin) % is.x;
    double yOffset = (int)(y - yOrigin) % is.y;
    int xNumber = ceil(w / (double)is.x) + (((int)w%is.x == 0) ? 1 : 0) + (((is.x - xOffset) <= (int)w % is.x) ? 1 : 0);
    int yNumber = ceil(h / (double)is.y) + (((int)h%is.y == 0) ? 1 : 0) + (((is.y - yOffset) <= (int)h % is.y) ? 1 : 0);

    VertexArray area(Quads, 4 * xNumber * yNumber);

    for (int xTmp = 0 ; xTmp < xNumber ; ++xTmp)
    {
        for (int yTmp = 0 ; yTmp < yNumber ; ++yTmp)
        {
            int o = 4*(xTmp * yNumber + yTmp);

            area[o + 0].position = Vector2f(max(x, x - xOffset + is.x * xTmp), max(y, y - yOffset + is.y * yTmp));
            area[o + 1].position = Vector2f(min((double)is.x * (int)((x-xOrigin) / is.x + xTmp+1) + xOrigin, x + w), area[o].position.y);
            area[o + 2].position = Vector2f(area[o+1].position.x, min((double)is.y * (int)((y-yOrigin) / is.y + yTmp+1) + yOrigin, y + h));
            area[o + 3].position = Vector2f(area[o].position.x, area[o+2].position.y);

            area[o + 0].texCoords = Vector2f((xTmp == 0) ? xOffset : 0 , (yTmp == 0) ? yOffset : 0);
            area[o + 1].texCoords = Vector2f((xTmp == xNumber-1) ? (int)(w + xOffset) % is.x : is.x, area[o].texCoords.y);
            area[o + 2].texCoords = Vector2f(area[o+1].texCoords.x, (yTmp == yNumber-1) ? (int)(h + yOffset) % is.y : is.y);
            area[o + 3].texCoords = Vector2f(area[o].texCoords.x, area[o+2].texCoords.y);
        }
    }

    RenderStates states;
    states.texture = &(i->texture);
    states.shader = shader;

    target.draw(area, states);
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
