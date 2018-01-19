
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

#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <string>

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

namespace imageManager{

class Image;
class Animation;

void addContainer(string container);
void addImage(string container, string key, string file, Vector2i of = Vector2i(0, 0));
void addArchiveFile(string path);
string getCurrentArchiveFile();
void removeArchiveFile(string path);
imageManager::Image* getImage(string container, string key);
void changeHSL(string container, string key, double h, double s, double l);
void display(RenderTarget& target, string container, string key, float x, float y, bool atCenter = false, const Shader* shader = nullptr);
void addAnimation(string name, string file);
imageManager::Animation* getAnimation(string name);

} //namespace imageManager

#endif //IMAGEMANAGER_H
