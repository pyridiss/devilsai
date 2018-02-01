
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

#include "gui/button.h"
#include "gui/style.h"
#include "musicManager/musicManager.h"

namespace gui{

Button::Button()
{
    _text.setDefaultProperties(gui::style::buttonTextFont(), gui::style::buttonTextSize(), gui::style::normalButtonTextColor());
    _textFlags = textManager::HAlignCenter | textManager::FixedHeight | textManager::VAlignCenter;
}

void Button::setAutoRelease(bool a)
{
    autoRelease = a;
}

bool Button::mouseHovering(RenderWindow& app)
{
    if ((_flags & Disabled) == Disabled)
        return false;

    if ((int)mousePosition().x >= left() && (int)mousePosition().x <= left() + width() &&
        (int)mousePosition().y >= top() && (int)mousePosition().y <= top() + height())
    {
        _flags |= MouseOver;
        return true;
    }
    else _flags &= ~MouseOver;

//     if (autoRelease) currentState = "normal";
//     else if (currentState == "hover") currentState = "normal";

    return false;
}

bool Button::activated(RenderWindow& app, Event event)
{
    if ((_flags & Disabled) == Disabled)
        return false;

    if (mouseHovering(app))
    {
        if (event.type == Event::MouseButtonPressed)
        {
            musicManager::playSound("Click");

            if (autoRelease)
            {
                _flags |= Activated;
                return false; //We wait for MouseButtonReleased
            }
            else
            {
                _flags ^= Activated;
                return true;
            }
        }
        if (event.type == Event::MouseButtonReleased)
        {
            if ((_flags & Activated) == Activated)
            {
                if (autoRelease)
                {
                    _flags &= ~Activated;
                    return true;
                }
                else //MouseButtonReleased must not be used
                {
                    return (_flags & Activated);
                }
            }
            else return false;
        }

        if (!autoRelease && (_flags & Activated) == Activated)
            return true;

        return false;
    }

//     if (autoRelease)
//         currentState = "normal";

    if (!autoRelease && (_flags & Activated) == Activated)
        return true;

    return false;
}

void Button::setValue(optionType v)
{
}

} //namespace gui
