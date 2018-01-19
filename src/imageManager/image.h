
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2016  Quentin Henriet

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

#ifndef IMAGE_H
#define IMAGE_H

#include <string>

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

namespace imageManager
{

class Image
{
    public:
        Texture texture;
        Sprite sprite;
        string sourceFile;
        Vector2i offset;

    public:
        void set(string file, Image* reference, Vector2i of);
        void applyShader(string file);
        const Vector2u getSize();
        void display(RenderTarget& target, float x, float y, bool atCenter = false, const Shader* shader = nullptr);
};

} //namespace imageManager

#endif // IMAGE_H
