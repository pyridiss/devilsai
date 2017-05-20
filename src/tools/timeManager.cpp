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

#include <SFML/System.hpp>

#include "tools/timeManager.h"

using namespace sf;

namespace tools{

namespace timeManager{

double speed = 0;
double FrameNumber = 1;
Time FrameTime;
Clock FrameClock;
bool NewStart = true;

void setSpeed(double s)
{
    speed = s;
}

void frameDone()
{
    FrameTime = FrameClock.getElapsedTime();

    if (NewStart) FrameNumber = 1;
    else ++FrameNumber;

    NewStart = false;

    if (FrameTime.asMilliseconds() > 1000)
    {
        FrameClock.restart();
        NewStart = true;
    }
}

double I(double i)
{
    return speed * FrameTime.asMilliseconds() / FrameNumber * i;
}

double getFPS()
{
    return 1000.0 * FrameNumber / FrameTime.asMilliseconds();
}


} //namespace timeManager

} //namespace tools
