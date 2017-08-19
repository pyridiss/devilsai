
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
    class XMLHandle;
    class XMLDocument;
};

namespace sf{
    class RenderTarget;
    class Color;
};

namespace tools{

namespace math{

struct Shape
{
    enum Profiles
    {
        None,
        Point,
        Circle,
        Triangle,
        Rectangle,
        Line,
        Arc,
        Complex
    };

    Profiles profile = Profiles::None;
    const Vector2d* origin = nullptr;
    std::vector<Vector2d> points;

    pair<Vector2d, Vector2d> box;

    double radius1, radius2;
    double length1, length2;
    double angle1, angle2;

    Shape* next = nullptr;

    Shape() = default;

    Shape(const Shape& s)
        : profile(s.profile),
          origin(nullptr),
          points(s.points),
          box(s.box),
          radius1(s.radius1),
          radius2(s.radius2),
          length1(s.length1),
          length2(s.length2),
          angle1(s.angle1),
          angle2(s.angle2)
    {
        if (s.next != nullptr)
            next = new Shape(*(s.next));
    }

    Shape(Shape&& s)
    {
        swap(*this, s);
    }

    ~Shape()
    {
        if (next != nullptr)
            delete next;
    }

    Shape& operator=(Shape s)
    {
        swap(*this, s);
        return *this;
    }

    friend void swap(Shape& first, Shape& second)
    {
        using std::swap;

        swap(first.profile, second.profile);
        swap(first.origin, second.origin);
        swap(first.points, second.points);
        swap(first.box, second.box);
        swap(first.radius1, second.radius1);
        swap(first.radius2, second.radius2);
        swap(first.length1, second.length1);
        swap(first.length2, second.length2);
        swap(first.angle1, second.angle1);
        swap(first.angle2, second.angle2);
        swap(first.next, second.next);
    }

    void setOrigin(const Vector2d* o);
    void point(const Vector2d& p);
    void circle(const Vector2d& p, double radius);
    void triangle(const Vector2d& p1, const Vector2d& p2, const Vector2d& p3);
    void rectangle(const Vector2d& p1, const Vector2d& p2, const Vector2d& p3);
    void line(const Vector2d& p1, const Vector2d& p2);
    void line(const Vector2d& p, double length, double angle);
    void arc(const Vector2d& p, double radius, double direction, double opening);

    double area();
    void updateDirection(double direction);

    void loadFromXML(tinyxml2::XMLElement* elem);
    void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);

    void display(sf::RenderTarget& target, const sf::Color& color);
};

} //namespace math

} //namespace tools

#endif // TOOLS_SHAPE_H

