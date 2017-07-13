
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

#include "gui/scrollingList.h"
#include "gui/style.h"

namespace gui{

ScrollingList::ScrollingList()
{
    addState("normal");

    setTextFont(gui::style::defaultTextFont(), gui::style::defaultTextSize());

    setTextColor("normal", Color::Black);

    index = 0;
}

void ScrollingList::addEntry(String32& entry, string data)
{
    entries.push_back(pair<String32, string>(entry, data));
}

void ScrollingList::removeAllEntries()
{
    entries.clear();
}

bool ScrollingList::mouseHovering(RenderWindow& app)
{
    if (Mouse::getPosition(app).x >= getXTopLeft() && Mouse::getPosition(app).x <= getXTopLeft() + width &&
        Mouse::getPosition(app).y >= getYTopLeft() && Mouse::getPosition(app).y <= getYTopLeft() + height)
    {
        return true;
    }

    return false;
}

bool ScrollingList::activated(RenderWindow& app, Event event)
{
    if (event.type == Event::MouseButtonPressed)
    {
        if (mouseHovering(app))
        {
            if (Mouse::getPosition(app).x <= getXTopLeft() + width - 20)
            {
                index = (Mouse::getPosition(app).y - (getYTopLeft() + 5))/20 + firstEntryDisplayed;
                if (index >= entries.size()) index = entries.size() - 1;
                return false;
            }
            else
            {
                mouseScrolling = true;
                scrolling = (double)(Mouse::getPosition(app).y - getYTopLeft()) / (double)height;
            }
        }
    }
    else if (event.type == Event::MouseMoved && mouseHovering(app) && mouseScrolling)
    {
        scrolling = (double)(Mouse::getPosition(app).y - getYTopLeft()) / (double)height;
    }
    else if (event.type == Event::MouseButtonReleased)
    {
        mouseScrolling = false;
    }
    else if (event.type == Event::MouseWheelScrolled)
    {
        scrolling -= 0.1 * event.mouseWheelScroll.delta;
        scrolling = min(1., scrolling);
        scrolling = max(0., scrolling);
    }

    return false;
}

void ScrollingList::setData(tools::signals::SignalData& data)
{
    data.stringData = entries[index].second;
}

void ScrollingList::setValue(const tools::signals::SignalData& d)
{
    index = 0;
    while (index < entries.size() && entries[index].second != d.stringData)
        ++index;
}

void ScrollingList::display(RenderWindow& app)
{
    Widget::display(app);

    RectangleShape drawing(Vector2f(width, height));
    drawing.setPosition(getXTopLeft(), getYTopLeft());
    drawing.setFillColor(Color::Black);
    drawing.setOutlineColor(Color(48, 48, 48, 255));
    drawing.setOutlineThickness(1);
    app.draw(drawing);

    RectangleShape bar(Vector2f(4, height - 16));
    bar.setPosition(getXTopLeft() + width - 12, getYTopLeft() + 8);
    bar.setFillColor(Color::White);
    app.draw(bar);

    CircleShape cursor(6);
    cursor.setPosition(getXTopLeft() + width - 16, getYTopLeft() + scrolling * (height-12));
    cursor.setFillColor(Color::White);
    cursor.setOutlineColor(Color(48, 48, 48, 255));
    cursor.setOutlineThickness(1);
    app.draw(cursor);

    int i = 0;
    int currentY = 0;

    int maxEntriesDisplayed = (height - 25) / 20;
    int firstEntryDisplayedAtMaxScrolling = max(0, (int)(entries.size() - maxEntriesDisplayed));
    firstEntryDisplayed = max(0., firstEntryDisplayedAtMaxScrolling * scrolling);

    for (auto& e : entries)
    {
        if (i < firstEntryDisplayed)
        {
            ++i;
            continue;
        }

        Text t;
        t.setString(e.first);
        t.setFont(gui::style::defaultTextFont());
        t.setCharacterSize(16);

        FloatRect rect = t.getGlobalBounds();
        t.setPosition((int)(getXTopLeft() + 5), (int)(getYTopLeft() + 5 + currentY));

        if (i == index)
        {
            RectangleShape highlight(Vector2f(width - 20, 20));
            highlight.setPosition(getXTopLeft(), getYTopLeft() + 5 + currentY);
            highlight.setFillColor(Color::Red);
            app.draw(highlight);
        }

        app.draw(t);

        ++i;
        currentY += 20;

        if (currentY > height - 20 - 5)
            break;
    }
}

} //namespace gui
