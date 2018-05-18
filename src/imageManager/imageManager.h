
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

using std::string;

namespace multimedia{

// Assets

void addFont(const string& name, const string& file);
sf::Font* font(const string& name);
void addFragmentShader(const string& name, const string& file);
sf::Shader* shader(const string& name);

}  // namespace multimedia

namespace imageManager{

class Image;
class Animation;

void addContainer(unsigned int container);
void addImage(unsigned int container, const string& key, const string& file, sf::Vector2i of = sf::Vector2i(0, 0), sf::Shader* sh = nullptr);
void addArchiveFile(string path);
string getCurrentArchiveFile();
void removeArchiveFile(string path);
imageManager::Image* getImage(unsigned int container, const string& key);
void changeHSL(unsigned int container, const string& key, double h, double s, double l);
void display(sf::RenderTarget& target, unsigned int container, const string& key, float x, float y, bool atCenter = false, const sf::Shader* shader = nullptr);
void addAnimation(const string& name, const string& file);
imageManager::Animation* getAnimation(const string& name);
void lockGLMutex(int id);
void unlockGLMutex(int id);

} //namespace imageManager

#endif //IMAGEMANAGER_H
