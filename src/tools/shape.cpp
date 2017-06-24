
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

#include <tinyxml2.h>

#include "tools/debug.h"
#include "tools/math.h"
#include "tools/vector2d.h"
#include "tools/shape.h"

using namespace std;
using namespace tinyxml2;

namespace tools{

namespace math{

void Shape::setOrigin(const Vector2d* o)
{
    origin = o;
}

void Shape::point(const Vector2d& p)
{
    profile = Profiles::Point;

    points.emplace_back(p.x, p.y);

    box.first.x = p.x - 1;
    box.first.y = p.y - 1;
    box.second.x = p.x + 1;
    box.second.y = p.y + 1;
}

void Shape::circle(const Vector2d& p, double radius)
{
    if (radius == 0)
    {
        profile = Profiles::None;
        return;
    }

    profile = Profiles::Circle;

    points.emplace_back(p.x, p.y);
    radius1 = radius;

    box.first.x = p.x - radius;
    box.first.y = p.y - radius;
    box.second.x = p.x + radius;
    box.second.y = p.y + radius;
}

void Shape::rectangle(const Vector2d& p1, const Vector2d& p2, const Vector2d& p3)
{
    if (p1 == p2 || p1 == p3 || p2 == p3)
    {
        profile = Profiles::None;
        return;
    }

    profile = Profiles::Rectangle;

    points.emplace_back(p1.x, p1.y);
    points.emplace_back(p2.x, p2.y);
    points.emplace_back(p3.x, p3.y);
    points.emplace_back((p2 - p1) + (p3 - p1));

    box.first.x = min(p1.x, min(p2.x, p3.x));
    box.first.y = min(p1.y, min(p2.y, p3.y));
    box.second.x = max(p1.x, max(p2.x, p3.x));
    box.second.y = max(p1.y, max(p2.y, p3.y));
}

void Shape::line(const Vector2d& p1, const Vector2d& p2)
{
    profile = Profiles::Line;

    points.emplace_back(p1.x, p1.y);
    points.emplace_back(p2.x, p2.y);

    length1 = sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y));
    angle1 = angle(p2.x - p1.x, p2.y - p1.y);

    box.first.x = min(p1.x, p2.x);
    box.first.y = min(p1.y, p2.y);
    box.second.x = max(p1.x, p2.x);
    box.second.y = max(p1.y, p2.y);
}

void Shape::line(const Vector2d& p, double length, double angle)
{
    profile = Profiles::Line;

    points.emplace_back(p.x, p.y);
    points.emplace_back(p.x + length * cos(angle), p.y + length * sin(angle));
    length1 = length;
    angle1 = angle;

    box.first.x = min(points[0].x, points[1].x);
    box.first.y = min(points[0].y, points[1].y);
    box.second.x = max(points[0].x, points[1].x);
    box.second.y = max(points[0].y, points[1].y);
}

void Shape::loadFromXML(XMLElement* elem)
{
    string type = elem->Attribute("type");

    if (type == "none")
    {
        tools::debug::warning("Shape::loadFromXML() received type == 'none', which is not implemented", "tools::math");
    }

    else if (type == "point")
    {
        tools::debug::warning("Shape::loadFromXML() received type == 'point', which is not implemented", "tools::math");
    }

    else if (type == "circle")
    {
        Vector2d p(0, 0);
        double radius = 0;
        elem->QueryAttribute("xCenter", &p.x);
        elem->QueryAttribute("yCenter", &p.y);
        elem->QueryAttribute("radius", &radius);

        circle(p, radius);
    }

    else if (type == "rectangle")
    {
        Vector2d p1(0, 0);
        Vector2d p2(0, 0);
        Vector2d p3(0, 0);

        if (elem->Attribute("points"))
        {
            tools::debug::warning("Shape::loadFromXML() received rectangle defined with points, which is not implemented", "tools::math");
        }

        if (elem->Attribute("xSize"))
        {
            double xCenter = 0, yCenter = 0, xSize = 0, ySize = 0;
            elem->QueryAttribute("xCenter", &xCenter);
            elem->QueryAttribute("yCenter", &yCenter);
            elem->QueryAttribute("xSize", &xSize);
            elem->QueryAttribute("ySize", &ySize);
            p1.x = xCenter - xSize/2.0;
            p1.y = yCenter - ySize/2.0;
            p2.x = xCenter + xSize/2.0;
            p2.y = yCenter - ySize/2.0;
            p3.x = xCenter - xSize/2.0;
            p3.y = yCenter + ySize/2.0;
        }

        rectangle(p1, p2, p3);
    }

    else if (type == "line")
    {
        Vector2d p(0, 0);
        double length = 0, angle = 0;
        elem->QueryAttribute("xOrigin", &p.x);
        elem->QueryAttribute("yOrigin", &p.y);
        elem->QueryAttribute("length", &length);
        elem->QueryAttribute("angle", &angle);

        line(p, length, angle);
    }
}

} //namespace math

} //namespace tools
