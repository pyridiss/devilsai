
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

#ifndef header_carte
#define header_carte

#include <string>
#include <list>
#include <map>

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

namespace tinyxml2{
    class XMLHandle;
    class XMLDocument;
}

class lua_State;

class Classe_Commune;
class Element_Carte;
class Coffre;
class Cadavre;
class Individu;
class Individu_Unique;
class Individu_Commun;
class Joueur;
class Paysage;
class Door;
class Actionneur;
class Trigger;


class Carte
{
	public:
		list <Element_Carte*> elements;
		list <Trigger*> triggers;
        list <Element_Carte*> places;
		map <string, lua_State*> luaTriggers;
		string Id = "";
		string ambience = "";
		string backgroundImage = "";
        set <string> dataFiles;
        bool commonDataLoaded;

	//Constructeurs / Destructeurs :
	public:
		Carte();
		~Carte();

	//Gestion liste chainée :
	private:
		void AjouterElementEnListe(Element_Carte *elem);

	public:
		Individu_Unique* AjouterElement_Unique(string Type, string liste, int x, int y);
		Individu_Commun* AjouterElement_Commun(string Type, string liste, int x, int y);
		Joueur* AjouterJoueur(string Type, string liste, int x, int y);
		Paysage* AjouterPaysage(string Type, string liste, int x, int y);
		Door* addDoor(string liste, int x, int y);
		Actionneur* AjouterActionneur(string liste, int x, int y);
		Trigger* addTrigger(string liste);
		Coffre* AjouterCoffre(string liste, int x, int y);
		Cadavre* AjouterCadavre(string liste, float x, float y);
		void SupprimerElement(Element_Carte* elem);

		void SupprimerListe(string num);

        void loadFromFile(string path, string tag="ALL");
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);

	//Gestion :
	public:
        void GestionElements(const View& worldView);
        void display(RenderTarget& target);

	//Fond de la carte :
	public:
		void setBackgroundImage(string path);
		void displayBackground(RenderTarget& target);

	//Collisions manager
	public:
		list<Element_Carte*>::iterator currentCollider;
		list<Element_Carte*>::iterator lastCollider;
		int maxRadius = 0;

	public:
		Element_Carte* getCurrentCollider();
		void resetCollisionManager();
		int browseCollisionList(Individu *elem);
};


void Load_Carte(string, bool);
Element_Carte* loadElementsFromStream(istream&, Carte*, string);
void ChangerCarte(Element_Carte*, string, string);

void Combat(Individu*, Individu*, lua_State* L = NULL);

#endif
