
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

#include "tools/button.h"

#include "Bibliotheque/Bibliotheque.h"

namespace tools{

void Button::setTopLeftCoordinates(int x, int y)
{
    xTopLeft = x;
    yTopLeft = y;
    xCenter = 0;
    yCenter = 0;
}

void Button::setCenterCoordinates(int x, int y)
{
    xCenter = x;
    yCenter = y;
    xTopLeft = 0;
    yTopLeft = 0;
}

void Button::setSize(int w, int h)
{
    width = w;
    height = h;
}

void Button::setNormalText(Text t)
{
    normal.text = t;
}

void Button::setActiveText(Text t)
{
    active.text = t;
}

void Button::setHoverText(Text t)
{
    hover.text = t;
}

void Button::setDisabledText(Text t)
{
    disabled.text = t;
}


void Button::setNormalBackground(string b)
{
    normal.background = b;
}

void Button::setActiveBackground(string b)
{
    active.background = b;
}

void Button::setHoverBackground(string b)
{
    hover.background = b;
}

void Button::setDisabledBackground(string b)
{
    disabled.background = b;
}

void Button::applyDefaultColors()
{
    normal.text.setColor  (Color(200, 170,   0, 255));
    active.text.setColor  (Color(200, 100,   0, 255));
    hover.text.setColor   (Color(200, 170,   0, 255));
    disabled.text.setColor(Color(200, 170,   0, 255));
}

void Button::applyDefaultFont()
{
    normal.text.setCharacterSize(10.);
    active.text.setCharacterSize(10.);
    hover.text.setCharacterSize(10.);
    disabled.text.setCharacterSize(10.);
    normal.text.setFont(getDefaultFont()); //Depends on Bibliotheque
    active.text.setFont(getDefaultFont()); //Depends on Bibliotheque
    hover.text.setFont(getDefaultFont()); //Depends on Bibliotheque
    disabled.text.setFont(getDefaultFont()); //Depends on Bibliotheque
}

bool Button::mouseHovering(RenderWindow& app)
{
    //TODO: make it work when xCenter and yCenter are set
    if (Mouse::getPosition(app).x >= xTopLeft && Mouse::getPosition(app).x <= xTopLeft + width &&
        Mouse::getPosition(app).y >= xTopLeft && Mouse::getPosition(app).y <= yTopLeft + height)
        return true;
    return false;
}

bool Button::activated(RenderWindow& app, Event::EventType event)
{
    if (mouseHovering(app))
    {
        if (event == Event::MouseButtonPressed || event == Event::MouseMoved)
        {
            state = Active;
            if (event == Event::MouseButtonPressed) playSound("Click");
            return false;
        }
        if (event == Event::MouseButtonReleased)
        {
            if (state == Active)
            {
                state = Normal;
                return true;
            }
            else return false;
        }
    }
    state = Normal;
    return false;
}

void Button::display(RenderWindow app)
{
    switch (state)
    {
        case Normal:
            if (!normal.background.empty())
                Disp_ImageDecoration(normal.background, xTopLeft, yTopLeft); //Depends on Bibliotheque
                //TODO: correctly display when xCenter and yCenter are set
            app.draw(normal.text);
            break;
        case Active:
            if (!active.background.empty())
                Disp_ImageDecoration(active.background, xTopLeft, yTopLeft); //Depends on Bibliotheque
                //TODO: correctly display when xCenter and yCenter are set
            app.draw(active.text);
            //TODO: auto highlight background when the same as Normal background
            break;
        case Hover:
            //TODO
            break;
        case Disabled:
            //TODO
            break;
    }
}

}
