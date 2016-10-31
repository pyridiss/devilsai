
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
#include "tools/style.h"

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

void Button::setAutoRelease(bool a)
{
    autoRelease = a;
}

void Button::setDisabled(bool d)
{
    if (d) state = Disabled;
    else if (state == Disabled) state = Normal;
}

void Button::setActive(bool a)
{
    if (a) state = Active;
    else if (state == Active) state = Normal;
}

int Button::getXTopLeft()
{
    if (xTopLeft == 0)
        return xCenter - width/2;

    return xTopLeft;
}

int Button::getYTopLeft()
{
    if (yTopLeft == 0)
        return yCenter - height/2;

    return yTopLeft;
}

int Button::getXCenter()
{
    if (xCenter == 0)
        return xTopLeft + width/2;

    return xCenter;
}

int Button::getYCenter()
{
    if (yCenter == 0)
        return yTopLeft + height/2;

    return yCenter;
}

void Button::setAllText(String32 & t)
{
    setNormalText(t);
    setActiveText(t);
    setHoverText(t);
    setDisabledText(t);
}

void Button::setNormalText(String32& t)
{
    normal.text.setString(t);

    //If no font properties set, use default (needed to compute position)
    if (normal.text.getFont() == NULL)
        setTextFont(tools::style::buttonTextFont(), tools::style::buttonTextSize());

    setNormalTextColor(tools::style::normalButtonTextColor());

    FloatRect rect = normal.text.getGlobalBounds();
    normal.text.setPosition((int)(getXCenter() - rect.width/2 - 1), (int)(getYCenter() - rect.height/2 - 3));

    if (width == 0 || height == 0)
    {
        width = rect.width + 12;
        height = rect.height + 6;
    }
}

void Button::setActiveText(String32& t)
{
    active.text.setString(t);

    //If no font properties set, use default (needed to compute position)
    if (active.text.getFont() == NULL)
        setTextFont(tools::style::buttonTextFont(), tools::style::buttonTextSize());

    setActiveTextColor(tools::style::activeButtonTextColor());

    FloatRect rect = active.text.getGlobalBounds();
    active.text.setPosition((int)(getXCenter() - rect.width/2 - 1), (int)(getYCenter() - rect.height/2 - 3));

    if (width == 0 || height == 0)
    {
        width = rect.width + 12;
        height = rect.height + 6;
    }
}

void Button::setHoverText(String32& t)
{
    hover.text.setString(t);

    //If no font properties set, use default (needed to compute position)
    if (hover.text.getFont() == NULL)
        setTextFont(tools::style::buttonTextFont(), tools::style::buttonTextSize());

    setHoverTextColor(tools::style::hoverButtonTextColor());

    FloatRect rect = hover.text.getGlobalBounds();
    hover.text.setPosition((int)(getXCenter() - rect.width/2 - 1), (int)(getYCenter() - rect.height/2 - 3));

    if (width == 0 || height == 0)
    {
        width = rect.width + 12;
        height = rect.height + 6;
    }
}

void Button::setDisabledText(String32& t)
{
    disabled.text.setString(t);

    //If no font properties set, use default (needed to compute position)
    if (disabled.text.getFont() == NULL)
        setTextFont(tools::style::buttonTextFont(), tools::style::buttonTextSize());

    setDisabledTextColor(tools::style::disabledButtonTextColor());

    FloatRect rect = disabled.text.getGlobalBounds();
    disabled.text.setPosition((int)(getXCenter() - rect.width/2 - 1), (int)(getYCenter() - rect.height/2 - 3));

    if (width == 0 || height == 0)
    {
        width = rect.width + 12;
        height = rect.height + 6;
    }
}

void Button::setTextFont(const Font& f, float s)
{
    normal.text.setFont(f);
    normal.text.setCharacterSize(s);
    active.text.setFont(f);
    active.text.setCharacterSize(s);
    hover.text.setFont(f);
    hover.text.setCharacterSize(s);
    disabled.text.setFont(f);
    disabled.text.setCharacterSize(s);
}

void Button::setNormalTextColor(Color c)
{
    normal.text.setColor(c);
}

void Button::setActiveTextColor(Color c)
{
    active.text.setColor(c);
}

void Button::setHoverTextColor(Color c)
{
    hover.text.setColor(c);
}

void Button::setDisabledTextColor(Color c)
{
    disabled.text.setColor(c);
}

void Button::setAllBackground(string b)
{
    setNormalBackground(b);
    setActiveBackground(b);
    setHoverBackground(b);
    setDisabledBackground(b);
}

void Button::addOffsetToNormalText(int x, int y)
{
    normal.text.setPosition(normal.text.getPosition().x + x, normal.text.getPosition().y + y);
}

void Button::addOffsetToActiveText(int x, int y)
{
    active.text.setPosition(active.text.getPosition().x + x, active.text.getPosition().y + y);
}

void Button::addOffsetToHoverText(int x, int y)
{
    hover.text.setPosition(hover.text.getPosition().x + x, hover.text.getPosition().y + y);
}

void Button::addOffsetToDisabledText(int x, int y)
{
    disabled.text.setPosition(disabled.text.getPosition().x + x, disabled.text.getPosition().y + y);
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

void Button::setNormalShader(void (*s)(RenderWindow&, int, int, int, int))
{
    normal.shader = s;
}

void Button::setActiveShader(void (*s)(RenderWindow&, int, int, int, int))
{
    active.shader = s;
}

void Button::setHoverShader(void (*s)(RenderWindow&, int, int, int, int))
{
    hover.shader = s;
}

void Button::setDisabledShader(void (*s)(RenderWindow&, int, int, int, int))
{
    disabled.shader = s;
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
            playSound("Click");

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
    switch (state)
    {
        case Normal:
            if (!normal.background.empty())
                Disp_ImageDecoration(normal.background, getXTopLeft(), getYTopLeft()); //Depends on Bibliotheque
            app.draw(normal.text);
            if (normal.shader != NULL)
                normal.shader(app, getXTopLeft(), getYTopLeft(), width, height);
            break;
        case Active:
            if (!active.background.empty())
                Disp_ImageDecoration(active.background, getXTopLeft(), getYTopLeft()); //Depends on Bibliotheque
            app.draw(active.text);
            if (active.shader != NULL)
                active.shader(app, getXTopLeft(), getYTopLeft(), width, height);
            break;
        case Hover:
            if (!hover.background.empty())
                Disp_ImageDecoration(hover.background, getXTopLeft(), getYTopLeft()); //Depends on Bibliotheque
            app.draw(hover.text);
            if (hover.shader != NULL)
                hover.shader(app, getXTopLeft(), getYTopLeft(), width, height);
            break;
        case Disabled:
            if (!disabled.background.empty())
                Disp_ImageDecoration(disabled.background, getXTopLeft(), getYTopLeft()); //Depends on Bibliotheque
            app.draw(disabled.text);
            if (disabled.shader != NULL)
                disabled.shader(app, getXTopLeft(), getYTopLeft(), width, height);
            break;
    }
}

}
