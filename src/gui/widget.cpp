
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

#include <tinyxml2.h>

#include "tools/debug.h"

#include "gui/window.h"
#include "gui/widget.h"
#include "gui/style.h"

#include "imageManager/image.h"
#include "imageManager/imageManager.h"

using namespace tinyxml2;

namespace gui{

Widget::Widget()
  : _x(0),
    _y(0),
    _width(0),
    _height(0),
    _flags(0),
    _textFlags(0),
    _parent(nullptr),
    _text(),
    _background(),
    _backgroundShader(),
    _embeddedData()
{
}

Widget::Widget(const Widget& other)
  : _x(other._x),
    _y(other._y),
    _width(other._width),
    _height(other._height),
    _flags(other._flags),
    _textFlags(other._textFlags),
    _parent(other._parent),
    _text(other._text),
    _background(other._background),
    _backgroundShader(other._backgroundShader),
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
    _textFlags = right._textFlags;
    _parent = right._parent;
    _text =  right._text;
    _background = right._background;
    _backgroundShader = right._backgroundShader;
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

    if (_width != 0 && _height != 0)
        _flags &= ~AdjustSizeToText;

    if (_height != 0)
        _textFlags |= textManager::FixedHeight;
}

void Widget::addFlags(uint32_t newFlags)
{
    _flags |= newFlags;
}

void Widget::removeFlags(uint32_t newFlags)
{
    _flags &= ~newFlags;
}

void Widget::addTextFlags(uint16_t newFlags)
{
    _textFlags |= newFlags;
}

int Widget::left()
{
    if (_parent == nullptr)
    {
        tools::debug::error("A widget has been badly started", "gui", __FILENAME__, __LINE__);
        return 0;
    }

    int x = _x + _parent->left();

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
    if (_parent == nullptr)
    {
        tools::debug::error("A widget has been badly started", "gui", __FILENAME__, __LINE__);
        return 0;
    }

    int y = _y + _parent->top();

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
    if (_parent == nullptr)
    {
        tools::debug::error("A widget has been badly started", "gui", __FILENAME__, __LINE__);
        return 0;
    }

    int w = _width;

    if ((_flags & WidthRelativeToScreenSize) == WidthRelativeToScreenSize)
        w = _parent->width() * _width / 100.0;

    if ((_flags & WidthMeansFixedMargin) == WidthMeansFixedMargin)
        w = _parent->width() - _width;

    if ((_flags & VerticalScrollBar) == VerticalScrollBar)
        w -= 20;

    return max(0, w);
}

int Widget::height()
{
    if (_parent == nullptr)
    {
        tools::debug::error("A widget has been badly started", "gui", __FILENAME__, __LINE__);
        return 0;
    }

    int h = _height;

    if ((_flags & HeightRelativeToScreenSize) == HeightRelativeToScreenSize)
        h = _parent->height() * _height / 100.0;

    if ((_flags & HeightMeansFixedMargin) == HeightMeansFixedMargin)
        h = _parent->height() - _height;

    return max(0, h);
}

void Widget::setText(const textManager::PlainText& t)
{
    if ((_flags & AdjustSizeToText) == AdjustSizeToText)
        _textFlags &= ~textManager::FixedHeight;
    if ((_flags & VerticalScrollBar) == VerticalScrollBar)
        _textFlags &= ~textManager::FixedHeight;

    if ((_flags & AdjustSizeToText) == AdjustSizeToText)
    {
        _text.setSize(0, 0);
        setSize(0, 0);
    }
    else
        _text.setSize(width(), height());

    _text.addFlags(_textFlags);

    _text.create(t);

    if ((_flags & AdjustSizeToText) == AdjustSizeToText)
    {
        _width = max(_width, _text.width());
        _height = max(_height, _text.height());
    }
}

void Widget::setBackground(string b)
{
    _background = std::move(b);
}

void Widget::setBackgroundShader(string s)
{
    _backgroundShader = std::move(s);
}

template<typename T>
void Widget::addEmbeddedData(string name, const T& value)
{
    optionType o;
    o.set<T>(value);
    _embeddedData.erase(name);
    _embeddedData.emplace(std::move(name), std::move(o));
}

