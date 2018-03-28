
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

#ifndef GUI_TEXTWIDGET_H
#define GUI_TEXTWIDGET_H

#include "gui/widget.h"

using namespace std;

namespace gui{

class TextWidget : public Widget
{
    private:
        int _verticalScrolling;

    public:
        TextWidget();
        ~TextWidget() = default;

    public:
        void setVerticalScrolling(int percentage);
        bool mouseHovering();
        bool mouseHoveringVerticalScrollBar();
        bool activated(sf::Event event);

        void setValue(optionType v);

        void display(sf::RenderWindow& app);
};

} //namespace gui

#endif // GUI_TEXTWIDGET_H

