
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
#include <unordered_map>
#include <list>

class Carte;
class Classe_Commune;
class Element_Carte;
class Paysage;
class Individu;
class Joueur;
class Journal;
class Dialog;

class lua_State;

namespace tinyxml2{
    class XMLHandle;
    class XMLDocument;
}

namespace textManager{
    class RichText;
}

using namespace std;

namespace gamedata{

void addWorld(const string& id);
Carte* world(const string& id);
const unordered_map<string, Carte*>& worlds();
void addSpecies(const string& s);
Classe_Commune* species(const string& s);
void addInertItemDesign(const string& design);
Paysage* inertItemDesign(const string& type);
void copyInertItemFromDesign(string t, Paysage *elem);
lua_State* sharedTrigger(string name);
unordered_map<string, lua_State*>& quests();
Joueur* player();
list<textManager::RichText>& listDialogs();
void setPlayerName(string s);
Carte* currentWorld();
void clear();
Element_Carte* findElement(int id);
Individu* findIndividuUnique(string type);
list<Element_Carte*> getItemsByTag(const string& w, const string& tag);
void updateCurrentPlace();
void updateCurrentWorld(const string& newWorld);
void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);
void loadGameDataFileAsync(const string& dataDirectory, const string& mainFile);
void loadFromXML(const string& dataDirectory, const string& mainFile);

} //namespace gamedata

#endif // GAMEDATA_H
