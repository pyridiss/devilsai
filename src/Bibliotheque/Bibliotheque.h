
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

#ifndef header_bibliotheque
#define header_bibliotheque

#include <string>

namespace sf
{
    class RenderTarget;
}

using namespace std;
using namespace sf;


/** DÉCLARATION DES FONCTIONS **/

/* Fonctions définies dans le fichier Decorations.cpp */

void Load_Decorations();
void Supprimer_Decorations();
void Disp_JaugesVie(RenderTarget& target);
bool Disp_Repos(RenderTarget& target);

/* Fonctions définies dans le fichier Utilitaires.cpp */

double ToSegment(double, int, int);
void Verbose(const string&, const short&);
void Erreur(const string&, const string&);
void Erreur(const string&, const float&);
string intToString(double, int size = -1);

#endif
