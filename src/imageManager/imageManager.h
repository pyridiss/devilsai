
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
void addImage(string container, string key, string file, Vector2i of = Vector2i(0, 0), float scale = 1);
imageManager::Image* getImage(string container, string key);
void display(RenderWindow& app, string container, string key, float x, float y, bool atCenter = false);
void addAnimation(string name, string file);
imageManager::Animation* getAnimation(string name);

void AjouterImagesIndividu(string, short, short, short, float[3][3]);
void AjouterImagesPaysageMouvant(string, short, short, int, int);
void AjouterImageProjectile(string, short);
void AjouterImageCompetence(string);
void Disp_ImageIndividu(string, short, short, short, float, float, bool = false);
void Disp_ImageCadavre(string, short, short, float, float, bool = false);
void Disp_ImagePaysageMouvant(string, short, short, float, float, bool = false);
void Disp_ImageProjectile(string, short, float, float, bool = false);
void Set_ImageDecoration(string, Color, IntRect, float = 1, float = 1);
void Disp_ImageCompetence(string, float, float);
void Supprimer_Images();

} //namespace imageManager

#endif //IMAGEMANAGER_H
