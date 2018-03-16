
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

#include "devilsai-resources/stats.h"
#include "devilsai-resources/wearableItem.h"

using namespace std;
using namespace sf;
using namespace tinyxml2;

class lua_State;

class Activite;
class Individu;

typedef map < string, Activite > MapActivites;


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
        vector<WearableItem> objects;
        list<objectDesign> designs;

	public:
        Objects();
        Objects(const Objects& other);
        Objects& operator=(const Objects& right);
		~Objects();

	public:
        WearableItem& addObject(string newObject, string key, int qualityRandomObject = 0);
        void createObjectsFromDesigns();
        void deleteObject(const WearableItem& key);
		void deleteObjects();

        /**
         * Returns a pointer to the WearableItem whose current slot is given in parameter.
         *
         * \param slot the slot where to find the WearableItem.
         * \return pointer to a WearableItem if any, nullptr otherwhise.
         */
        WearableItem* at(const string& slot);

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

        /**
         * Calls the getDegats() function of the script and returns its result.
         */
        double damage();

        /**
         * Calls the getAmplitude() function of the script and returns its result.
         */
        double amplitude();

        void atBegin(Individu* owner);
        void atEnd(Individu* owner);
};

#endif
