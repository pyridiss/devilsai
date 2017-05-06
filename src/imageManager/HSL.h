
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2017  Quentin Henriet

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

/*
 * This code is derived largely from https://github.com/SFML/SFML/wiki/Source:-HSL-Color
 */

#ifndef HSL_H
#define HSL_H

#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <cmath>

using namespace std;
using namespace sf;

namespace imageManager
{

struct HSL
{
    double hue;
    double saturation;
    double luminance;

    HSL();
    HSL(int h, int s, int l);

    Color turnToRGB();

    private:

    double hueToRGB(double arg1, double arg2, double h);

};

HSL turnToHSL(const Color& C);

} //namespace imageManager

#endif // HSL_H
