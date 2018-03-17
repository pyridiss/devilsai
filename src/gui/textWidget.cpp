
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2014  Quentin Henriet

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

#include "textManager/plainText.h"
#include "textManager/textManager.h"

#include "imageManager/image.h"
#include "imageManager/imageManager.h"

#include "gui/textWidget.h"
#include "gui/style.h"

namespace gui{

TextWidget::TextWidget()
  : _verticalScrolling(0)
{
    _text.setDefaultProperties("liberation", gui::style::defaultTextSize(), Color::Black);

    _flags |= AdjustSizeToText;
}

void TextWidget::setVerticalScrolling(int percentage)
{
    int maxScroll = max(1, _text.height() - height());
    _verticalScrolling = percentage * maxScroll / 100;
}

bool TextWidget::mouseHovering(RenderWindow& app)
{
    if (mousePosition().x >= left() && mousePosition().x <= left() + width() &&
        mousePosition().y >= top() && mousePosition().y <= top() + height())
    {
        return true;
    }

    return false;
}

bool TextWidget::mouseHoveringVerticalScrollBar(RenderWindow& app)
{
    if ((_flags & VerticalScrollBar) == 0)
        return false;

    if (mousePosition().x >= left() + width() && mousePosition().x <= left() + width() + 20 &&
        mousePosition().y >= top() && mousePosition().y <= top() + height())
    {
        return true;
    }

    return false;
}

bool TextWidget::activated(RenderWindow& app, Event event)
{
    if ((_flags & VerticalScrollBar) == 0)
        return false;

    if (event.type == Event::MouseButtonPressed && mouseHoveringVerticalScrollBar(app))
    {
        setVerticalScrolling((float)(mousePosition().y - top()) / (float)height() * 100);
        _flags |= VerticalScrollBarInUse;
    }
    else if (event.type == Event::MouseMoved &&
        (mouseHovering(app) || mouseHoveringVerticalScrollBar(app)) &&
        (_flags & VerticalScrollBarInUse) == VerticalScrollBarInUse)
    {
        setVerticalScrolling((float)(mousePosition().y - top()) / (float)height() * 100);
    }
    else if (event.type == Event::MouseButtonReleased)
    {
        _flags &= ~VerticalScrollBarInUse;
    }
    else if (event.type == Event::MouseWheelScrolled && (mouseHovering(app) || mouseHoveringVerticalScrollBar(app)))
    {
        _verticalScrolling -= 10 * event.mouseWheelScroll.delta;
    }

    _verticalScrolling = min(_text.height() - height(), _verticalScrolling);
    _verticalScrolling = max(0, _verticalScrolling);

    return false;
}

void TextWidget::setValue(optionType v)
{
    if (v.is<string>())
        v.set<textManager::PlainText>(textManager::PlainText(v.get<string>()));

    if (v.is<textManager::PlainText>())
    {
        if (_embeddedData.find("format") != _embeddedData.end())
        {
            textManager::PlainText& t = embeddedData<textManager::PlainText>("format");
            t.resetParameters();
            t.addParameter(v.get<textManager::PlainText>());
            setText(t);
        }
        else setText(v.get<textManager::PlainText>());
    }
}

void TextWidget::display(RenderWindow& app)
{
    if ((_flags & Hidden) == Hidden)
        return;

    displayBackground(app);

    if ((_flags & VerticalScrollBar) == VerticalScrollBar)
    {
        RectangleShape bar(Vector2f(4, height() - 16));
        bar.setPosition(left() + width() + 8, top() + 8);
        bar.setFillColor(Color::White);
        app.draw(bar);

        CircleShape cursor(6);
        float maxScroll = max(1, _text.height() - height());
        cursor.setPosition(left() + width() + 4, top() + (float)_verticalScrolling / maxScroll * (height()-12));
        cursor.setFillColor(Color::White);
        cursor.setOutlineColor(Color(48, 48, 48, 255));
        cursor.setOutlineThickness(1);
        app.draw(cursor);

        View newView(FloatRect(0, _verticalScrolling, width(), height()));
        newView.setViewport(FloatRect((float)left()/(float)app.getSize().x, (float)top()/(float)app.getSize().y,
                                      width()/(float)app.getSize().x, height()/(float)app.getSize().y));

        _text.displayToView(app, newView);
    }
    else
    {
        displayText(app);
    }
}

} //namespace gui
