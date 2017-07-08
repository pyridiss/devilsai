
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

#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include <string>

#include "gui/widget.h"

using namespace std;
using namespace sf;

namespace gui{

class Button : public Widget
{
    private:
        bool autoRelease = true;

    public:
        Button();
        ~Button() = default;

    public:
        void setAutoRelease(bool a);

        bool mouseHovering(RenderWindow& app);
        bool activated(RenderWindow& app, Event event);

        void setData(tools::signals::SignalData& data);
        void setValue(const tools::signals::SignalData& d);
};

} //namespace gui

#endif // GUI_BUTTON_H

