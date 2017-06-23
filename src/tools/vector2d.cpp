
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

#include <cmath>
#include "tools/vector2d.h"

using namespace std;

namespace tools{

namespace math{

Vector2d::Vector2d()
{
}

Vector2d::Vector2d(const Vector2d& v)
{
    x = v.x;
    y = v.y;
}

Vector2d::Vector2d(double _x, double _y)
{
    x = _x;
    y = _y;
}

bool Vector2d::operator==(const Vector2d& right) const
{
    return (x == right.x && y == right.y);
}

Vector2d Vector2d::operator+(const Vector2d& right) const
{
    return Vector2d(x + right.x, y + right.y);
}

Vector2d Vector2d::operator-(const Vector2d& right) const
{
    return Vector2d(x - right.x, y - right.y);
}

double Vector2d::length()
{
    return sqrt(x * x + y * y);
}

double Vector2d::dotProduct(const Vector2d& v1, const Vector2d& v2)
{
    return (v1.x * v2.x + v1.y * v2.y);
}

double Vector2d::distanceSquare(const Vector2d& v1, const Vector2d& v2)
{
    return ((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
}

double Vector2d::distance(const Vector2d& v1, const Vector2d& v2)
{
    return sqrt(distanceSquare(v1, v2));
}

} //namespace math

} //namespace tools
