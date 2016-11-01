
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

#include "Bibliotheque/Bibliotheque.h"

#include "image.h"
#include "config.h"

namespace imageManager{

void Image::set(string file, Vector2i of, float scale)
{
    offset = of;

    if (!texture.loadFromFile(file))
        Erreur("Unable to load file:", file.c_str());

    sprite.setTexture(texture);
    texture.setSmooth(false);

    if (scale != 1) sprite.setScale(scale, scale);
}

void Image::setFromArchive(string file, Vector2i of)
{
    offset = of;

    if (PHYSFS_exists(file.c_str()) == 0)
    {
        const char* error = PHYSFS_getLastError();
        if (error != NULL) Erreur("PhysicsFS error:", error);
    }
    else
    {
        PHYSFS_file* physfsFile = PHYSFS_openRead(file.c_str());
        char *buffer = new char[PHYSFS_fileLength(physfsFile)];
        PHYSFS_read(physfsFile, buffer, 1, PHYSFS_fileLength(physfsFile));

        if (!texture.loadFromMemory(buffer, PHYSFS_fileLength(physfsFile)))
        {
            Erreur("Unable to load file:", file.c_str());
        }
        sprite.setTexture(texture);
        texture.setSmooth(false);

        delete[] buffer;
        PHYSFS_close(physfsFile);
    }
}

void Image::applyShader(string file)
{
    RenderTexture tex;
    tex.create(texture.getSize().x, texture.getSize().y);

    Shader shader;
    shader.loadFromFile(INSTALL_DIR + file, Shader::Type::Fragment);

    tex.draw(sprite, &shader);

    texture = tex.getTexture();
}

const Vector2u Image::getSize()
{
    return texture.getSize();
}

void Image::display(RenderWindow& app, float x, float y, bool atCenter)
{
    if (!atCenter) sprite.setPosition(x + offset.x, y + offset.y);
    else sprite.setPosition((int)(x - sprite.getLocalBounds().width/2) + offset.x, (int)(y - sprite.getLocalBounds().height/2) + offset.y);
    app.draw(sprite);
}

} //namespace imageManager
