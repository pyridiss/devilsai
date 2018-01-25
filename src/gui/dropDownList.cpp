
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

#include "gui/dropDownList.h"
#include "gui/style.h"

namespace gui{

DropDownList::DropDownList()
  : entries(),
    index(0)
{
    addState("normal");
    addState("developed");

    states.find("normal")->second.text.setDefaultProperties(gui::style::buttonTextFont(), gui::style::buttonTextSize(), gui::style::normalButtonTextColor());
    states.find("developed")->second.text.setDefaultProperties(gui::style::buttonTextFont(), gui::style::buttonTextSize(), gui::style::normalButtonTextColor());

    _textFlags = textManager::HAlignCenter | textManager::FixedHeight | textManager::VAlignCenter;
}

void DropDownList::addEntry(textManager::PlainText& entry, string data)
{
    entries.push_back(pair<textManager::PlainText, string>(entry, data));
}

bool DropDownList::mouseHovering(RenderWindow& app)
{
    if ((int)Mouse::getPosition(app).x >= left() && (int)Mouse::getPosition(app).x <= left() + width() &&
        (int)Mouse::getPosition(app).y >= top() && (int)Mouse::getPosition(app).y <= top() + height())
    {
        return true;
    }

    return false;
}

bool DropDownList::mouseHoveringDeveloped(RenderWindow& app)
{
    if ((int)Mouse::getPosition(app).x >= left() && (int)Mouse::getPosition(app).x <= left() + width() &&
        (int)Mouse::getPosition(app).y >= top() + height() && (int)Mouse::getPosition(app).y <= top() + height() * (1 + (int)entries.size()))
    {
        return true;
    }

    return false;
}

bool DropDownList::activated(RenderWindow& app, Event event)
{
    if (event.type == Event::MouseButtonPressed)
    {
        if (mouseHovering(app))
        {
            if (currentState == "normal")
            {
                currentState = "developed";
                _needsFocus = true;
                return false;
            }
            if (currentState == "developed")
            {
                currentState = "normal";
                _needsFocus = false;
                return false;
            }
        }
        else if (mouseHoveringDeveloped(app) && currentState == "developed")
        {
            int i = Mouse::getPosition(app).y - (top() + height());
            index = i / height();
            setAllText(entries[index].first);
            currentState = "normal";
            _needsFocus = false;
        }
        else if (currentState == "developed")
        {
            currentState = "normal";
            _needsFocus = false;
            return false;
        }
    }

    return false;
}

void DropDownList::setValue(const string& d)
{
    index = 0;
    while (index < entries.size() && entries[index].second != d)
        ++index;
    setAllText(entries[index].first);
}

string DropDownList::value()
{
    return entries[index].second;
}

void DropDownList::display(RenderWindow& app)
{
    Widget::display(app);

    if (currentState == "developed")
    {
        RectangleShape drawing(Vector2f(width(), height() * entries.size()));
        drawing.setPosition(left(), top() + height());
        drawing.setFillColor(Color::Black);
        drawing.setOutlineColor(Color(48, 48, 48, 255));
        drawing.setOutlineThickness(1);
        app.draw(drawing);

        if (mouseHoveringDeveloped(app) && currentState == "developed")
        {
            int i = (Mouse::getPosition(app).y - (top() + height())) / height();
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

            if (i == index + 1)
                t.setDefaultProperties(gui::style::buttonTextFont(), gui::style::buttonTextSize(), gui::style::normalButtonTextColor());
            else
                t.setDefaultProperties(gui::style::buttonTextFont(), gui::style::buttonTextSize(), gui::style::activeButtonTextColor());

            t.create(e.first);
            t.displayFullText(app, getXCenter(), getYCenter() + i * height());

            ++i;
        }
    }
}

} //namespace gui
