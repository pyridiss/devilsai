
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

#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <string>

class Carte;
class Classe_Commune;
class Paysage;
class Joueur;

using namespace std;

namespace gamedata{

void addWorld(const string& id);
void clearWorlds();
Carte* world(const string& id);
void addSpecies(const string& s);
void clearSpecies();
Classe_Commune* species(const string& s);
void addInertItemDesign(const string& design);
void clearInertItemDesigns();
Paysage* inertItemDesign(const string& type);
void copyInertItemFromDesign(string t, Paysage *elem);
Joueur* player();
void setPlayer(Joueur* p, Carte *w);
Carte* currentWorld();
Element_Carte* findElement(int id);
Individu_Unique* findIndividuUnique(string type);

} //namespace gamedata

#endif // GAMEDATA_H
