
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

void Widget::setOriginCoordinates(int x, int y)
{
    xOrigin = x;
    yOrigin = y;
}

void Widget::setSize(int w, int h)
{
    width = w;
    height = h;
}

void Widget::setDisabled(bool d)
{
    if (d) currentState = "disabled";
    else if (currentState == "disabled") currentState = "normal";
}

void Widget::setActive(bool a)
{
    if (a) currentState = "active";
    else if (currentState == "active") currentState = "normal";
}

int Widget::getXTopLeft()
{
    if (xTopLeft == 0)
        return xOrigin + xCenter - width/2;

    return xOrigin + xTopLeft;
}

int Widget::getYTopLeft()
{
    if (yTopLeft == 0)
        return yOrigin + yCenter - height/2;

    return yOrigin + yTopLeft;
}

int Widget::getXCenter()
{
    if (xCenter == 0)
        return xOrigin + xTopLeft + width/2;

    return xOrigin + xCenter;
}

int Widget::getYCenter()
{
    if (yCenter == 0)
        return yOrigin + yTopLeft + height/2;

    return yOrigin + yCenter;
}

void Widget::setAllText(String32& t)
{
    for (auto& state : states)
    {
        setText(state.first, t);
    }
}

void Widget::setAllBackground(string b)
{
    for (auto& state : states)
    {
        setBackground(state.first, b);
    }
}

void Widget::setText(string state, String32& t)
{
    const auto& s = states.find(state);

    s->second.text.setString(t);

    FloatRect rect = s->second.text.getGlobalBounds();

    if (width == 0 || height == 0)
    {
        width = rect.width + 12;
        height = rect.height + 6;
    }
}

void Widget::setTextFont(const Font& f, float s)
{
    for (auto& state : states)
    {
        state.second.text.setFont(f);
        state.second.text.setCharacterSize(s);
    }
}

void Widget::setTextColor(string state, Color c)
{
    states.find(state)->second.text.setFillColor(c);
}

void Widget::addOffsetToText(string state, int x, int y)
{
    states.find(state)->second.text.setPosition(
        states.find(state)->second.text.getPosition().x + x,
        states.find(state)->second.text.getPosition().y + y
    );
}

void Widget::setBackground(string state, string b)
{
    states.find(state)->second.background = b;
}

void Widget::setShader(string state, void (*s)(RenderWindow&, int, int, int, int))
{
    states.find(state)->second.shader = s;
}

} //namespace gui
