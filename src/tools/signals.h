
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

#ifndef TOOLS_SIGNALS_H
#define TOOLS_SIGNALS_H

#include <string>
#include <list>

#include <SFML/System/Utf.hpp>

using namespace std;
using namespace sf;

typedef basic_string<Uint32> String32;

namespace tools{

namespace signals{

struct SignalData
{
    int intData;
    double doubleData;
    bool boolData;
    string stringData;
    String32 String32Data;

    SignalData();
    SignalData(const SignalData&) = default;
    SignalData(SignalData&&) noexcept = default;
    SignalData& operator=(const SignalData&) = default;
};

struct SignalListener
{
    string signal;
    bool signalSent = false;
};

typedef pair < string , SignalData > Signal;

void registerListener(SignalListener* l);
void addSignal(string s, SignalData d = SignalData());
Signal& getNextSignal();
void removeSignal();

} //namespace signals

} //namespace tools

#endif // TOOLS_SIGNALS_H

