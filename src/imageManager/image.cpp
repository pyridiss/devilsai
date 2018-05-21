
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
#include "imageManager/image.h"
#include "imageManager/imageManager.h"

namespace imageManager{

Image::Image()
  : texture(),
    sprite(),
    sourceFile(),
    offset()
{
}

void Image::set(string file, Vector2i of)
{
    offset = of;

    //Images can be loaded from a file or from an archive.
    bool fromFile = tools::filesystem::checkFile(tools::filesystem::dataDirectory() + file);
    bool fromArch = PHYSFS_exists(file.c_str());

    if (fromFile)
    {
        texture.loadFromFile(tools::filesystem::dataDirectory() + file);
        sprite.setTexture(texture);
        sourceFile = file;
    }
    else if (fromArch)
    {
        PHYSFS_file* physfsFile = PHYSFS_openRead(file.c_str());
        char *buffer = new char[PHYSFS_fileLength(physfsFile)];
        PHYSFS_readBytes(physfsFile, buffer, PHYSFS_fileLength(physfsFile));

        if (!texture.loadFromMemory(buffer, PHYSFS_fileLength(physfsFile)))
            fromArch = false;

        delete[] buffer;

        sprite.setTexture(texture);
        sourceFile = getCurrentArchiveFile() + "/" + file;

        PHYSFS_close(physfsFile);
    }

    if (!fromFile && !fromArch)
    {
        tools::debug::error("Unable to load file: " + file, "images", __FILENAME__, __LINE__);
    }

    texture.setSmooth(false);
}

void Image::applyShader(const Shader* shader)
{
    RenderTexture tex;
    tex.create(texture.getSize().x, texture.getSize().y);
    tex.clear(Color(0, 0, 0, 0));

    tex.draw(sprite, shader);
    tex.display();

    texture = tex.getTexture();
}

void Image::display(RenderTarget& target, float x, float y, bool atCenter, const Shader* shader)
{
    if (!atCenter) sprite.setPosition(x + offset.x, y + offset.y);
    else sprite.setPosition((int)(x - sprite.getLocalBounds().width/2) + offset.x, (int)(y - sprite.getLocalBounds().height/2) + offset.y);

    target.draw(sprite, shader);
}

} //namespace imageManager
