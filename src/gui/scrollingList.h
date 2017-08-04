
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

#include <list>

#include "gui/widget.h"

using namespace std;
using namespace sf;

namespace gui{

class ScrollingList : public Widget
{
    private:
        vector<pair<String32, string>> entries;
        int index;
        double scrolling = 0;
        bool mouseScrolling = false;
        int firstEntryDisplayed = 0;

    public:
        ScrollingList();
        ~ScrollingList() = default;

    public:
        void addEntry(String32& entry, string data);
        void removeAllEntries();

        bool mouseHovering(RenderWindow& app);
        bool activated(RenderWindow& app, Event event);

        void setValue(const string& d);
        string value();

        void display(RenderWindow& app);
};

} //namespace gui

#endif // GUI_SCROLLINGLIST_H
