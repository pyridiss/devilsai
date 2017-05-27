
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

#include "tools/debug.h"
#include "gui/widget.h"
#include "gui/style.h"

namespace gui{

void Widget::setTopLeftCoordinates(int x, int y)
{
    xTopLeft = x;
    yTopLeft = y;
    xCenter = -1;
    yCenter = -1;
}

void Widget::setCenterCoordinates(int x, int y)
{
    xCenter = x;
    yCenter = y;
    xTopLeft = -1;
    yTopLeft = -1;
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

void Widget::addState(string state)
{
    states.emplace(state, minimalistWidget());
}

void Widget::setCurrentState(string state)
{
    if (states.find(state) == states.end())
    {
        tools::debug::warning("gui", "An unknown state (" + state + ") has been asked to a widget; aborting.");
        return;
    }

    currentState = state;
}

int Widget::getXTopLeft()
{
    if (xTopLeft == -1)
        return xOrigin + xCenter - width/2;

    return xOrigin + xTopLeft;
}

int Widget::getYTopLeft()
{
    if (yTopLeft == -1)
        return yOrigin + yCenter - height/2;

    return yOrigin + yTopLeft;
}

int Widget::getXCenter()
{
    if (xCenter == -1)
        return xOrigin + xTopLeft + width/2;

    return xOrigin + xCenter;
}

int Widget::getYCenter()
{
    if (yCenter == -1)
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

    updateTextPosition();
    updateSize();
}

void Widget::setTextFont(const Font& f, float s)
{
    for (auto& state : states)
    {
        state.second.text.setFont(f);
        state.second.text.setCharacterSize(s);
    }

    updateTextPosition();
    updateSize();
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

void Widget::setBackgroundShader(string state, void (*s)(RenderWindow&, int, int, int, int))
{
    states.find(state)->second.backgroundShader = s;
}

void Widget::setForegroundShader(string state, void (*s)(RenderWindow&, int, int, int, int))
{
    states.find(state)->second.foregroundShader = s;
}

void Widget::setBackgroundShader(string state, string s)
{
    states.find(state)->second.bShader = s;
}

void Widget::setForegroundShader(string state, string s)
{
    states.find(state)->second.fShader = s;
}

void Widget::updateTextPosition()
{
    for (auto& state : states)
    {
        FloatRect rect = state.second.text.getGlobalBounds();
        state.second.text.setPosition((int)(getXCenter() - rect.width/2 - 1), (int)(getYCenter() - rect.height/2 - 3));
    }
}

void Widget::updateSize()
{
    for (auto& state : states)
    {
        FloatRect rect = state.second.text.getGlobalBounds();

        width = max(width, (int)rect.width + 12);
        height = max(height, (int)rect.height + 6);
    }
}

void Widget::display(RenderWindow& app)
{
    const auto& state = states.find(currentState);

    if (!state->second.background.empty())
        imageManager::display(app, "misc", state->second.background, getXTopLeft(), getYTopLeft());

    if (state->second.backgroundShader != nullptr)
        state->second.backgroundShader(app, getXTopLeft(), getYTopLeft(), width, height);
    else if (!state->second.bShader.empty())
        gui::style::displayShader(app, state->second.bShader, getXTopLeft(), getYTopLeft(), width, height);

    app.draw(state->second.text);

    if (state->second.foregroundShader != nullptr)
        state->second.foregroundShader(app, getXTopLeft(), getYTopLeft(), width, height);
    else if (!state->second.fShader.empty())
        gui::style::displayShader(app, state->second.fShader, getXTopLeft(), getYTopLeft(), width, height);
}

} //namespace gui
