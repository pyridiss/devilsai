
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

#include "textManager/textManager.h"
#include "gui/dropDownList.h"
#include "gui/window.h"
#include "gui/style.h"

namespace gui{

DropDownList::DropDownList()
  : entries(),
    index(0)
{
    _text.setDefaultProperties(gui::style::buttonTextFont(), gui::style::buttonTextSize(), gui::style::normalButtonTextColor());

    _textFlags = textManager::HAlignCenter | textManager::FixedHeight | textManager::VAlignCenter;
}

void DropDownList::addEntry(textManager::PlainText& entry, string data)
{
    entries.emplace_back(entry, data);
}

bool DropDownList::mouseHovering()
{
    if ((_flags & Disabled) == Disabled)
        return false;

    if ((int)mousePosition().x >= left() && (int)mousePosition().x <= left() + width() &&
        (int)mousePosition().y >= top() && (int)mousePosition().y <= top() + height())
    {
        _flags |= MouseOver;
        return true;
    }
    else _flags &= ~MouseOver;

    return false;
}

bool DropDownList::mouseHoveringDeveloped()
{
    if ((_flags & Activated) == 0)
        return false;

    if ((int)mousePosition().x >= left() && (int)mousePosition().x <= left() + width() &&
        (int)mousePosition().y >= top() + height() && (int)mousePosition().y <= top() + height() * (1 + (int)entries.size()))
    {
        return true;
    }

    return false;
}

bool DropDownList::activated(Event event)
{
    if ((_flags & Disabled) == Disabled)
        return false;

    if (event.type == Event::MouseButtonPressed)
    {
        if (mouseHovering())
        {
            _flags ^= Activated;
            _parent->askFocus(this, (_flags & Activated) == Activated);
            return false;
        }
        else if (mouseHoveringDeveloped())
        {
            int i = mousePosition().y - (top() + height());
            index = i / height();
            setText(entries[index].first);
            addEmbeddedData("value", entries[index].second);
            _parent->addEvent(this, WidgetValueChanged, embeddedData<optionType>("value"));
            _flags &= ~Activated;
            _parent->askFocus(this, false);
        }
        else if ((_flags & Activated) == Activated)
        {
            _flags &= ~Activated;
            _parent->askFocus(this, false);
            return false;
        }
    }

    return false;
}

void DropDownList::setValue(optionType v)
{
    index = 0;
    while (index < entries.size() && entries[index].second != v.get<string>())
        ++index;
    setText(entries[index].first);
    addEmbeddedData("value", entries[index].second);
}

void DropDownList::display(RenderWindow& app)
{
    mouseHovering();

    Widget::display(app);

    if ((_flags & Activated) == Activated)
    {
        RectangleShape drawing(Vector2f(width(), height() * entries.size()));
        drawing.setPosition(left(), top() + height());
        drawing.setFillColor(Color::Black);
        drawing.setOutlineColor(Color(48, 48, 48, 255));
        drawing.setOutlineThickness(1);
        app.draw(drawing);

        if (mouseHoveringDeveloped())
        {
            int i = (mousePosition().y - (top() + height())) / height();
            i = min(i, (int)entries.size()-1);
            i = max(i, 0);
            RectangleShape drawing2(Vector2f(width(), height()));
            drawing2.setPosition(left(), top() + height() * (i+1));
            drawing2.setFillColor(Color(100, 100, 100));
            app.draw(drawing2);
        }

        unsigned i = 1;
        for (auto& e : entries)
        {
            textManager::RichText t;
            t.addFlags(textManager::HAlignCenter | textManager::OriginXCenter | textManager::OriginYCenter | textManager::FixedHeight | textManager::VAlignCenter);
            t.setSize(width(), height());
            t.setDefaultProperties(gui::style::buttonTextFont(), gui::style::buttonTextSize(), gui::style::normalButtonTextColor());

            t.create(e.first);

            if (i == index + 1)
                t.displayFullText(app, getXCenter(), getYCenter() + i * height(), style::getContrastShader(1, 0, 0));
            else
                t.displayFullText(app, getXCenter(), getYCenter() + i * height());

            ++i;
        }
    }
}

} //namespace gui
