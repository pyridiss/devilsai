
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

#ifndef GUI_INPUTFIELD_H
#define GUI_INPUTFIELD_H

#include <string>

#include "gui/widget.h"

using namespace std;

namespace gui{

class InputField : public Widget
{
    private:
        basic_string<unsigned int> input;

    public:
        InputField();
        ~InputField() = default;

    public:
        bool mouseHovering();
        bool activated(sf::Event event);

        void setValue(optionType v);
};

} //namespace gui

#endif // GUI_INPUTFIELD_H

