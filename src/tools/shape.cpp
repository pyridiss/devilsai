
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
#include <sstream>

#include <tinyxml2.h>
#include <SFML/Graphics.hpp>

#include "tools/debug.h"
#include "tools/math.h"
#include "tools/vector2d.h"
#include "tools/shape.h"

using namespace std;
using namespace tinyxml2;
using namespace sf;

namespace tools{

namespace math{

Vector2d _absoluteOrigin(0, 0);

Vector2d* absoluteOrigin()
{
    return &_absoluteOrigin;
}

void Shape::setOrigin(const Vector2d* o)
{
    origin = o;
    if (next != nullptr) next->setOrigin(o);
}

void Shape::point(const Vector2d& p)
{
    profile = Profiles::Point;

    points.clear();
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

    points.clear();
    points.emplace_back(p.x, p.y);
    radius1 = radius;

    box.first.x = p.x - radius;
    box.first.y = p.y - radius;
    box.second.x = p.x + radius;
    box.second.y = p.y + radius;
}

void Shape::triangle(const Vector2d& p1, const Vector2d& p2, const Vector2d& p3)
{
    if (p1 == p2 || p1 == p3 || p2 == p3)
    {
        profile = Profiles::None;
        return;
    }

    profile = Profiles::Triangle;

    points.clear();
    points.emplace_back(p1.x, p1.y);
    points.emplace_back(p2.x, p2.y);
    points.emplace_back(p3.x, p3.y);

    box.first.x = min({p1.x, p2.x, p3.x});
    box.first.y = min({p1.y, p2.y, p3.y});
    box.second.x = max({p1.x, p2.x, p3.x});
    box.second.y = max({p1.y, p2.y, p3.y});
}

void Shape::rectangle(const Vector2d& p1, const Vector2d& p2, const Vector2d& p3)
{
    if (p1 == p2 || p1 == p3 || p2 == p3)
    {
        profile = Profiles::None;
        return;
    }

    profile = Profiles::Rectangle;

    points.clear();
    points.emplace_back(p1.x, p1.y);
    points.emplace_back(p2.x, p2.y);
    points.emplace_back(p3.x, p3.y);
    points.emplace_back(p2 + p3 - p1);

    box.first.x = min({p1.x, p2.x, p3.x, points[3].x});
    box.first.y = min({p1.y, p2.y, p3.y, points[3].y});
    box.second.x = max({p1.x, p2.x, p3.x, points[3].x});
    box.second.y = max({p1.y, p2.y, p3.y, points[3].y});
}

void Shape::line(const Vector2d& p1, const Vector2d& p2, int thickness)
{
    profile = Profiles::Line;

    points.clear();
    points.emplace_back(p1.x, p1.y);
    points.emplace_back(p2.x, p2.y);

    length1 = sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y));
    angle1 = angle(p2.x - p1.x, p2.y - p1.y);
    radius1 = thickness;

    box.first.x = min(p1.x, p2.x);
    box.first.y = min(p1.y, p2.y);
    box.second.x = max(p1.x, p2.x);
    box.second.y = max(p1.y, p2.y);
}

void Shape::line(const Vector2d& p, double length, double angle, int thickness)
{
    profile = Profiles::Line;

    points.clear();
    points.emplace_back(p.x, p.y);
    points.emplace_back(p.x + length * cos(angle), p.y + length * sin(angle));
    length1 = length;
    angle1 = angle;
    radius1 = thickness;

    box.first.x = min(points[0].x, points[1].x);
    box.first.y = min(points[0].y, points[1].y);
    box.second.x = max(points[0].x, points[1].x);
    box.second.y = max(points[0].y, points[1].y);
}

void Shape::polyline(vector<Vector2d>& p, int thickness)
{
    if (p.size() < 2)
    {
        tools::debug::error("A polyline with less than 2 points has been asked.", "tools::math", __FILENAME__, __LINE__);
        return;
    }

    profile = Profiles::Line;

    points = p;
    radius1 = thickness;

    box.first.x = points[0].x;
    box.first.y = points[0].y;
    box.second.x = points[0].x;
    box.second.y = points[0].y;

    for (auto& pt : points)
    {
        if (pt.x - radius1 < box.first.x) box.first.x = pt.x - radius1;
        if (pt.y - radius1 < box.first.y) box.first.y = pt.y - radius1;
        if (pt.x + radius1 > box.second.x) box.second.x = pt.x + radius1;
        if (pt.y + radius1 > box.second.y) box.second.y = pt.y + radius1;
    }
}

