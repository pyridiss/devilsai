
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
#include <vector>
#include <map>
#include <set>

#include <SFML/Graphics.hpp>

#include <tools/math.h>
#include <tools/shape.h>
#include <tools/vector2d.h>

using namespace std;
using namespace sf;

namespace tinyxml2{
    class XMLElement;
    class XMLHandle;
    class XMLDocument;
}

class lua_State;

class Classe_Commune;
class Element_Carte;
class Coffre;
class Individu;
class Individu_Unique;
class Individu_Commun;
class Joueur;
class Paysage;
class CheckPoint;
class Trigger;


class Carte
{
	public:
		list <Element_Carte*> elements;

        /*
         * collidingItems will store items which can collide with others.
         * As a vector, it will be more efficient than a list to search for collisions.
         * Moreover, it will avoid take time to check if an item has 'ignoreCollision' set or not.
         */
    private:
        vector <Element_Carte*> collidingItems;

    public:
        list <pair<Element_Carte*, string>> places;
        list <pair<tools::math::Vector2d, tools::math::Shape>> paths;
		string Id = "";
		string backgroundImage = "";
        set <string> dataFiles;
        bool commonDataLoaded;

    private:
        /*
         * _stopManagement must be set when elements are modified outside this class.
         */
        bool _stopManagement;

	//Constructeurs / Destructeurs :
	public:
		Carte();
		~Carte();

	public:
        void insertItem(Element_Carte *elem);
        void removeItem(Element_Carte *elem);
		Individu_Commun* AjouterElement_Commun(string Type, string liste, int x, int y);
        CheckPoint* addCheckPoint(string liste, int x, int y);
		Coffre* AjouterCoffre(string liste, int x, int y);
		void SupprimerElement(Element_Carte* elem);

        Element_Carte* createNewItem(tinyxml2::XMLElement* item);

		void SupprimerListe(string num);
        void stopManagement();

        void loadFromFile(string path, string tag="ALL");
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);

	//Gestion :
	public:
        void GestionElements(const View& worldView);
        void display(RenderTarget& target);

	//Fond de la carte :
	public:
		void displayBackground(RenderTarget& target);

	//Collisions manager
	public:
        vector<Element_Carte*>::iterator currentCollider;
        vector<Element_Carte*>::iterator lastCollider;

	public:
		Element_Carte* getCurrentCollider();
		void resetCollisionManager();
		int browseCollisionList(Individu *elem);
};


void Combat(Individu*, Individu*, lua_State* L = NULL);

#endif
