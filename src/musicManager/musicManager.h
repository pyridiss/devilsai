
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

#ifndef MUSICMANAGER_MUSICMANAGER_H
#define MUSICMANAGER_MUSICMANAGER_H

#include <string>

using namespace std;

namespace musicManager{

void addSound(string);
void addMusic(string);
void playSound(string);
void playMusic(string);
void stopMusic(string);
void manageRunningMusics();
void deleteMusics();

} //namespace musicManager

#endif //MUSICMANAGER_MUSICMANAGER_H