void Shape::arc(const Vector2d& p, double radius, double direction, double opening)
{
    profile = Profiles::Arc;

    points.clear();
    points.emplace_back(p.x, p.y);
    radius1 = radius;
    angle1 = direction;
    angle2 = opening;

    box.first.x = p.x - radius;
    box.first.y = p.y - radius;
    box.second.x = p.x + radius;
    box.second.y = p.y + radius;
}

double Shape::length()
{
    //This function just computes the sum of the lengths of each edge.

    double l = 0;

    for (unsigned i = 1 ; i < points.size() ; ++i)
    {
        l += Vector2d::distance(points[i-1], points[i]);
    }

    return l;
}

double Shape::area()
{
    switch(profile)
    {
        case Profiles::None:
            return 0;
        case Profiles::Point:
            return 0;
        case Profiles::Circle:
            return M_PI * radius1 * radius1;
        case Profiles::Triangle:
            return 1./2. * abs((points[1].x - points[0].x)*(points[2].y - points[0].y) - (points[2].x - points[0].x)*(points[1].y - points[0].y));
        case Profiles::Rectangle:
            return Vector2d::distance(points[0], points[1]) * Vector2d::distance(points[0], points[2]);
        case Profiles::Line:
            return 0;
        case Profiles::Arc:
            return angle2 * radius1 * radius1;
        case Profiles::Complex:
        {
            //Simple addition of areas... does not take into account overlap
            double value = 0;
            for (Shape* s = next ; s != nullptr ; s = s->next)
            {
                value += s->area();
            }
            return value;
        }
        default:
            return 0;
    }

    return 0;
}

void Shape::updateDirection(double direction)
{
    if (profile == Profiles::Line)
    {
        angle1 = direction;
        points.pop_back();
        points.emplace_back(points[0].x + length1 * cos(angle1), points[0].y + length1 * sin(angle1));
        box.first.x = min(points[0].x, points[1].x);
        box.first.y = min(points[0].y, points[1].y);
        box.second.x = max(points[0].x, points[1].x);
        box.second.y = max(points[0].y, points[1].y);
    }
    else if (profile == Profiles::Arc)
    {
        angle1 = direction;
    }
}

