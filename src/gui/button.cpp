
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
#include "imageManager/imageManager.h"
#include "musicManager/musicManager.h"

namespace gui{

Button::Button()
{
    minimalistWidget w;

    states.insert(map<string,minimalistWidget>::value_type("normal", w));
    states.insert(map<string,minimalistWidget>::value_type("active", w));
    states.insert(map<string,minimalistWidget>::value_type("hover", w));
    states.insert(map<string,minimalistWidget>::value_type("disabled", w));

    setTextFont(gui::style::buttonTextFont(), gui::style::buttonTextSize());

    setTextColor("normal",   gui::style::normalButtonTextColor());
    setTextColor("active",   gui::style::activeButtonTextColor());
    setTextColor("hover",    gui::style::hoverButtonTextColor());
    setTextColor("disabled", gui::style::disabledButtonTextColor());

}

void Button::setAutoRelease(bool a)
{
    autoRelease = a;
}

bool Button::mouseHovering(RenderWindow& app)
{
    if (currentState == "disabled") return false;

    if (Mouse::getPosition(app).x >= getXTopLeft() && Mouse::getPosition(app).x <= getXTopLeft() + width &&
        Mouse::getPosition(app).y >= getYTopLeft() && Mouse::getPosition(app).y <= getYTopLeft() + height)
    {
        if (currentState == "normal") currentState = "hover";
        return true;
    }

    if (autoRelease) currentState = "normal";
    else if (currentState == "hover") currentState = "normal";

    return false;
}

bool Button::activated(RenderWindow& app, Event::EventType event)
{
    if (currentState == "disabled") return false;

    if (mouseHovering(app))
    {
        if (event == Event::MouseButtonPressed)
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
        if (event == Event::MouseButtonReleased)
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

void Button::display(RenderWindow& app)
{
    const auto& state = states.find(currentState);

    if (!state->second.background.empty())
        imageManager::display(app, "misc", state->second.background, getXTopLeft(), getYTopLeft());

    FloatRect rect = state->second.text.getGlobalBounds();
    state->second.text.setPosition((int)(getXCenter() - rect.width/2 - 1), (int)(getYCenter() - rect.height/2 - 3));

    app.draw(state->second.text);

    if (state->second.shader != NULL)
        state->second.shader(app, getXTopLeft(), getYTopLeft(), width, height);
}

} //namespace gui
