
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2014  Quentin Henriet

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
#include <sstream>
#include <cmath>
#include <string>

#include "tools/math.h"

#include "Constantes.h"
#include "Jeu/options.h"

#define PI 3.1415926


double ToSegment(double x, int min, int max)
{
	return min + 2*(max-min)/PI * atan(x*PI/(2*(max-min)));
}

#ifdef DEBOGAGE
void Verbose(const string &Message, const short &Type)
{
    if(!options::option<bool>(tools::math::sdbm_hash("verbose"))) return;

	switch (Type)
	{
        case FICHIER :
            if (!options::option<bool>(tools::math::sdbm_hash("verbose-files"))) return;
            cout << "VERB.FICH : " << Message << endl;
            break;
        case IMAGE :
            if (!options::option<bool>(tools::math::sdbm_hash("verbose-images"))) return;
            cout << "VERB.IMG  : " << Message << endl;
            break;
        case LISTE :
            if (!options::option<bool>(tools::math::sdbm_hash("verbose-lists"))) return;
            cout << "VERB.LIST : " << Message << endl;
            break;
        case LUA :
            if (!options::option<bool>(tools::math::sdbm_hash("verbose-lua"))) return;
            cout << "VERB.LUA  : " << Message << endl;
            break;
        case MUSIC :
            if (!options::option<bool>(tools::math::sdbm_hash("verbose-musics"))) return;
            cout << "VERB.MUSIC  : " << Message << endl;
            break;
	}
}
#endif

string intToString(double number, int size)
{
	stringstream out;
	out.precision(8);
    if (size != -1)
    {
        out.width(size);
        out.fill('0');
    }
	out << number;
	return out.str();
}
