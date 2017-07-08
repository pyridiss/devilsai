
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

#include "gui/inputField.h"
#include "gui/style.h"

namespace gui{

InputField::InputField()
{
    input.clear();

    addState("normal");

    setTextFont(gui::style::defaultTextFont(), gui::style::defaultTextSize());

    setTextColor("normal", Color::Black);
}

bool InputField::mouseHovering(RenderWindow& app)
{
    if (Mouse::getPosition(app).x >= getXTopLeft() && Mouse::getPosition(app).x <= getXTopLeft() + width &&
        Mouse::getPosition(app).y >= getYTopLeft() && Mouse::getPosition(app).y <= getYTopLeft() + height)
    {
        return true;
    }

    return false;
}

bool InputField::activated(RenderWindow& app, Event event)
{
    if (event.type == Event::TextEntered)
    {
        switch(event.text.unicode)
        {
            case 8 :    if (input.size() > 0) input.resize(input.size()-1);
                        break;  // Backspace
            case 13 :   break;  // Return
            case 27 :   input.clear();
                        break;  // Escape
            default :   input += event.text.unicode;
                        break;
        }
    }

    return false;
}

void InputField::setData(tools::signals::SignalData& data)
{
    data.String32Data = input;
}

void InputField::setValue(const tools::signals::SignalData& d)
{
    input = d.String32Data;
}

void InputField::display(RenderWindow& app)
{
    setAllText(input);
    updateTextPosition();

    Widget::display(app);
}

} //namespace gui
