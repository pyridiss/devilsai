
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

#ifndef TOOLS_VECTOR2D_H
#define TOOLS_VECTOR2D_H

using namespace std;

namespace tools{

namespace math{

struct Vector2d
{
    double x = 0;
    double y = 0;

    Vector2d();
    Vector2d(const Vector2d& v);
    Vector2d(double _x, double _y);

    bool operator==(const Vector2d& right) const;
    Vector2d operator+(const Vector2d& right) const;
    Vector2d operator-(const Vector2d& right) const;

    double length();
    static double dotProduct(const Vector2d& v1, const Vector2d& v2);
    static double distanceSquare(const Vector2d& v1, const Vector2d& v2);
    static double distance(const Vector2d& v1, const Vector2d& v2);
};

} //namespace math

} //namespace tools

#endif // TOOLS_VECTOR2D_H

