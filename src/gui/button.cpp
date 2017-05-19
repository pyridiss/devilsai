
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

void Button::setAutoRelease(bool a)
{
    autoRelease = a;
}

bool Button::mouseHovering(RenderWindow& app)
{
    if (state == Disabled) return false;

    if (Mouse::getPosition(app).x >= getXTopLeft() && Mouse::getPosition(app).x <= getXTopLeft() + width &&
        Mouse::getPosition(app).y >= getYTopLeft() && Mouse::getPosition(app).y <= getYTopLeft() + height)
    {
        if (state == Normal) state = Hover;
        return true;
    }

    if (autoRelease) state = Normal;
    else if (state == Hover) state = Normal;

    return false;
}

bool Button::activated(RenderWindow& app, Event::EventType event)
{
    if (state == Disabled) return false;

    if (mouseHovering(app))
    {
        if (event == Event::MouseButtonPressed)
        {
            musicManager::playSound("Click");

            if (autoRelease)
            {
                state = Active;
                return false; //We wait for MouseButtonReleased
            }
            else
            {
                if (state == Active) state = Normal;
                else state = Active;
                return true;
            }
        }
        if (event == Event::MouseButtonReleased)
        {
            if (state == Active)
            {
                if (autoRelease)
                {
                    state = Normal;
                    return true;
                }
                else //MouseButtonReleased must not be used
                {
                    if (state == Active) return true;
                    else return false;
                }
            }
            else return false;
        }

        if (!autoRelease && state == Active)
            return true;

        return false;
    }

    if (autoRelease)
        state = Normal;

    if (!autoRelease && state == Active)
        return true;

    return false;
}

void Button::display(RenderWindow& app)
{
    //TODO: Button shoud add images by itself in its own container
    switch (state)
    {
        case Normal:
            if (!normal.background.empty())
                imageManager::display(app, "misc", normal.background, getXTopLeft(), getYTopLeft()); //Depends on Bibliotheque
            app.draw(normal.text);
            if (normal.shader != NULL)
                normal.shader(app, getXTopLeft(), getYTopLeft(), width, height);
            break;
        case Active:
            if (!active.background.empty())
                imageManager::display(app, "misc", active.background, getXTopLeft(), getYTopLeft()); //Depends on Bibliotheque
            app.draw(active.text);
            if (active.shader != NULL)
                active.shader(app, getXTopLeft(), getYTopLeft(), width, height);
            break;
        case Hover:
            if (!hover.background.empty())
                imageManager::display(app, "misc", hover.background, getXTopLeft(), getYTopLeft()); //Depends on Bibliotheque
            app.draw(hover.text);
            if (hover.shader != NULL)
                hover.shader(app, getXTopLeft(), getYTopLeft(), width, height);
            break;
        case Disabled:
            if (!disabled.background.empty())
                imageManager::display(app, "misc", disabled.background, getXTopLeft(), getYTopLeft()); //Depends on Bibliotheque
            app.draw(disabled.text);
            if (disabled.shader != NULL)
                disabled.shader(app, getXTopLeft(), getYTopLeft(), width, height);
            break;
    }
}

} //namespace gui
