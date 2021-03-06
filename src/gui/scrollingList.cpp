
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

#include "tools/math.h"

#include "textManager/plainText.h"

#include "gui/scrollingList.h"
#include "gui/style.h"

namespace gui{

ScrollingList::ScrollingList()
  : entries(),
    index(0),
    firstEntryDisplayed(0)
{
    _text.setDefaultProperties(parameter<string>(tools::hash("text-font")),
                               parameter<int>(tools::hash("text-size")),
                               parameter<Color>(tools::hash("text-color")));
}

void ScrollingList::addEntry(textManager::PlainText& entry, string data)
{
    //The first entry is auto-selected.
    if (entries.empty())
        addEmbeddedData<string>("value", data);

    entries.emplace_back(entry, data);
}

void ScrollingList::removeAllEntries()
{
    entries.clear();
}

bool ScrollingList::mouseHovering()
{
    if (mousePosition().x >= left() && mousePosition().x <= left() + width() &&
        mousePosition().y >= top() && mousePosition().y <= top() + height())
    {
        return true;
    }

    return false;
}

bool ScrollingList::activated(Event event)
{
    if (event.type == Event::MouseButtonPressed)
    {
        if (mouseHovering())
        {
            if (mousePosition().x <= left() + width() - 20)
            {
                if (entries.empty()) return false;
                index = (mousePosition().y - (top() + 5))/20 + firstEntryDisplayed;
                if (index >= entries.size()) index = entries.size() - 1;
                addEmbeddedData<string>("value", entries[index].second);
                return false;
            }
            else
            {
                mouseScrolling = true;
                scrolling = (double)(mousePosition().y - top()) / (double)height();
            }
        }
    }
    else if (event.type == Event::MouseMoved && mouseHovering() && mouseScrolling)
    {
        scrolling = (double)(mousePosition().y - top()) / (double)height();
    }
    else if (event.type == Event::MouseButtonReleased)
    {
        mouseScrolling = false;
    }
    else if (event.type == Event::MouseWheelScrolled)
    {
        scrolling -= 0.1 * event.mouseWheelScroll.delta;
    }

    scrolling = min(0.99, scrolling);
    scrolling = max(0., scrolling);

    return false;
}

void ScrollingList::setValue(optionType v)
{
    index = 0;
    while (index < entries.size() && entries[index].second != v.get<string>())
        ++index;

    addEmbeddedData<string>("value", entries[index].second);
}

void ScrollingList::display(RenderWindow& app)
{
    Widget::display(app);

    RectangleShape drawing(Vector2f(width(), height()));
    drawing.setPosition(left(), top());
    drawing.setFillColor(parameter<Color>(tools::hash("list-background-color")));
    drawing.setOutlineColor(Color(48, 48, 48, 255));
    drawing.setOutlineThickness(1);
    app.draw(drawing);

    RectangleShape bar(Vector2f(4, height() - 16));
    bar.setPosition(left() + width() - 12, top() + 8);
    bar.setFillColor(Color::White);
    app.draw(bar);

    CircleShape cursor(6);
    cursor.setPosition(left() + width() - 16, top() + scrolling * (height()-12));
    cursor.setFillColor(Color::White);
    cursor.setOutlineColor(Color(48, 48, 48, 255));
    cursor.setOutlineThickness(1);
    app.draw(cursor);

    unsigned i = 0;
    int currentY = 0;

    int maxEntriesDisplayed = (height() - 25) / 20;
    int firstEntryDisplayedAtMaxScrolling = max(0, (int)(entries.size() - maxEntriesDisplayed));
    firstEntryDisplayed = max(0., firstEntryDisplayedAtMaxScrolling * scrolling);

    if (mouseHovering())
    {
        int h_index = (mousePosition().y - (top() + 5)) / 20;
        h_index = min(h_index, maxEntriesDisplayed-1);
        h_index = min(h_index, (int)entries.size()-1);
        h_index = max(h_index, 0);
        RectangleShape highlight(Vector2f(width() - 20, 20));
        highlight.setPosition(left(), top() + 5 + 20 * h_index);
        highlight.setFillColor(parameter<Color>(tools::hash("list-highlight-color")));
        app.draw(highlight);
    }

    for (auto& e : entries)
    {
        if (i < firstEntryDisplayed)
        {
            ++i;
            continue;
        }

        textManager::RichText t;
        t.setSize(width() - 20, 20);
        t.setDefaultProperties(parameter<string>(tools::hash("list-text-font")),
                               parameter<int>(tools::hash("list-text-size")),
                               parameter<Color>(tools::hash("list-text-color")));
        t.create(e.first);

        if (i == index)
        {
            RectangleShape selected(Vector2f(width() - 20, 20));
            selected.setPosition(left(), top() + 5 + currentY);
            selected.setFillColor(parameter<Color>(tools::hash("list-selected-color")));
            app.draw(selected);
        }

        t.displayFullText(app, left() + 5, top() + 5 + currentY);

        ++i;
        currentY += 20;

        if (currentY > height() - 20 - 5)
            break;
    }
}

} //namespace gui
