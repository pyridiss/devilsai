
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

#include <list>

#include "tools/signals.h"

namespace tools{

namespace signals{

list<Signal> signals;
vector< pair<string, queue<Signal>*> > listeners;

Signal emptySignal;


void registerListener(string s, queue<Signal>* v)
{
    listeners.emplace_back(std::move(s), v);
}

void addSignal(string s, string d)
{
    for (auto& l : listeners)
    {
        if (l.first == s)
            l.second->emplace(s, d);
    }

    signals.push_back(Signal(s, d));
}

Signal& getNextSignal()
{
    if (signals.empty())
    {
        return emptySignal;
    }
    return signals.front();
}

void removeSignal()
{
    signals.pop_front();
}

} //namespace signals

} //namespace tools
