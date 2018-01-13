
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

#include "gui/window.h"
#include "gui/widget.h"
#include "gui/style.h"

#include "imageManager/imageManager.h"

namespace gui{

Widget::Widget()
  : _x(0),
    _y(0),
    _width(0),
    _height(0),
    _flags(0),
    _parent(nullptr),
    states(),
    _embeddedData()
{
}

Widget::Widget(const Widget& other)
  : _x(other._x),
    _y(other._y),
    _width(other._width),
    _height(other._height),
    _flags(other._flags),
    _parent(other._parent),
    states(other.states),
    _embeddedData(other._embeddedData)
{
}

Widget& Widget::operator=(const Widget& right)
{
    _x = right._x;
    _y = right._y;
    _width = right._width;
    _height = right._height;
    _flags = right._flags;
    _parent = right._parent;
    states = right.states;
    _embeddedData = right._embeddedData;

    return *this;
}

void Widget::setPosition(int x, int y)
{
    _x = x;
    _y = y;
}

void Widget::setParent(Window* p)
{
    _parent = p;
}

void Widget::setSize(int w, int h)
{
    _width = w;
    _height = h;
}

void Widget::addFlags(uint16_t newFlags)
{
    _flags |= newFlags;
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

int Widget::left()
{
    if (_parent == nullptr || _parent->screen() == nullptr)
    {
        tools::debug::error("A widget has been badly started", "gui", __FILENAME__, __LINE__);
        return 0;
    }

    int x = _x + _parent->left();

    if ((_flags & Fullscreen) == Fullscreen)
        return 0;
    if ((_flags & XPositionRelativeToCenter) == XPositionRelativeToCenter)
        x = _parent->width()/2 + x;
    if ((_flags & XPositionRelativeToRight) == XPositionRelativeToRight)
        x = _parent->width() - x;
    if ((_flags & XPositionRelativeToScreenSize) == XPositionRelativeToScreenSize)
        x *= _parent->width() / 100.0;

    if ((_flags & OriginXCenter) == OriginXCenter)
        return x - width()/2;
    if ((_flags & OriginRight) == OriginRight)
        return x - width();

    return x;
}

int Widget::top()
{
    if (_parent == nullptr || _parent->screen() == nullptr)
    {
        tools::debug::error("A widget has been badly started", "gui", __FILENAME__, __LINE__);
        return 0;
    }

    int y = _y + _parent->top();

    if ((_flags & Fullscreen) == Fullscreen)
        return 0;
    if ((_flags & YPositionRelativeToCenter) == YPositionRelativeToCenter)
        y = _parent->height()/2 + y;
    if ((_flags & YPositionRelativeToBottom) == YPositionRelativeToBottom)
        y = _parent->height() - y;
    if ((_flags & YPositionRelativeToScreenSize) == YPositionRelativeToScreenSize)
        y *= _parent->height() / 100.0;

    if ((_flags & OriginYCenter) == OriginYCenter)
        return y - height()/2;
    if ((_flags & OriginBottom) == OriginBottom)
        return y - height();

    return y;
}

int Widget::getXCenter()
{
    return left() + width()/2;
}

int Widget::getYCenter()
{
    return top() + height()/2;
}

int Widget::width()
{
    if (_parent == nullptr || _parent->screen() == nullptr)
    {
        tools::debug::error("A widget has been badly started", "gui", __FILENAME__, __LINE__);
        return 0;
    }

    if ((_flags & Fullscreen) == Fullscreen)
        return _parent->width();

    return _width;
}

int Widget::height()
{
    if (_parent == nullptr || _parent->screen() == nullptr)
    {
        tools::debug::error("A widget has been badly started", "gui", __FILENAME__, __LINE__);
        return 0;
    }

    if ((_flags & Fullscreen) == Fullscreen)
        return _parent->height();

    return _height;
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

    s->second.text.setSize(max(0, _width - 12), _height);

    if ((_flags & OriginXCenter) == OriginXCenter)
        s->second.text.addFlags(textManager::HAlignCenter | textManager::OriginXCenter);
    if ((_flags & OriginYCenter) == OriginYCenter)
        s->second.text.addFlags(textManager::OriginYCenter);

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
        _width = max(_width, state.second.text.width() + 12);
        _height = max(_height, state.second.text.height() + 6);
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
        imageManager::display(app, "gui", state->second.background, left(), top());

    if (state->second.backgroundShader != nullptr)
        state->second.backgroundShader(app, left(), top(), width(), height());
    else if (!state->second.bShader.empty())
        gui::style::displayShader(app, state->second.bShader, left(), top(), width(), height());

    state->second.text.displayFullText(app, getXCenter(), getYCenter());

    if (state->second.foregroundShader != nullptr)
        state->second.foregroundShader(app, left(), top(), width(), height());
    else if (!state->second.fShader.empty())
        gui::style::displayShader(app, state->second.fShader, left(), top(), width(), height());
}

} //namespace gui
