
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

#include "textManager/textManager.h"

#include "gui/inputField.h"
#include "gui/style.h"

namespace gui{

InputField::InputField()
  : input()
{
    addState("normal");

    states.find("normal")->second.text.setDefaultProperties("liberation", 15, Color::Black);

    _textFlags = textManager::HAlignCenter | textManager::FixedHeight | textManager::VAlignCenter;
}

bool InputField::mouseHovering(RenderWindow& app)
{
    if (Mouse::getPosition(app).x >= left() && Mouse::getPosition(app).x <= left() + width() &&
        Mouse::getPosition(app).y >= top() && Mouse::getPosition(app).y <= top() + height())
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

        setAllText(textManager::PlainText(input));
    }

    return false;
}

void InputField::setValue(const string& d)
{
    input = textManager::fromStdString(d).aggregatedText();
}

string InputField::value()
{
    return textManager::PlainText(input).toStdString();
}

} //namespace gui