template<> void Widget::addEmbeddedData(string name, const optionType& value)
{
    _embeddedData.erase(name);
    _embeddedData.emplace(std::move(name), value);
}

//Explicit instantiations for the linker
template void Widget::addEmbeddedData(string name, const string& value);
template void Widget::addEmbeddedData(string name, const textManager::PlainText& value);
template void Widget::addEmbeddedData(string name, const Keyboard::Key& value);
template void Widget::addEmbeddedData(string name, const float& value);

template<typename T>
T& Widget::embeddedData(const string& name)
{
    if (_embeddedData.find(name) != _embeddedData.end())
        return _embeddedData.find(name)->second.get<T>();
    else
    {
        addEmbeddedData<T>(name, T());
        return _embeddedData.find(name)->second.get<T>();
    }
}

template<> optionType& Widget::embeddedData(const string& name)
{
    if (_embeddedData.find(name) != _embeddedData.end())
        return _embeddedData.find(name)->second;
    else
    {
        addEmbeddedData<optionType>(name, optionType());
        return _embeddedData.find(name)->second;
    }
}

//Explicit instantiations for the linker
template string& Widget::embeddedData(const string& name);
template textManager::PlainText& Widget::embeddedData(const string& name);
template Keyboard::Key& Widget::embeddedData(const string& name);
template float& Widget::embeddedData(const string& name);

bool Widget::needsFocus()
{
    return _needsFocus;
}

void Widget::displayBackground(RenderWindow& app)
{
    //Lambda function to display the background image which shaders according to flags.
    auto backgroundImage = [this, &app](int x, int y)
    {
        if ((_flags & Disabled) == Disabled)
            imageManager::display(app, "gui", _background, x, y, false, style::getContrastShader(0.25, 0.25, 0.25));
        else if ((_flags & MouseOver) == MouseOver)
            imageManager::display(app, "gui", _background, x, y, false, style::getContrastShader(1, 1, 1));
        else
            imageManager::display(app, "gui", _background, x, y);
    };

    if (!_background.empty())
    {
        if ((_flags & AdjustBackgroundToSize) == AdjustBackgroundToSize)
        {
            imageManager::Image* image = imageManager::getImage("gui", _background);
            View currentView = app.getView();

            View newView(FloatRect(0, 0, image->getSize().x, image->getSize().y));
            newView.setViewport(FloatRect((float)left()/(float)app.getSize().x, (float)top()/(float)app.getSize().y, width()/(float)app.getSize().x, height()/(float)app.getSize().y));

            app.setView(newView);

            backgroundImage(0, 0);

            app.setView(currentView);
        }
        else if ((_flags & RepeatBackgroundToFitSize) == RepeatBackgroundToFitSize)
        {
            imageManager::Image* image = imageManager::getImage("gui", _background);
            View currentView = app.getView();

            View newView(FloatRect(0, 0, width(), height()));
            newView.setViewport(FloatRect((float)left()/(float)app.getSize().x, (float)top()/(float)app.getSize().y, width()/(float)app.getSize().x, height()/(float)app.getSize().y));

            app.setView(newView);
            for (unsigned i = 0 ; i <= width() / image->getSize().x + 1 ; ++i)
                    for (unsigned j = 0 ; j <= height() / image->getSize().y + 1 ; ++j)
                        backgroundImage(i * image->getSize().x, j * image->getSize().y);

            app.setView(currentView);
        }
        else
        {
            backgroundImage(left(), top());
        }
    }

    if (!_backgroundShader.empty())
        gui::style::displayShader(app, _backgroundShader, left(), top(), width(), height());
}

void Widget::displayText(RenderWindow& app)
{
    if ((_flags & MouseOver) == MouseOver)
        _text.displayFullText(app, left(), top(), style::getContrastShader(1, 0, 0));
    else if ((_flags & Disabled) == Disabled)
        _text.displayFullText(app, left(), top(), style::getContrastShader(0.25, 0.25, 0.25));
    else if ((_flags & CustomTextShader) == CustomTextShader)
    {
        string_view shader = embeddedData<string>("custom-shader");
        if (shader == "fade")
            _text.displayFullText(app, left(), top(), style::getFadeShader(embeddedData<float>("fade-value")));
    }
    else
    {
        _text.displayFullText(app, left(), top());
    }
}

