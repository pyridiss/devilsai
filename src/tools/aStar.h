
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

#ifndef TOOLS_ASTAR_H
#define TOOLS_ASTAR_H

#include <map>

#include "tools/vector2d.h"
#include "tools/shape.h"

using namespace std;


namespace tools{

namespace aStar{

void setPoints(const math::Vector2d& start, const math::Vector2d& end);
void setField(math::Shape& field);
void setNodesProperties(math::Shape& shape, unsigned childrenNumber, double distance, unsigned maxNodes);
void setObstacles(vector<pair<math::Shape*, int>>& obstacles);
math::Shape aStar();
void clear();

} //namespace aStar

} //namespace tools

#endif // TOOLS_ASTAR_H

