
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

#include "tools/math.h"

#include "textManager/plainText.h"
#include "textManager/textManager.h"

#include "gui/inputField.h"
#include "gui/style.h"

namespace gui{

InputField::InputField()
  : input()
{
    _text.setDefaultProperties(parameter<string>(tools::hash("input-text-font")),
                               parameter<int>(tools::hash("input-text-size")),
                               parameter<Color>(tools::hash("input-text-color")));

    _textFlags = textManager::HAlignCenter | textManager::FixedHeight | textManager::VAlignCenter;
}

bool InputField::mouseHovering()
{
    if (mousePosition().x >= left() && mousePosition().x <= left() + width() &&
        mousePosition().y >= top() && mousePosition().y <= top() + height())
    {
        return true;
    }

    return false;
}

bool InputField::activated(Event event)
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

        setText(input);
        addEmbeddedData<string>("value", textManager::PlainText(input).toStdString());
    }

    return false;
}

void InputField::setValue(optionType v)
{
    if (v.is<textManager::PlainText>())
    {
        input = v.get<textManager::PlainText>().aggregatedText();
        addEmbeddedData<string>("value", v.get<textManager::PlainText>().toStdString());
    }
    else if (v.is<string>())
    {
        input = textManager::PlainText(v.get<string>()).aggregatedText();
        addEmbeddedData<string>("value", v.get<string>());
    }

    setText(input);
}

} //namespace gui
