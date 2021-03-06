
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2016  Quentin Henriet

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

#ifndef GUI_DROPDOWNLIST_H
#define GUI_DROPDOWNLIST_H

#include <string>
#include <vector>

#include "gui/widget.h"

using namespace std;

namespace gui{

class DropDownList : public Widget
{
    private:
        vector<pair<textManager::PlainText, string>> entries;
        unsigned index;

    public:
        DropDownList();
        ~DropDownList() = default;

    public:
        void addEntry(textManager::PlainText& entry, string data);

        bool mouseHovering();
        bool mouseHoveringDeveloped();
        bool activated(sf::Event event);

        void setValue(optionType v);

        void display(sf::RenderWindow& app);
};

} //namespace gui

#endif // GUI_DROPDOWNLIST_H
