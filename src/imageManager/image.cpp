
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

#include <fstream>

#include <physfs.h>

#include "config.h"
#include "tools/debug.h"
#include "imageManager/image.h"

namespace imageManager{

void Image::set(string file, Image* reference, Vector2i of, float scale)
{
    offset = of;

    //Images can be loaded from a file or from an archive.
    ifstream f(file);
    bool fromFile = f.good();
    bool fromArch = PHYSFS_exists(file.c_str());
    bool fromRef  = (reference != nullptr);

    if (fromRef)
    {
        sprite.setTexture(reference->texture);
        sourceFile = reference->sourceFile;
    }
    else if (fromFile)
    {
        texture.loadFromFile(file);
        sprite.setTexture(texture);
        sourceFile = file;
    }
    else if (fromArch)
    {
        PHYSFS_file* physfsFile = PHYSFS_openRead(file.c_str());
        char *buffer = new char[PHYSFS_fileLength(physfsFile)];
        PHYSFS_read(physfsFile, buffer, 1, PHYSFS_fileLength(physfsFile));

        if (!texture.loadFromMemory(buffer, PHYSFS_fileLength(physfsFile)))
            fromArch = false;

        delete[] buffer;

        sprite.setTexture(texture);
        sourceFile = *(PHYSFS_getSearchPath()) + string("/") + file;

        PHYSFS_close(physfsFile);
    }

    if (!fromRef && !fromFile && !fromArch)
    {
        tools::debug::error("Unable to load file: " + file, "images");
    }

    texture.setSmooth(false);

    if (scale != 1) sprite.setScale(scale, scale);
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
