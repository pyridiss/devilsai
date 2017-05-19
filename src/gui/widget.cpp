
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2017  Quentin Henriet

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

#include "gui/widget.h"
#include "gui/style.h"

namespace gui{

void Widget::setTopLeftCoordinates(int x, int y)
{
    xTopLeft = x;
    yTopLeft = y;
    xCenter = 0;
    yCenter = 0;
}

void Widget::setCenterCoordinates(int x, int y)
{
    xCenter = x;
    yCenter = y;
    xTopLeft = 0;
    yTopLeft = 0;
}

void Widget::setSize(int w, int h)
{
    width = w;
    height = h;
}

void Widget::setDisabled(bool d)
{
    if (d) state = Disabled;
    else if (state == Disabled) state = Normal;
}

void Widget::setActive(bool a)
{
    if (a) state = Active;
    else if (state == Active) state = Normal;
}

int Widget::getXTopLeft()
{
    if (xTopLeft == 0)
        return xCenter - width/2;

    return xTopLeft;
}

int Widget::getYTopLeft()
{
    if (yTopLeft == 0)
        return yCenter - height/2;

    return yTopLeft;
}

int Widget::getXCenter()
{
    if (xCenter == 0)
        return xTopLeft + width/2;

    return xCenter;
}

int Widget::getYCenter()
{
    if (yCenter == 0)
        return yTopLeft + height/2;

    return yCenter;
}

void Widget::setAllText(String32 & t)
{
    setNormalText(t);
    setActiveText(t);
    setHoverText(t);
    setDisabledText(t);
}

void Widget::setNormalText(String32& t)
{
    normal.text.setString(t);

    //If no font properties set, use default (needed to compute position)
    if (normal.text.getFont() == NULL)
        setTextFont(gui::style::buttonTextFont(), gui::style::buttonTextSize());

    setNormalTextColor(gui::style::normalButtonTextColor());

    FloatRect rect = normal.text.getGlobalBounds();
    normal.text.setPosition((int)(getXCenter() - rect.width/2 - 1), (int)(getYCenter() - rect.height/2 - 3));

    if (width == 0 || height == 0)
    {
        width = rect.width + 12;
        height = rect.height + 6;
    }
}

void Widget::setActiveText(String32& t)
{
    active.text.setString(t);

    //If no font properties set, use default (needed to compute position)
    if (active.text.getFont() == NULL)
        setTextFont(gui::style::buttonTextFont(), gui::style::buttonTextSize());

    setActiveTextColor(gui::style::activeButtonTextColor());

    FloatRect rect = active.text.getGlobalBounds();
    active.text.setPosition((int)(getXCenter() - rect.width/2 - 1), (int)(getYCenter() - rect.height/2 - 3));

    if (width == 0 || height == 0)
    {
        width = rect.width + 12;
        height = rect.height + 6;
    }
}

void Widget::setHoverText(String32& t)
{
    hover.text.setString(t);

    //If no font properties set, use default (needed to compute position)
    if (hover.text.getFont() == NULL)
        setTextFont(gui::style::buttonTextFont(), gui::style::buttonTextSize());

    setHoverTextColor(gui::style::hoverButtonTextColor());

    FloatRect rect = hover.text.getGlobalBounds();
    hover.text.setPosition((int)(getXCenter() - rect.width/2 - 1), (int)(getYCenter() - rect.height/2 - 3));

    if (width == 0 || height == 0)
    {
        width = rect.width + 12;
        height = rect.height + 6;
    }
}

void Widget::setDisabledText(String32& t)
{
    disabled.text.setString(t);

    //If no font properties set, use default (needed to compute position)
    if (disabled.text.getFont() == NULL)
        setTextFont(gui::style::buttonTextFont(), gui::style::buttonTextSize());

    setDisabledTextColor(gui::style::disabledButtonTextColor());

    FloatRect rect = disabled.text.getGlobalBounds();
    disabled.text.setPosition((int)(getXCenter() - rect.width/2 - 1), (int)(getYCenter() - rect.height/2 - 3));

    if (width == 0 || height == 0)
    {
        width = rect.width + 12;
        height = rect.height + 6;
    }
}

void Widget::setTextFont(const Font& f, float s)
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

void Widget::setNormalTextColor(Color c)
{
    normal.text.setFillColor(c);
}

void Widget::setActiveTextColor(Color c)
{
    active.text.setFillColor(c);
}

void Widget::setHoverTextColor(Color c)
{
    hover.text.setFillColor(c);
}

void Widget::setDisabledTextColor(Color c)
{
    disabled.text.setFillColor(c);
}

void Widget::setAllBackground(string b)
{
    setNormalBackground(b);
    setActiveBackground(b);
    setHoverBackground(b);
    setDisabledBackground(b);
}

void Widget::addOffsetToNormalText(int x, int y)
{
    normal.text.setPosition(normal.text.getPosition().x + x, normal.text.getPosition().y + y);
}

void Widget::addOffsetToActiveText(int x, int y)
{
    active.text.setPosition(active.text.getPosition().x + x, active.text.getPosition().y + y);
}

void Widget::addOffsetToHoverText(int x, int y)
{
    hover.text.setPosition(hover.text.getPosition().x + x, hover.text.getPosition().y + y);
}

void Widget::addOffsetToDisabledText(int x, int y)
{
    disabled.text.setPosition(disabled.text.getPosition().x + x, disabled.text.getPosition().y + y);
}

void Widget::setNormalBackground(string b)
{
    normal.background = b;
}

void Widget::setActiveBackground(string b)
{
    active.background = b;
}

void Widget::setHoverBackground(string b)
{
    hover.background = b;
}

void Widget::setDisabledBackground(string b)
{
    disabled.background = b;
}

void Widget::setNormalShader(void (*s)(RenderWindow&, int, int, int, int))
{
    normal.shader = s;
}

void Widget::setActiveShader(void (*s)(RenderWindow&, int, int, int, int))
{
    active.shader = s;
}

void Widget::setHoverShader(void (*s)(RenderWindow&, int, int, int, int))
{
    hover.shader = s;
}

void Widget::setDisabledShader(void (*s)(RenderWindow&, int, int, int, int))
{
    disabled.shader = s;
}

} //namespace gui
