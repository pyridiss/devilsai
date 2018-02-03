
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
#include <map>

#include "tools/math.h"
#include "tools/aStar.h"

using namespace std;

namespace tools{

namespace aStar{

struct Node
{
    double costFromStart, costToEnd, costSum;
    math::Vector2d parent;

    Node()
      : costFromStart(0),
        costToEnd(0),
        costSum(0),
        parent()
    {
    }
};

struct compare
{
    bool operator()(const math::Vector2d& left, const math::Vector2d& right) const
    {
        if (left.x == right.x) return (left.y < right.y);
        return left.x < right.x;
    }
};

map < math::Vector2d, Node, compare > _openSet;
map < math::Vector2d, Node, compare > _closedSet;

math::Shape _finalPath;

math::Vector2d _start, _end;

math::Shape _field;
vector<pair<math::Shape*, int>> _obstacles;
math::Shape _nodeShape;
unsigned _childrenNumber;
double _distance;
unsigned _maximumNodes;

void setPoints(const math::Vector2d& start, const math::Vector2d& end)
{
    _start.x = floor(start.x);
    _start.y = floor(start.y);
    _end.x = floor(end.x);
    _end.y = floor(end.y);
}

void setField(math::Shape& field)
{
    _field = field;
    _field.origin = field.origin;
}

void setNodesProperties(math::Shape& shape, unsigned childrenNumber, double distance, unsigned maxNodes)
{
    _nodeShape = shape;
    _childrenNumber = childrenNumber;
    _distance = distance;
    _maximumNodes = maxNodes;
}

void setObstacles(vector<pair<math::Shape*,int>>& obstacles)
{
    _obstacles = obstacles;
}

void toClosedSet(math::Vector2d& p)
{
    Node& n = _openSet[p];
    _closedSet.emplace(p, std::move(n));
    _openSet.erase(p);
}

void addAdjacentNodes(math::Vector2d& n)
{
    if (_closedSet.size() + _openSet.size() > _maximumNodes) return;

    for (unsigned i = 0 ; i < _childrenNumber ; ++i)
    {
        math::Vector2d newNode;

        newNode.x = floor(n.x + _distance * cos(i * (2 * M_PI) / _childrenNumber));
        newNode.y = floor(n.y + _distance * sin(i * (2 * M_PI) / _childrenNumber));

        _nodeShape.origin = &newNode;

        if (!math::intersection(_field, _nodeShape))
            continue;

        bool free = true;
        for (auto& c : _closedSet)
            if (math::Vector2d::distanceSquare(newNode, c.first) < 0.9 * _distance * _distance)
            {
                free = false;
                break;
            }

        if (!free)
            continue;

        int additionalCost = 0;
        for (auto& o : _obstacles)
            if (math::intersection(*(o.first), _nodeShape))
            {
                additionalCost += o.second;
            }

        if (additionalCost >= 100000)
            continue;

        Node tmp;
        tmp.costFromStart = _closedSet[n].costFromStart + math::Vector2d::distanceSquare(newNode, n);
        tmp.costToEnd = math::Vector2d::distanceSquare(newNode, _end);
        tmp.costSum = tmp.costFromStart + tmp.costToEnd + additionalCost;

        tmp.parent = n;

        if (_openSet.find(newNode) != _openSet.end())
        {
            if (tmp.costSum < _openSet[newNode].costSum)
                _openSet[newNode] = tmp;
        }
        else
            _openSet.emplace(std::move(newNode), std::move(tmp));
    }
}

math::Shape aStar()
{
    Node startNode;
    startNode.parent.x = 0;
    startNode.parent.y = 0;

    _openSet.emplace(_start, startNode);
    toClosedSet(_start);
    addAdjacentNodes(_start);

    math::Vector2d tmp = _start;

    while(math::Vector2d::distanceSquare(tmp, _end) > 4 * _distance * _distance && !_openSet.empty())
    {
        double f = _openSet.begin()->second.costSum;
        tmp = _openSet.begin()->first;

        for (auto& i : _openSet)
        {
            if (i.second.costSum < f)
            {
                f = i.second.costSum;
                tmp = i.first;
            }
        }

        toClosedSet(tmp);
        addAdjacentNodes(tmp);
    }

    if (math::Vector2d::distanceSquare(tmp, _end) <= 4 * _distance * _distance)
    {
        _finalPath.profile = math::Shape::Profiles::Line;

        Node& tmp2 = _closedSet[tmp];
        math::Vector2d current = _end, previous = tmp2.parent;

        _finalPath.points.push_back(current);

        while (previous.x != 0 && previous.y != 0)
        {
            current = previous;
            _finalPath.points.push_back(current);

            tmp2 = _closedSet[tmp2.parent];
            previous = tmp2.parent;
        }
    }
    else
        _finalPath.profile = math::Shape::Profiles::None;

    return _finalPath;
}

void clear()
{
    _finalPath.points.clear();
    _openSet.clear();
    _closedSet.clear();
}

} //namespace aStar

} //namespace tools
