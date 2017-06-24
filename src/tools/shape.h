
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

#ifndef TOOLS_SHAPE_H
#define TOOLS_SHAPE_H

#include <vector>

#include "tools/vector2d.h"

using namespace std;

namespace tinyxml2{
    class XMLElement;
};

namespace sf{
    class RenderTarget;
    class Color;
};

namespace tools{

namespace math{

    int nv();
struct Shape
{
    enum Profiles
    {
        None,
        Point,
        Circle,
        Rectangle,
        Line
    };

    Profiles profile;
    const Vector2d* origin = nullptr;
    std::vector<Vector2d> points;

    pair<Vector2d, Vector2d> box;

    double radius1, radius2;
    double length1, length2;
    double angle1, angle2;

    void setOrigin(const Vector2d* o);
    void point(const Vector2d& p);
    void circle(const Vector2d& p, double radius);
    void rectangle(const Vector2d& p1, const Vector2d& p2, const Vector2d& p3);
    void line(const Vector2d& p1, const Vector2d& p2);
    void line(const Vector2d& p, double length, double angle);

    void loadFromXML(tinyxml2::XMLElement* elem);

    void display(sf::RenderTarget& target, const sf::Color& color);
};

} //namespace math

} //namespace tools

#endif // TOOLS_SHAPE_H

