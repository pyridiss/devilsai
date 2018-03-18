
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

#ifndef DEVILSAI_RESOURCES_INVENTORY
#define DEVILSAI_RESOURCES_INVENTORY

#include <string>
#include <list>
#include <vector>

#include "devilsai-resources/wearableItem.h"

using namespace std;

namespace tinyxml2 {
    class XMLDocument;
    class XMLElement;
    class XMLHandle;
}


class Pack
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
        Pack();
        Pack(const Pack& other);
        Pack(Pack&& other) noexcept;
        Pack& operator=(const Pack& right);
        Pack& operator=(Pack&& right) noexcept;
        ~Pack() = default;

    public:
        WearableItem& addObject(string newObject, string key, int qualityRandomObject = 0);
        void createObjectsFromDesigns();
        void deleteObject(const WearableItem& key);

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

#endif // DEVILSAI_RESOURCES_INVENTORY