void Shape::loadFromXML(XMLElement* elem)
{
    points.clear();

    string type = elem->Attribute("type");

    if (type == "none")
    {
        profile = Profiles::None;
    }

    else if (type == "point")
    {
        tools::debug::warning("Shape::loadFromXML() received type == 'point', which is not implemented", "tools::math", __FILENAME__, __LINE__);
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

    else if (type == "triangle")
    {
        Vector2d p1(0, 0);
        Vector2d p2(0, 0);
        Vector2d p3(0, 0);

        elem->QueryAttribute("x1", &p1.x);
        elem->QueryAttribute("y1", &p1.y);
        elem->QueryAttribute("x2", &p2.x);
        elem->QueryAttribute("y2", &p2.y);
        elem->QueryAttribute("x3", &p3.x);
        elem->QueryAttribute("y3", &p3.y);

        triangle(p1, p2, p3);
    }

    else if (type == "rectangle")
    {
        Vector2d p1(0, 0);
        Vector2d p2(0, 0);
        Vector2d p3(0, 0);
        double xCenter = 0, yCenter = 0, xSize = 0, ySize = 0, angle = 0;

        elem->QueryAttribute("xCenter", &xCenter);
        elem->QueryAttribute("yCenter", &yCenter);
        elem->QueryAttribute("xSize", &xSize);
        elem->QueryAttribute("ySize", &ySize);
        elem->QueryAttribute("angle", &angle);

        if (xSize == 0 || ySize == 0)
        {
            double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
            elem->QueryAttribute("x1", &x1);
            elem->QueryAttribute("y1", &y1);
            elem->QueryAttribute("x2", &x2);
            elem->QueryAttribute("y2", &y2);
            xCenter = (x1 + x2) / 2.0;
            yCenter = (y1 + y2) / 2.0;
            xSize = x2 - x1;
            ySize = y2 - y1;
        }

        p1.x = xCenter + ((-xSize/2.0) * cos(angle) - (-ySize/2.0) * sin(angle));
        p1.y = yCenter + ((-xSize/2.0) * sin(angle) + (-ySize/2.0) * cos(angle));
        p2.x = xCenter + (xSize/2.0 * cos(angle) - (-ySize/2.0) * sin(angle));
        p2.y = yCenter + (xSize/2.0 * sin(angle) + (-ySize/2.0) * cos(angle));
        p3.x = xCenter + ((-xSize/2.0) * cos(angle) - ySize/2.0 * sin(angle));
        p3.y = yCenter + ((-xSize/2.0) * sin(angle) + ySize/2.0 * cos(angle));

        rectangle(p1, p2, p3);
    }

    else if (type == "line")
    {
        Vector2d p(0, 0);
        double length = 0, angle = 0, thickness = 0;
        elem->QueryAttribute("xOrigin", &p.x);
        elem->QueryAttribute("yOrigin", &p.y);
        elem->QueryAttribute("length", &length);
        elem->QueryAttribute("angle", &angle);
        elem->QueryAttribute("thickness", &thickness);

        line(p, length, angle, thickness);
    }

    else if (type == "polyline")
    {
        vector<Vector2d> p;
        int n = 0, thickness = 0;
        elem->QueryAttribute("points", &n);
        elem->QueryAttribute("thickness", &thickness);
        for (int i = 1 ; i <= n ; ++i)
        {
            Vector2d v(0, 0);

            stringstream x, y;
            x << "x" << i;
            y << "y" << i;

            elem->QueryAttribute(x.str().c_str(), &v.x);
            elem->QueryAttribute(y.str().c_str(), &v.y);
            p.emplace_back(std::move(v));
        }

        polyline(p, thickness);
    }

    else if (type == "arc")
    {
        Vector2d p(0, 0);
        double radius = 0, direction = 0, opening = 0;
        elem->QueryAttribute("xCenter", &p.x);
        elem->QueryAttribute("yCenter", &p.y);
        elem->QueryAttribute("radius", &radius);
        elem->QueryAttribute("direction", &direction);
        elem->QueryAttribute("opening", &opening);

        arc(p, radius, direction, opening);
    }

    else if (type == "complex")
    {
        profile = Profiles::Complex;

        Shape *current = this;
        XMLHandle hdl(elem);
        XMLElement *elem2 = hdl.FirstChildElement().ToElement();
        while (elem2)
        {
            current->next = new Shape;
            current->next->loadFromXML(elem2);
            current->next->setOrigin(origin);
            current = current->next;

            elem2 = elem2->NextSiblingElement();
        }

        current = this->next;
        box = current->box;
        while (current != nullptr)
        {
            box.first.x = min(box.first.x, current->box.first.x);
            box.first.y = min(box.first.y, current->box.first.y);
            box.second.x = max(box.second.x, current->box.second.x);
            box.second.y = max(box.second.y, current->box.second.y);
            current = current->next;
        }
    }
}

void Shape::saveToXML(XMLDocument& doc, XMLHandle& handle)
{
    XMLElement* root = handle.ToElement();

    switch(profile)
    {
        case Profiles::None:
            root->SetAttribute("type", "none");
            break;
        case Profiles::Point:
            root->SetAttribute("type", "point");
            break;
        case Profiles::Circle:
            root->SetAttribute("type", "circle");
            root->SetAttribute("xCenter", points[0].x);
            root->SetAttribute("yCenter", points[0].y);
            root->SetAttribute("radius", radius1);
            break;
        case Profiles::Triangle:
            root->SetAttribute("type", "circle");
            root->SetAttribute("x1", points[0].x);
            root->SetAttribute("y1", points[0].y);
            root->SetAttribute("x2", points[1].x);
            root->SetAttribute("y2", points[1].y);
            root->SetAttribute("x3", points[2].x);
            root->SetAttribute("y3", points[2].x);
            break;
        case Profiles::Rectangle:
            root->SetAttribute("type", "rectangle");
            root->SetAttribute("xCenter", (points[1].x + points[2].x) / 2.0);
            root->SetAttribute("yCenter", (points[1].y + points[2].y) / 2.0);
            root->SetAttribute("xSize", Vector2d::distance(points[0], points[1]));
            root->SetAttribute("ySize", Vector2d::distance(points[0], points[2]));
            root->SetAttribute("angle", atan((points[1].y - points[0].y) / (points[1].x - points[0].x)));
            break;
        case Profiles::Line:
            root->SetAttribute("type", "line");
            root->SetAttribute("thickness", radius1);
            root->SetAttribute("points", (int)points.size());
            for (unsigned i = 0 ; i < points.size() ; ++i)
            {
                stringstream x, y;
                x << "x" << i + 1;
                y << "y" << i + 1;
                root->SetAttribute(x.str().c_str(), points[i].x);
                root->SetAttribute(y.str().c_str(), points[i].y);
            }
            break;
        case Profiles::Arc:
            root->SetAttribute("type", "arc");
            root->SetAttribute("xCenter", points[0].x);
            root->SetAttribute("yCenter", points[0].y);
            root->SetAttribute("radius", radius1);
            root->SetAttribute("direction", angle1);
            root->SetAttribute("opening", angle2);
            break;
        case Profiles::Complex:
            root->SetAttribute("type", "complex");
            for (Shape* s = next ; s != nullptr ; s = s->next)
            {
                XMLElement* elem = doc.NewElement("shape");
                XMLHandle hdl(elem);
                s->saveToXML(doc, hdl);
                root->InsertEndChild(elem);
            }
            break;
        default:
            break;
    }
}

void Shape::display(RenderTarget& target, const Color& color)
{
    if (origin == nullptr)
    {
        tools::debug::error("Shape::display called while the shape has no origin.", "tools::math", __FILENAME__, __LINE__);
        return;
    }

    switch (profile)
    {
        case Profiles::None:
            break;
        case Profiles::Point:
            break;
        case Profiles::Circle:
            {
                CircleShape drawing(radius1);
                drawing.setPosition(origin->x + points[0].x - radius1, origin->y + points[0].y - radius1);
                drawing.setFillColor(color);
                target.draw(drawing);
            }
            break;
        case Profiles::Triangle:
            {
                ConvexShape drawing;
                drawing.setPointCount(3);
                drawing.setPoint(0, Vector2f(origin->x + points[0].x, origin->y + points[0].y));
                drawing.setPoint(1, Vector2f(origin->x + points[1].x, origin->y + points[1].y));
                drawing.setPoint(2, Vector2f(origin->x + points[2].x, origin->y + points[2].y));
                drawing.setFillColor(color);
                target.draw(drawing);
            }
            break;
        case Profiles::Rectangle:
            {
                ConvexShape drawing;
                drawing.setPointCount(4);
                drawing.setPoint(0, Vector2f(origin->x + points[0].x, origin->y + points[0].y));
                drawing.setPoint(1, Vector2f(origin->x + points[1].x, origin->y + points[1].y));
                drawing.setPoint(2, Vector2f(origin->x + points[3].x, origin->y + points[3].y));
                drawing.setPoint(3, Vector2f(origin->x + points[2].x, origin->y + points[2].y));
                drawing.setFillColor(color);
                target.draw(drawing);
            }
            break;
        case Profiles::Line:
            {
                CircleShape drawing0(radius1);
                drawing0.setPosition(origin->x + points[0].x - radius1, origin->y + points[0].y - radius1);
                drawing0.setFillColor(color);
                target.draw(drawing0);
                for (unsigned i = 1 ; i < points.size() ; ++i)
                {
                    ConvexShape drawing;
                    drawing.setPointCount(4);
                    drawing.setPoint(0, Vector2f(origin->x + points[i-1].x, origin->y + points[i-1].y));
                    drawing.setPoint(1, Vector2f(origin->x + points[i].x, origin->y + points[i].y));
                    drawing.setPoint(2, Vector2f(origin->x + points[i].x, origin->y + points[i].y));
                    drawing.setPoint(3, Vector2f(origin->x + points[i-1].x, origin->y + points[i-1].y));
                    drawing.setOutlineColor(color);
                    drawing.setOutlineThickness(radius1);
                    target.draw(drawing);
                    CircleShape drawing2(radius1);
                    drawing2.setPosition(origin->x + points[i].x - radius1, origin->y + points[i].y - radius1);
                    drawing2.setFillColor(color);
                    target.draw(drawing2);
                }
            }
            break;
        case Profiles::Arc:
            {
                ConvexShape drawing;
                drawing.setPointCount(6);
                drawing.setPoint(0, Vector2f(origin->x + points[0].x, origin->y + points[0].y));
                drawing.setPoint(1, Vector2f(origin->x + points[0].x + radius1 * cos(angle1 - angle2), origin->y + points[0].y + radius1 * sin(angle1 - angle2)));
                drawing.setPoint(2, Vector2f(origin->x + points[0].x + radius1 * cos(angle1 - 0.5 * angle2), origin->y + points[0].y + radius1 * sin(angle1 - 0.5 * angle2)));
                drawing.setPoint(3, Vector2f(origin->x + points[0].x + radius1 * cos(angle1), origin->y + points[0].y + radius1 * sin(angle1)));
                drawing.setPoint(4, Vector2f(origin->x + points[0].x + radius1 * cos(angle1 + 0.5 * angle2), origin->y + points[0].y + radius1 * sin(angle1 + 0.5 * angle2)));
                drawing.setPoint(5, Vector2f(origin->x + points[0].x + radius1 * cos(angle1 + angle2), origin->y + points[0].y + radius1 * sin(angle1 + angle2)));
                drawing.setFillColor(color);
                target.draw(drawing);
            }
            break;
        case Profiles::Complex:
        {
            Shape* current = next;
            while (current != nullptr)
            {
                current->display(target, color);
                current = current->next;
            }
            break;
        }
        default:
            break;
    }
}

} //namespace math

} //namespace tools
