
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

namespace tinyxml2{
    class XMLElement;
    class XMLHandle;
    class XMLDocument;
};

typedef map < string, lua_State* > mapObjects;
typedef basic_string<Uint32> String32;
typedef map < string, Activite > MapActivites;

class Statistiques
{
	private:
		double Vitalite		= 1000;
		double Energie		= 1000;
		double Recuperation	= 0;

	public:
        enum Attribute { Life, Energy, Healing,
                         enumSize };

        double operator[](string stat) const;
        double operator[](Attribute a);
        void add(Attribute a, double value);
        void set(Attribute a, double value);
        static string toString(Attribute a);

    private:
        double& get(Attribute a);

    public:
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
		lua_State* addObject(string newObject, string key, int qualityRandomObject = 0);
		void deleteObject(lua_State* obj);
		void deleteObjects();

        void loadFromXML(tinyxml2::XMLElement* elem);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);
};

class Caracteristiques
{
	private:
        double strength, power, agility, intellect;
        double constitution, charisma, dodge, healingPower;
        double runSpeed, attackSpeed, injurySpeed;

	public:
        Caracteristiques();
		~Caracteristiques();

	public:
        enum Attribute { Strength, Power, Agility, Intellect,
                         Constitution, Charisma, Dodge, HealingPower,
                         RunSpeed, AttackSpeed, InjurySpeed,
                         enumSize };

        double operator[](string characteristic) const;
        double operator[](Attribute a);
        void add(Attribute a, double value);
        void set(Attribute a, double value);

    private:
        double& get(Attribute a);

	public:
        static string toString(Attribute a);

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

        void atBegin(Individu* owner);
        void atEnd(Individu* owner);
};


void Disp_Competences();
void Gestion_Competences(Event&);
void Disp_EmplacementVide(string);
void Disp_Equipement();
void Load_Decorations_Objets();
void Supprimer_Decorations_Objets();

static const string ObjectProperties[19] = {
    "strength",
    "power",
    "agility",
    "intellect",
    "constitution",
    "charisma",
    "dodge",
    "healingPower",
    "strengthFactor",
    "powerFactor",
    "agilityFactor",
    "intellectFactor",
    "constitutionFactor",
    "charismaFactor",
    "dodgeFactor",
    "healingPowerFactor",
    "attackSpeed",
    "runSpeed",
    "injurySpeed"
};

#endif
