
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

#include "imageManager/imageManager.h"

namespace gui{

Widget::Widget()
  : _x(0),
    _y(0),
    _flags(0),
    states(),
    _embeddedData()
{
}

void Widget::setTopLeftCoordinates(int x, int y)
{
    _x = x;
    _y = y;
    _flags = TopLeftCoordinates;
}

void Widget::setCenterCoordinates(int x, int y)
{
    _x = x;
    _y = y;
    _flags = CenterCoordinates;
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
        tools::debug::warning("gui", "An unknown state (" + state + ") has been asked to a widget; aborting.", __FILENAME__, __LINE__);
        return;
    }

    currentState = state;
}

int Widget::getXTopLeft()
{
    if ((_flags & CenterCoordinates) == CenterCoordinates)
        return xOrigin + _x - width/2;

    return xOrigin + _x;
}

int Widget::getYTopLeft()
{
    if ((_flags & CenterCoordinates) == CenterCoordinates)
        return yOrigin + _y - height/2;

    return yOrigin + _y;
}

int Widget::getXCenter()
{
    if ((_flags & CenterCoordinates) == CenterCoordinates)
        return xOrigin + _x;

    return xOrigin + _x + width/2;
}

int Widget::getYCenter()
{
    if ((_flags & CenterCoordinates) == CenterCoordinates)
        return yOrigin + _y;

    return yOrigin + _y + height/2;
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

    s->second.text.setSize(max(0, width - 12), height);
    if ((_flags & CenterCoordinates) == CenterCoordinates)
        s->second.text.addFlags(textManager::HAlignCenter | textManager::OriginXCenter | textManager::OriginYCenter);
    s->second.text.setSource(&t);

    if ((_flags & AdjustSizeToText) == AdjustSizeToText)
        updateSize();
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

void Widget::updateSize()
{
    for (auto& state : states)
    {
        width = max(width, state.second.text.width() + 12);
        height = max(height, state.second.text.height() + 6);
    }
}

void Widget::addEmbeddedData(string name, string value)
{
    _embeddedData.emplace(std::move(name), std::move(value));
}

string Widget::embeddedData(string name)
{
    if (_embeddedData.find(name) != _embeddedData.end())
        return _embeddedData.find(name)->second;
    return string();
}

bool Widget::needsFocus()
{
    return _needsFocus;
}

void Widget::display(RenderWindow& app)
{
    const auto& state = states.find(currentState);

    if (!state->second.background.empty())
        imageManager::display(app, "gui", state->second.background, getXTopLeft(), getYTopLeft());

    if (state->second.backgroundShader != nullptr)
        state->second.backgroundShader(app, getXTopLeft(), getYTopLeft(), width, height);
    else if (!state->second.bShader.empty())
        gui::style::displayShader(app, state->second.bShader, getXTopLeft(), getYTopLeft(), width, height);

    state->second.text.displayFullText(app, getXCenter(), getYCenter());

    if (state->second.foregroundShader != nullptr)
        state->second.foregroundShader(app, getXTopLeft(), getYTopLeft(), width, height);
    else if (!state->second.fShader.empty())
        gui::style::displayShader(app, state->second.fShader, getXTopLeft(), getYTopLeft(), width, height);
}

} //namespace gui
