
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

#include <SFML/System.hpp>

using namespace std;
using namespace sf;

class lua_State;

class Caracteristiques;
class Activite;
class Joueur;
class Individu_Unique;
class Projectile;
class Bouton;
class EmplacementEquipement;

typedef map < int, int > MapVitesses;
typedef map < string, lua_State* > mapObjects;
typedef basic_string<Uint32> String32;
typedef map < int, Activite > MapActivites;
typedef map < string, lua_State* > mapSkills;
typedef list <EmplacementEquipement> ListEmplacementsEquipements;

class Statistiques
{
	public:
		float Vitalite		= 1000;
		float Energie		= 1000;
		float Recuperation	= 0;
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
};

class Caracteristiques
{
	public:
		unsigned int Force				= 0;
		unsigned int Puissance			= 0;
		unsigned short Agilite			= 0;
		unsigned short Intelligence		= 0;

	public:
		unsigned short Constitution		= 0;
		unsigned short Charisme			= 0;
		unsigned short Esquive			= 0;
		short int RecuperationMoyenne	= 0;

	public:
		MapVitesses Vitesses;

	public:
		Objects objects;
		mapSkills skills;

	public:
		~Caracteristiques();

	public:
		unsigned operator[](string characteristic);

	public:
		void addSkill(string newSkill, int owner);
		void manageSkills();
		void saveSkills(ofstream& stream);
		void loadSkillFromSavedGame(string skill, string data);
		void deleteSkills();
};

class Activite
{
	public:
		int Id				= -1;
		short Num_Max[8]	= {-1, -1, -1, -1, -1, -1, -1, -1};
		short Vitesse		= 0;
		short MaJ			= 0;

		short Blocage		= 0;

	public:
		int NombreDirections();
};

class EmplacementEquipement
{
	public:
		Bouton* BoutonEquipement	= nullptr;

	public:
		string CategorieObjet		= "";
		string TypeObjet			= "";
		string ClasseObjet			= "";

	public:
		~EmplacementEquipement();

	public:
		void Set(int x, int y, int w, int h);
		string Get_IdEmplacement();
};

void Gestion_Competences();
void Disp_EmplacementVide(string);
void Disp_Equipement();
void Load_Decorations_Objets();
void Supprimer_Decorations_Objets();

#endif
