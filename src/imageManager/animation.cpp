
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

#include <cmath>

#include "imageManager/animation.h"

using namespace std;

float I(float);

namespace imageManager{

void Animation::setRectangle(int x, int y, int w, int h)
{
    IntRect newRect(x, y, w,h );
    image.sprite.setTextureRect(newRect);
}

void Animation::setSmoothRectangle(int x, int y, int w, int h)
{
    IntRect newRect;
    IntRect currentRect = image.sprite.getTextureRect();

    newRect.left   = currentRect.left   - (currentRect.left   - x) / (abs(currentRect.left   - x)/3 + 1) ;
    newRect.top    = currentRect.top    - (currentRect.top    - y) / (abs(currentRect.top    - y)/3 + 1) ;
    newRect.width  = currentRect.width  - (currentRect.width  - w) / (abs(currentRect.width  - w)/3 + 1) ;
    newRect.height = currentRect.height - (currentRect.height - h) / (abs(currentRect.height - h)/3 + 1) ;

    image.sprite.setTextureRect(newRect);
}

void Animation::setFlickering(float f)
{
    flickering = f;
}

void Animation::setColor(Color c)
{
    image.sprite.setColor(c);
}

void Animation::display(RenderWindow& app, int x, int y, bool atCenter)
{
    static double time;
    time += I(1);
    if (flickering)
        image.sprite.setColor(Color(255, 255, 255, 255*sin(time*flickering)));

    image.display(app, x, y, atCenter);
}

} //namespace imageManager
