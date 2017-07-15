
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

#ifndef header_attributs
#define header_attributs

#include <string>
#include <map>
#include <list>

#include "gui/button.h"

using namespace std;
using namespace sf;

class lua_State;

class Caracteristiques;
class Activite;
class Individu;
class Joueur;
class Individu_Unique;
class EmplacementEquipement;

namespace tinyxml2{
    class XMLElement;
    class XMLHandle;
    class XMLDocument;
};

typedef map < int, int > MapVitesses;
typedef map < string, lua_State* > mapObjects;
typedef basic_string<Uint32> String32;
typedef map < string, Activite > MapActivites;
typedef map < string, lua_State* > mapSkills;
typedef list <EmplacementEquipement> ListEmplacementsEquipements;

class Statistiques
{
	private:
		float Vitalite		= 1000;
		float Energie		= 1000;
		float Recuperation	= 0;

	public:
		float& operator[](string stat);
		const float& operator[](string stat) const;

        void loadFromXML(tinyxml2::XMLElement* elem);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);
};

class Objects
{
	public:
		mapObjects objects;

	public:
		~Objects();

	public:
		pair<mapObjects::iterator, bool> addObject(string newObject, string key, int qualityRandomObject = 0);
		void deleteObject(lua_State* obj);
		void saveObjects(ostream& stream);
		void loadObjectFromSavedGame(string object, string key, string data);
		void deleteObjects();

        void loadFromXML(tinyxml2::XMLElement* elem);
};

class Caracteristiques
{
	private:
		int Force				= 0;
		int Puissance			= 0;
		int Agilite				= 0;
		int Intelligence		= 0;

		int Constitution		= 0;
		int Charisme			= 0;
		int Esquive				= 0;
		int RecuperationMoyenne	= 0;

	public:
		MapVitesses Vitesses;

	public:
		Objects objects;
		mapSkills skills;

	public:
		~Caracteristiques();

	public:
		int& operator[](string characteristic);
		const int& operator[](string characteristic) const;

	public:
		void addSkill(string newSkill, Individu* owner);
		void manageSkills();
		void saveSkills(ofstream& stream);
		void loadSkillFromSavedGame(string skill, string data);
		void deleteSkills();

        void loadFromXML(tinyxml2::XMLElement* elem);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);

	public:
		pair<int, int> getFromObjectsAndSkills(string characteristic);
};

class Activite
{
	public:
        string Id          = "-1";
        int numberOfImages = 0;
        int speed          = 0;
        int step           = 0;
        bool fightAtEnd    = false;
        string scriptString;
        lua_State* script  = nullptr;

        int priority       = 0;

        map< pair<double , int> , string > Animation;

	public:
        ~Activite();

        void addImage(double angle, int num, string imageKey);
        string getImageKey(double angle, int num);

        void loadFromXML(tinyxml2::XMLHandle &handle);
        void loadScript();

        void atEnd(Individu* owner);
};

class EmplacementEquipement
{
	public:
        gui::Button* BoutonEquipement;

	public:
		string CategorieObjet;
		string TypeObjet;
		string ClasseObjet;

	public:
        EmplacementEquipement();
		~EmplacementEquipement();
        EmplacementEquipement(EmplacementEquipement&& e);

	public:
		void Set(int x, int y, int w, int h);
		string Get_IdEmplacement();
};

void Disp_Competences();
void Gestion_Competences(Event&);
void Disp_EmplacementVide(string);
void Disp_Equipement();
void Load_Decorations_Objets();
void Supprimer_Decorations_Objets();

#endif
