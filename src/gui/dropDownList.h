
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
using namespace sf;

namespace gui{

class DropDownList : public Widget
{
    private:
        vector<pair<String32, string>> entries;
        int index;

    public:
        DropDownList();
        ~DropDownList() = default;

    public:
        void addEntry(String32& entry, string data);

        bool mouseHovering(RenderWindow& app);
        bool mouseHoveringDeveloped(RenderWindow& app);
        bool activated(RenderWindow& app, Event event);

        void setData(tools::signals::SignalData& data);
        void setValue(const tools::signals::SignalData& d);

        void display(RenderWindow& app);
};

} //namespace gui

#endif // GUI_DROPDOWNLIST_H

