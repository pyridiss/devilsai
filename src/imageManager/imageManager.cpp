
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

#include <sstream>

#include <physfs.h>

#include "Jeu/Jeu.h"
#include "Bibliotheque/Bibliotheque.h"
#include "Bibliotheque/Constantes.h"

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

void addContainer(string container)
{
    Database::iterator c = images.find(container);

    if (c == images.end())
    {
        Container ctnr;
        images.insert(Database::value_type(container, ctnr));
    }
}

void addImage(string container, string key, string file, Vector2i of, float scale)
{
    Database::iterator c = images.find(container);

    if (c == images.end())
    {
        Erreur("Contained has not been created yet:", container);
        return;
    }

    Container::iterator i = (*c).second.find(key);

    if (i == (*c).second.end())
    {
        imageManager::Image img;
        imageManager::Image* reference = nullptr;
        const auto& result = (*c).second.insert(Container::value_type(key, img));
        Container::iterator j = (*c).second.begin();
        while (j != (*c).second.end())
        {
            if (j->second.sourceFile == file) reference = &(j->second);
            if (PHYSFS_getSearchPath() != NULL && *(PHYSFS_getSearchPath()) != NULL)
            {
                string s = *(PHYSFS_getSearchPath());
                s += "/" + file;
                if (j->second.sourceFile == s) reference = &(j->second);
            }
            ++j;
        }
        result.first->second.set(file, reference, of, scale);
    }
}

void addArchiveFile(string path)
{
    if (PHYSFS_addToSearchPath(path.c_str(), 1) == 0)
    {
        Erreur("Unable to load file:", path);
    }
    else MESSAGE("File \"" + path + "\" open", FICHIER)
}

void removeArchiveFile(string path)
{
    PHYSFS_removeFromSearchPath(path.c_str());
}

imageManager::Image* getImage(string container, string key)
{
    Database::iterator c = images.find(container);

    if (c == images.end())
    {
        Erreur("Container not found:", container);
        return nullptr;
    }

    Container::iterator i = (*c).second.find(key);

    if (i == (*c).second.end())
    {
        Erreur("Image not found:", container + "::" + key);
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

void display(RenderWindow& app, string container, string key, float x, float y, bool atCenter)
{
    Database::iterator c = images.find(container);

    if (c == images.end())
    {
        Erreur("Image container has not been added yet:", container);
        return;
    }

    Container::iterator i = (*c).second.find(key);

    (*i).second.display(app, x, y, atCenter);
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
        Erreur("Animation not found:", name);
        return nullptr;
    }

    return &(a->second);
}



void Set_ImageDecoration(string Id, Color Couleur, IntRect Clip, float ScaleX, float ScaleY)
{
	ClasseDecorations::iterator i = ImgDecorations.find(ClefDecoration(Id));

	i->second.sprite.setColor(Couleur);
	i->second.sprite.setTextureRect(Clip);
	i->second.sprite.setScale(ScaleX, ScaleY);
}



} //namespace imageManager
