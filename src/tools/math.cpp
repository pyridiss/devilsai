
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
#include <random>
#include <chrono>

#include "tools/debug.h"
#include "tools/math.h"
#include "tools/shape.h"

namespace tools{

namespace math{

default_random_engine randomGenerator;

void initLibrary()
{
    chrono::high_resolution_clock::time_point t = chrono::high_resolution_clock::now();
    chrono::high_resolution_clock::duration   d = t.time_since_epoch();
    randomGenerator.seed(d.count());
}

int newUniqueIdentifier()
{
    static int id = -1;
	return ++id;
}

double angle(double x, double y)
{
    if (x > 0 && y >= 0)
        return atan(y / x);
    if (x > 0 && y < 0)
        return atan(y / x) + 2 * M_PI;
    if (x < 0)
        return atan(y / x) + M_PI;
    if (x == 0 && y > 0)
        return M_PI / 2;
    if (x == 0 && y < 0)
        return 3 * M_PI / 2;
}

// binomial_distribution

double randomNumber(double min, double max)
{
    double n = randomGenerator() / 1000.0;
    n -= floor(n);

    return min + (max - min) * n;
}

double randomNumber_BinomialLaw(double min, double max)
{
    binomial_distribution<int> distribution(1000*(max - min), 0.5);

    return min + distribution(randomGenerator) / 1000.0;
}

double square(double a)
{
    return a*a;
}

bool intersection_point_point(Shape& shape1, Shape& shape2)
{
    if (*shape1.origin + shape1.points[0] == *shape2.origin + shape2.points[0]) return true;

    return false;
}

bool intersection_point_circle(Shape& shape1, Shape& shape2)
{
    if (Vector2d::distance(*shape1.origin + shape1.points[0], *shape2.origin + shape2.points[0]) <= shape2.radius1) return true;

    return false;
}

bool intersection_point_rectangle(Shape& shape1, Shape& shape2)
{
    Vector2d vec_AB = shape2.points[1] - shape2.points[0];
    Vector2d vec_AC = shape2.points[2] - shape2.points[0];
    Vector2d vec_AH = *shape1.origin + shape1.points[0] - (*shape2.origin + shape2.points[0]);

    double h1 = Vector2d::dotProduct(vec_AH, vec_AB) / vec_AB.length();
    double h2 = Vector2d::dotProduct(vec_AH, vec_AC) / vec_AC.length();

    if (h1 >= 0 && h1 <= vec_AB.length() && h2 >= 0 && h2 <= vec_AC.length())
        return true;

    return false;
}

bool intersection_point_line(Shape& shape1, Shape& shape2)
{
    tools::debug::warning("intersection_point_line() is not implemented", "tools::math", __FILENAME__, __LINE__);
    return false;
}

bool intersection_point_arc(Shape& shape1, Shape& shape2)
{
    if (Vector2d::distanceSquare(*shape1.origin + shape1.points[0], *shape2.origin + shape2.points[0]) > square(shape2.radius1))
        return false;

    Vector2d vec_AB = *shape1.origin + shape1.points[0] - (*shape2.origin + shape2.points[0]);
    if (angle(vec_AB.x, vec_AB.y) >= shape2.angle1 - shape2.angle2 && angle(vec_AB.x, vec_AB.y) <= shape2.angle1 + shape2.angle2)
        return true;

    return false;
}

bool intersection_circle_circle(Shape& shape1, Shape& shape2)
{
    if (Vector2d::distanceSquare(*shape1.origin + shape1.points[0], *shape2.origin + shape2.points[0]) <= square(shape1.radius1 + shape2.radius1)) return true;

    return false;
}

bool intersection_circle_rectangle(Shape& shape1, Shape& shape2)
{
    Vector2d vec_AB = shape2.points[1] - shape2.points[0];
    Vector2d vec_AC = shape2.points[2] - shape2.points[0];
    Vector2d vec_AH = *shape1.origin + shape1.points[0] - (*shape2.origin + shape2.points[0]);

    double h1 = Vector2d::dotProduct(vec_AH, vec_AB) / vec_AB.length();
    double h2 = Vector2d::dotProduct(vec_AH, vec_AC) / vec_AC.length();

    if (h1 >= - shape1.radius1 && h1 <= vec_AB.length() + shape1.radius1 && h2 >= - shape1.radius1 && h2 <= vec_AC.length() + shape1.radius1)
        return true;

    return false;
}

bool intersection_circle_line(Shape& shape1, Shape& shape2)
{
    Vector2d vec_AB = shape2.points[1] - shape2.points[0];
    Vector2d vec_AC(-vec_AB.y, vec_AB.x);
    Vector2d vec_AH = *shape1.origin + shape1.points[0] - (*shape2.origin + shape2.points[0]);

    double h1 = Vector2d::dotProduct(vec_AH, vec_AB) / vec_AB.length();
    double h2 = Vector2d::dotProduct(vec_AH, vec_AC) / vec_AC.length();

    if (h1 >= - shape1.radius1 && h1 <= vec_AB.length() + shape1.radius1 && abs(h2) <= shape1.radius1)
        return true;

    return false;
}

bool intersection_circle_arc(Shape& shape1, Shape& shape2)
{
    if (!intersection_circle_circle(shape1, shape2))
        return false;

    Vector2d vec_AB = *shape1.origin + shape1.points[0] - (*shape2.origin + shape2.points[0]);
    if (angle(vec_AB.x, vec_AB.y) >= shape2.angle1 - shape2.angle2 && angle(vec_AB.x, vec_AB.y) <= shape2.angle1 + shape2.angle2)
        return true;

    Shape line1;
    line1.line(shape2.points[0], shape2.radius1, shape2.angle1 - shape2.angle2);
    line1.setOrigin(shape2.origin);
    if (intersection_circle_line(shape1, line1))
        return true;

    Shape line2;
    line2.line(shape2.points[0], shape2.radius1, shape2.angle1 + shape2.angle2);
    line2.setOrigin(shape2.origin);
    if (intersection_circle_line(shape1, line2))
        return true;

    return false;
}

bool intersection_rectangle_rectangle(Shape& shape1, Shape& shape2)
{
    Shape p;
    for (int i = 0 ; i < 4 ; ++i)
    {
        p.points.clear();
        p.point(shape1.points[i]);
        p.setOrigin(shape1.origin);
        if (intersection_point_rectangle(p, shape2)) return true;
        p.points.clear();
        p.point(shape2.points[i]);
        p.setOrigin(shape2.origin);
        if (intersection_point_rectangle(p, shape1)) return true;
    }

    return false;
}

bool intersection_rectangle_line(Shape& shape1, Shape& shape2)
{
    tools::debug::warning("intersection_rectangle_line() is not implemented", "tools::math", __FILENAME__, __LINE__);
    return false;
}

bool intersection_rectangle_arc(Shape& shape1, Shape& shape2)
{
    Shape p;
    for (int i = 0 ; i < 4 ; ++i)
    {
        p.points.clear();
        p.point(shape1.points[i]);
        p.setOrigin(shape1.origin);
        if (intersection_point_arc(p, shape2)) return true;
    }

    p.points.clear();
    p.point(shape2.points[0]);
    p.setOrigin(shape2.origin);
    if (intersection_point_rectangle(p, shape1)) return true;

    p.points.clear();
    p.point(shape2.points[0] + Vector2d(shape2.radius1 + cos(shape2.angle1 - shape2.angle2), shape2.radius1 + sin(shape2.angle1 - shape2.angle2)));
    p.setOrigin(shape2.origin);
    if (intersection_point_rectangle(p, shape1)) return true;

    p.points.clear();
    p.point(shape2.points[0] + Vector2d(shape2.radius1 + cos(shape2.angle1), shape2.radius1 + sin(shape2.angle1)));
    p.setOrigin(shape2.origin);
    if (intersection_point_rectangle(p, shape1)) return true;

    p.points.clear();
    p.point(shape2.points[0] + Vector2d(shape2.radius1 + cos(shape2.angle1 + shape2.angle2), shape2.radius1 + sin(shape2.angle1 + shape2.angle2)));
    p.setOrigin(shape2.origin);
    if (intersection_point_rectangle(p, shape1)) return true;

    return false;
}

bool intersection_line_line(Shape& shape1, Shape& shape2)
{
    tools::debug::warning("intersection_line_line() is not implemented", "tools::math", __FILENAME__, __LINE__);
    return false;
}

bool intersection_line_arc(Shape& shape1, Shape& shape2)
{
    tools::debug::warning("intersection_line_arc() is not implemented", "tools::math", __FILENAME__, __LINE__);
    return false;
}

bool intersection_arc_arc(Shape& shape1, Shape& shape2)
{
    tools::debug::warning("intersection_arc_arc() is not implemented", "tools::math", __FILENAME__, __LINE__);
    return false;
}

bool intersection(Shape& shape1, Shape& shape2)
{
    //Check if there is no 'none' profile
    if (shape1.profile == Shape::Profiles::None || shape2.profile == Shape::Profiles::None)
        return false;

    //Simplify the switches
    if (shape2.profile < shape1.profile)
        return intersection(shape2, shape1);

    //Shapes with no origin cannot intersect
    if (shape1.origin == nullptr || shape2.origin == nullptr)
    {
        tools::debug::warning("intersection() has been called with a shape which has no origin", "tools::math", __FILENAME__, __LINE__);
        return false;
    }

    //If the boxes have no intersection, shapes will not have one either
    if (shape1.origin->x + shape1.box.first.x > shape2.origin->x + shape2.box.second.x) return false;
    if (shape1.origin->y + shape1.box.first.y > shape2.origin->y + shape2.box.second.y) return false;
    if (shape1.origin->x + shape1.box.second.x < shape2.origin->x + shape2.box.first.x) return false;
    if (shape1.origin->y + shape1.box.second.y < shape2.origin->y + shape2.box.first.y) return false;

    //if shape2 is a complex shape
    if (shape2.profile == Shape::Profiles::Complex)
    {
        Shape* current = shape2.next;
        while (current != nullptr)
        {
            if (intersection(*current, shape1))
                return true;
            current = current->next;
        }
        return false;
    }

    //Find the good function to test intersection
    switch (shape1.profile)
    {
        case Shape::Profiles::Point:
            switch(shape2.profile)
            {
                case Shape::Profiles::Point:
                    return intersection_point_point(shape1, shape2);
                case Shape::Profiles::Circle:
                    return intersection_point_circle(shape1, shape2);
                case Shape::Profiles::Rectangle:
                    return intersection_point_rectangle(shape1, shape2);
                case Shape::Profiles::Line:
                    return intersection_point_line(shape1, shape2);
                case Shape::Profiles::Arc:
                    return intersection_point_arc(shape1, shape2);
            }
            break;
        case Shape::Profiles::Circle:
            switch(shape2.profile)
            {
                case Shape::Profiles::Circle:
                    return intersection_circle_circle(shape1, shape2);
                case Shape::Profiles::Rectangle:
                    return intersection_circle_rectangle(shape1, shape2);
                case Shape::Profiles::Line:
                    return intersection_circle_line(shape1, shape2);
                case Shape::Profiles::Arc:
                    return intersection_circle_arc(shape1, shape2);
            }
            break;
        case Shape::Profiles::Rectangle:
            switch(shape2.profile)
            {
                case Shape::Profiles::Rectangle:
                    return intersection_rectangle_rectangle(shape1, shape2);
                case Shape::Profiles::Line:
                    return intersection_rectangle_line(shape1, shape2);
                case Shape::Profiles::Arc:
                    return intersection_rectangle_arc(shape1, shape2);
            }
            break;
        case Shape::Profiles::Line:
            switch(shape2.profile)
            {
                case Shape::Profiles::Line:
                    return intersection_line_line(shape1, shape2);
                case Shape::Profiles::Arc:
                    return intersection_line_arc(shape1, shape2);
            }
            break;
        case Shape::Profiles::Arc:
            switch(shape2.profile)
            {
                case Shape::Profiles::Arc:
                    return intersection_arc_arc(shape1, shape2);
            }
    }

    return false;
}

} //namespace math

} //namespace tools