void Widget::display(RenderWindow& app)
{
    if ((_flags & Hidden) == Hidden)
        return;

    displayBackground(app);

    displayText(app);
}

void Widget::loadFromXMLElement(tinyxml2::XMLElement* elem)
{
    elem->QueryAttribute("x", &_x);
    elem->QueryAttribute("y", &_y);

    if (elem->Attribute("OriginXCenter"))
        addFlags(OriginXCenter);
    if (elem->Attribute("OriginRight"))
        addFlags(OriginRight);
    if (elem->Attribute("OriginYCenter"))
        addFlags(OriginYCenter);
    if (elem->Attribute("OriginBottom"))
        addFlags(OriginBottom);
    if (elem->Attribute("OriginCenter"))
        addFlags(OriginXCenter | OriginYCenter);

    if (elem->Attribute("XPositionRelativeToCenter"))
        addFlags(XPositionRelativeToCenter);
    if (elem->Attribute("XPositionRelativeToRight"))
        addFlags(XPositionRelativeToRight);
    if (elem->Attribute("YPositionRelativeToCenter"))
        addFlags(YPositionRelativeToCenter);
    if (elem->Attribute("YPositionRelativeToBottom"))
        addFlags(YPositionRelativeToBottom);
    if (elem->Attribute("PositionRelativeToCenter"))
        addFlags(XPositionRelativeToCenter | YPositionRelativeToCenter);
    if (elem->Attribute("XPositionRelativeToScreenSize"))
        addFlags(XPositionRelativeToScreenSize);
    if (elem->Attribute("YPositionRelativeToScreenSize"))
        addFlags(YPositionRelativeToScreenSize);
    if (elem->Attribute("PositionRelativeToScreenSize"))
        addFlags(XPositionRelativeToScreenSize | YPositionRelativeToScreenSize);
    if (elem->Attribute("AdjustSizeToText"))
        addFlags(AdjustSizeToText);

    int w = 0, h = 0;
    elem->QueryAttribute("width", &w);
    elem->QueryAttribute("height", &h);
    setSize(w, h);

    if (elem->Attribute("WidthRelativeToScreenSize"))
        addFlags(WidthRelativeToScreenSize);
    if (elem->Attribute("WidthMeansFixedMargin"))
        addFlags(WidthMeansFixedMargin);
    if (elem->Attribute("HeightRelativeToScreenSize"))
        addFlags(HeightRelativeToScreenSize);
    if (elem->Attribute("HeightMeansFixedMargin"))
        addFlags(HeightMeansFixedMargin);

    if (elem->Attribute("Fullscreen"))
    {
        setSize(100, 100);
        addFlags(WidthRelativeToScreenSize | HeightRelativeToScreenSize);
    }

    if (elem->Attribute("allBackground"))
        setBackground(elem->Attribute("allBackground"));

    if (elem->Attribute("AdjustBackgroundToSize"))
        addFlags(AdjustBackgroundToSize);
    if (elem->Attribute("RepeatBackgroundToFitSize"))
        addFlags(RepeatBackgroundToFitSize);

    if (elem->Attribute("Hidden"))
        addFlags(Hidden);

    if (elem->Attribute("VerticalScrollBar"))
        addFlags(VerticalScrollBar);

    if (elem->Attribute("CustomTextShader"))
        addFlags(CustomTextShader);

    if (elem->Attribute("textShaded"))
        addTextFlags(textManager::Shaded);

    if (elem->Attribute("allText"))
        setText(textManager::getText("gui", elem->Attribute("allText")));

    if (elem->Attribute("backgroundShader"))
        setBackgroundShader(elem->Attribute("backgroundShader"));
}

void Widget::show()
{
    _flags &= ~Hidden;
}

void Widget::hide()
{
    _flags |= Hidden;
}

} //namespace gui
