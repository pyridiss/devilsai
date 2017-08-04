
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
{
    addState("normal");
    addState("developed");

    setTextFont(gui::style::defaultTextFont(), gui::style::defaultTextSize());

    setTextColor("normal", Color::Black);

    index = 0;
}

void DropDownList::addEntry(String32& entry, string data)
{
    entries.push_back(pair<String32, string>(entry, data));
}

bool DropDownList::mouseHovering(RenderWindow& app)
{
    if (Mouse::getPosition(app).x >= getXTopLeft() && Mouse::getPosition(app).x <= getXTopLeft() + width &&
        Mouse::getPosition(app).y >= getYTopLeft() && Mouse::getPosition(app).y <= getYTopLeft() + height)
    {
        return true;
    }

    return false;
}

bool DropDownList::mouseHoveringDeveloped(RenderWindow& app)
{
    if (Mouse::getPosition(app).x >= getXTopLeft() && Mouse::getPosition(app).x <= getXTopLeft() + width &&
        Mouse::getPosition(app).y >= getYTopLeft() && Mouse::getPosition(app).y <= getYTopLeft() + height * (1 + entries.size()))
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
            int i = Mouse::getPosition(app).y - (getYTopLeft() + height);
            index = i / height;
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
}

string DropDownList::value()
{
    return entries[index].second;
}

void DropDownList::display(RenderWindow& app)
{
    setAllText(entries[index].first);
    updateTextPosition();

    Widget::display(app);

    if (currentState == "developed")
    {
        RectangleShape drawing(Vector2f(width, height * entries.size()));
        drawing.setPosition(getXTopLeft(), getYTopLeft() + height);
        drawing.setFillColor(Color::Black);
        drawing.setOutlineColor(Color(48, 48, 48, 255));
        drawing.setOutlineThickness(1);
        app.draw(drawing);

        int i = 1;
        for (auto& e : entries)
        {
            Text t;
            t.setString(e.first);
            t.setFont(gui::style::defaultTextFont());
            t.setCharacterSize(gui::style::defaultTextSize());

            FloatRect rect = t.getGlobalBounds();
            t.setPosition((int)(getXTopLeft() + width/2 - rect.width/2 - 1), (int)(getYTopLeft() + i*height + height/2 - rect.height/2 - 3));

            app.draw(t);

            ++i;
        }
    }
}

} //namespace gui
