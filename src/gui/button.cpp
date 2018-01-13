
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
    addState("normal");
    addState("active");
    addState("hover");
    addState("disabled");

    states.find("normal")->second.text.setDefaultProperties(gui::style::buttonTextFont(), gui::style::buttonTextSize(), gui::style::normalButtonTextColor());
    states.find("active")->second.text.setDefaultProperties(gui::style::buttonTextFont(), gui::style::buttonTextSize(), gui::style::activeButtonTextColor());
    states.find("hover")->second.text.setDefaultProperties(gui::style::buttonTextFont(), gui::style::buttonTextSize(), gui::style::hoverButtonTextColor());
    states.find("disabled")->second.text.setDefaultProperties(gui::style::buttonTextFont(), gui::style::buttonTextSize(), gui::style::disabledButtonTextColor());

    for (auto& s : states)
        s.second.text.addFlags(textManager::HAlignCenter | textManager::OriginXCenter | textManager::OriginYCenter | textManager::FixedHeight | textManager::VAlignCenter);
}

void Button::setAutoRelease(bool a)
{
    autoRelease = a;
}

bool Button::mouseHovering(RenderWindow& app)
{
    if (currentState == "disabled") return false;

    if ((int)Mouse::getPosition(app).x >= getXTopLeft() && (int)Mouse::getPosition(app).x <= getXTopLeft() + width &&
        (int)Mouse::getPosition(app).y >= getYTopLeft() && (int)Mouse::getPosition(app).y <= getYTopLeft() + height)
    {
        if (currentState == "normal") currentState = "hover";
        return true;
    }

    if (autoRelease) currentState = "normal";
    else if (currentState == "hover") currentState = "normal";

    return false;
}

bool Button::activated(RenderWindow& app, Event event)
{
    if (currentState == "disabled") return false;

    if (mouseHovering(app))
    {
        if (event.type == Event::MouseButtonPressed)
        {
            musicManager::playSound("Click");

            if (autoRelease)
            {
                currentState = "active";
                return false; //We wait for MouseButtonReleased
            }
            else
            {
                if (currentState == "active") currentState = "normal";
                else currentState = "active";
                return true;
            }
        }
        if (event.type == Event::MouseButtonReleased)
        {
            if (currentState == "active")
            {
                if (autoRelease)
                {
                    currentState = "normal";
                    return true;
                }
                else //MouseButtonReleased must not be used
                {
                    if (currentState == "active") return true;
                    else return false;
                }
            }
            else return false;
        }

        if (!autoRelease && currentState == "active")
            return true;

        return false;
    }

    if (autoRelease)
        currentState = "normal";

    if (!autoRelease && currentState == "active")
        return true;

    return false;
}

void Button::setValue(const string& d)
{
    addEmbeddedData("value", d);
}

string Button::value()
{
    return embeddedData("value");
}

} //namespace gui
