
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
#include <string_view>
#include <map>
#include <list>

#include <tinyxml2.h>

#include "tools/shape.h"
#include "gui/button.h"

using namespace std;
using namespace sf;
using namespace tinyxml2;

class lua_State;

class Activite;
class Individu;

typedef map < string, lua_State* > mapObjects;
typedef map < string, Activite > MapActivites;

enum Attribute {
    Life = 0, Energy, Healing,
    Strength, Power, Agility, Intellect,
    Constitution, Charisma, Dodge, HealingPower,
    RunSpeed, AttackSpeed, InjurySpeed,
    numberOfAttributes
};

enum AttributeAmplifier {
    LifeAmplifier = 0, EnergyAmplifier, HealingAmplifier,
    StrengthAmplifier, PowerAmplifier, AgilityAmplifier, IntellectAmplifier,
    ConstitutionAmplifier, CharismaAmplifier, DodgeAmplifier, HealingPowerAmplifier,
    RunSpeedAmplifier, AttackSpeedAmplifier, InjurySpeedAmplifier
};

constexpr const char* AttributesNames[] = {
    "life", "energy", "healing",
    "strength", "power", "agility", "intellect",
    "constitution", "charisma", "dodge", "healingPower",
    "runSpeed", "attackSpeed", "injurySpeed"
};

constexpr const char* AttributesAmplifiersNames[] = {
    "lifeAmplifier", "energyAmplifier", "healingAmplifier",
    "strengthAmplifier", "powerAmplifier", "agilityAmplifier", "intellectAmplifier",
    "constitutionAmplifier", "charismaAmplifier", "dodgeAmplifier", "healingPowerAmplifier",
    "runSpeedAmplifier", "attackSpeedAmplifier", "injurySpeedAmplifier"
};

class Statistiques
{
	private:
        double _stats[numberOfAttributes];

	public:
        Statistiques()
            : _stats {1000, 1000} //Only Life and Energy are 1000 by default
        {
        }

        constexpr double operator[](Attribute a)
        {
            if (a == numberOfAttributes) return 0;
            return _stats[a];
        }
        void add(Attribute a, double value)
        {
            if (a == numberOfAttributes) return;
            _stats[a] += value;
        }
        void set(Attribute a, double value)
        {
            if (a == numberOfAttributes) return;
            _stats[a] = value;
        }

    public:
        void loadFromXML(tinyxml2::XMLElement* elem)
        {
            for (int i = 0 ; i < numberOfAttributes ; ++i)
                elem->QueryAttribute(AttributesNames[i], &_stats[i]);
        }
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle)
        {
            XMLElement* root = handle.ToElement();

            for (int i = 0 ; i < numberOfAttributes ; ++i)
            {
                if (_stats[i])
                    root->SetAttribute(AttributesNames[i], _stats[i]);
            }
        }
};

class Objects
{
    public:
        struct objectDesign
        {
            string file;
            int quality;
            int probability;

            objectDesign(const string& f, int q, int p) :
                file(f),
                quality(q),
                probability(p)
            {
            }
        };

	public:
		mapObjects objects;
        list<objectDesign> designs;

	public:
		~Objects();

	public:
		lua_State* addObject(string newObject, string key, int qualityRandomObject = 0);
        void createObjectsFromDesigns();
		void deleteObject(lua_State* obj);
		void deleteObjects();

        void loadFromXML(tinyxml2::XMLElement* elem);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);
};

class Activite
{
	public:
        string Id          = "-1";
        unsigned numberOfImages;
        int speed          = 0;
        int step           = 0;
        bool fightAtEnd    = false;
        string scriptString;
        lua_State* script  = nullptr;
        Attribute speedImprover = numberOfAttributes;
        tools::math::Shape interactionField;

        int priority       = 0;

        map< pair<double , int> , string > Animation;

	public:
        Activite();
        Activite(const Activite& other);
        Activite& operator=(const Activite& right);
        ~Activite();

        void addImage(double angle, int num, string imageKey);
        string getImageKey(double angle, int num);

        void loadFromXML(tinyxml2::XMLHandle &handle, Individu* owner = nullptr);
        void loadScript();

        void atBegin(Individu* owner);
        void atEnd(Individu* owner);
};

static constexpr const char* attributeToString(Attribute a)
{
    return AttributesNames[static_cast<int>(a)];
}
static constexpr const char* attributeAmplifierToString(AttributeAmplifier a)
{
    return AttributesAmplifiersNames[static_cast<int>(a)];
}
static constexpr Attribute stringToAttribute(string_view a)
{
    for (int i = 0 ; i < numberOfAttributes ; ++i)
        if (AttributesNames[i] == a)
            return static_cast<Attribute>(i);
    return numberOfAttributes;
}
static constexpr AttributeAmplifier stringToAttributeAmplifier(string_view a)
{
    for (int i = 0 ; i < numberOfAttributes ; ++i)
        if (AttributesAmplifiersNames[i] == a)
            return static_cast<AttributeAmplifier>(i);
    return static_cast<AttributeAmplifier>(numberOfAttributes);
}

#endif
