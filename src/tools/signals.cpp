
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

#include <iostream>
#include <fstream>
#include "Bibliotheque/Templates.h"
#include "tools/signals.h"

namespace tools{

namespace signals{

list<Signal> signals;
Signal emptySignal;

SignalData::SignalData()
{
    intData = 0;
    doubleData = 0;
    boolData = false;
    stringData.clear();
    String32Data.clear();
}

void addSignal(string s, SignalData d)
{
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
