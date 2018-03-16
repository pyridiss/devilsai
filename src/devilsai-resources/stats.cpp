
/*
    devilsai - An Action-RPG game
    Copyright (C) 2009-2018  Quentin Henriet

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

#include <tinyxml2.h>

#include <devilsai-resources/stats.h>

Stats::Stats()
  : _stats {1000, 1000} //Only Life and Energy are 1000 by default
{
}

void Stats::add(Attribute a, double value)
{
    if (a == numberOfAttributes) return;
    _stats[a] += value;
}

void Stats::set(Attribute a, double value)
{
    if (a == numberOfAttributes) return;
    _stats[a] = value;
}

void Stats::loadFromXML(tinyxml2::XMLElement* elem)
{
    for (int i = 0 ; i < numberOfAttributes ; ++i)
        elem->QueryAttribute(AttributesNames[i], &_stats[i]);
}

void Stats::saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle)
{
    tinyxml2::XMLElement* root = handle.ToElement();

    for (int i = 0 ; i < numberOfAttributes ; ++i)
    {
        if (_stats[i])
            root->SetAttribute(AttributesNames[i], _stats[i]);
    }
}
