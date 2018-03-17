
/*
    devilsai - An Action-RPG game
    Copyright (C) 2009-2018  Quentin Henriet

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

#ifndef DEVILSAI_RESOURCES_SKILL
#define DEVILSAI_RESOURCES_SKILL

#include <string>
#include <map>

#include "tools/shape.h"

#include "devilsai-resources/stats.h"

using namespace std;

class lua_State;

namespace tinyxml2 {
    class XMLHandle;
}

class Skill;
class Individu;

typedef map < string, Skill > MapActivites;

class Skill
{
    public:
        string Id;
        tools::math::Shape interactionField;
        map< pair<double , int> , string > Animation;
        string scriptString;

        unsigned numberOfImages;
        int priority;
        int speed;
        int step;
        Attribute speedImprover;
        lua_State* script;

    public:
        Skill(string id);
        Skill(const Skill& other);
        Skill& operator=(const Skill& right);
        ~Skill();

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

#endif // DEVILSAI_RESOURCES_SKILL