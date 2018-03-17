
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2018  Quentin Henriet

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

#include "tools/timeManager.h"

#include "textManager/plainText.h"
#include "textManager/textManager.h"

#include "imageManager/image.h"
#include "imageManager/imageManager.h"

#include "gui/progressBar.h"
#include "gui/style.h"

namespace gui{

ProgressBar::ProgressBar()
  : _value(-0.01)
{
    _text.setDefaultProperties("liberation", gui::style::defaultTextSize(), Color::Black);
    _textFlags = textManager::HAlignCenter | textManager::FixedHeight | textManager::VAlignCenter;
}

bool ProgressBar::mouseHovering(RenderWindow& app)
{
    return false;
}

bool ProgressBar::activated(RenderWindow& app, Event event)
{
    return false;
}

void ProgressBar::setValue(optionType v)
{
    if (v.is<float>())
    {
        _value = v.get<float>();
        textManager::PlainText t(_value);
        t += " %";
        setText(t);
    }
}

void ProgressBar::display(RenderWindow& app)
{
    if ((_flags & Hidden) == Hidden)
        return;

    RectangleShape outline(Vector2f(width(), height()));
    outline.setPosition(left(), top());
    outline.setFillColor(Color::Transparent);
    outline.setOutlineColor(Color::Black);
    outline.setOutlineThickness(1);
    app.draw(outline);

    if (_value >= 0)
    {
        RectangleShape bar(Vector2f(width() * _value / 100.f, height()));
        bar.setPosition(left(), top());
        bar.setFillColor(Color::Red);
        app.draw(bar);
    }
    else
    {
        RectangleShape bar(Vector2f(width() / 10, height()));
        bar.setPosition(left() + (0.45 - abs(0.45+_value)) * 2 * width(), top());
        bar.setFillColor(Color::Red);
        app.draw(bar);
        if (_value > -0.9) _value -= tools::timeManager::I(0.01);
        if (_value < -0.9) _value = -0.01;
    }

    displayText(app);
}

} //namespace gui
