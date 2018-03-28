
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2017  Quentin Henriet

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

#ifndef GUI_SCROLLINGLIST_H
#define GUI_SCROLLINGLIST_H

#include <vector>

#include "gui/widget.h"

using namespace std;

namespace gui{

class ScrollingList : public Widget
{
    private:
        vector<pair<textManager::PlainText, string>> entries;
        unsigned index;
        double scrolling = 0;
        bool mouseScrolling = false;
        unsigned firstEntryDisplayed;

    public:
        ScrollingList();
        ~ScrollingList() = default;

    public:
        void addEntry(textManager::PlainText& entry, string data);
        void removeAllEntries();

        bool mouseHovering();
        bool activated(sf::Event event);

        void setValue(optionType v);

        void display(sf::RenderWindow& app);
};

} //namespace gui

#endif // GUI_SCROLLINGLIST_H
