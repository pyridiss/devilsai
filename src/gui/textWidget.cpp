
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

#include "tools/textManager.h"

#include "imageManager/image.h"
#include "imageManager/imageManager.h"

#include "gui/textWidget.h"
#include "gui/style.h"

namespace gui{

TextWidget::TextWidget()
  : _verticalScrolling(0)
{
    addState("normal");

    states.find("normal")->second.text.setDefaultProperties("liberation", gui::style::defaultTextSize(), Color::Black);

    _flags |= AdjustSizeToText;
}

void TextWidget::setVerticalScrolling(int percentage)
{
    int maxScroll = max(1, states[currentState].text.height() - height());
    _verticalScrolling = percentage * maxScroll / 100;
}

bool TextWidget::mouseHovering(RenderWindow& app)
{
    if (Mouse::getPosition(app).x >= left() && Mouse::getPosition(app).x <= left() + width() &&
        Mouse::getPosition(app).y >= top() && Mouse::getPosition(app).y <= top() + height())
    {
        return true;
    }

    return false;
}

bool TextWidget::mouseHoveringVerticalScrollBar(RenderWindow& app)
{
    if (Mouse::getPosition(app).x >= left() + width() && Mouse::getPosition(app).x <= left() + width() + 20 &&
        Mouse::getPosition(app).y >= top() && Mouse::getPosition(app).y <= top() + height())
    {
        return true;
    }

    return false;
}

bool TextWidget::activated(RenderWindow& app, Event event)
{
    if (event.type == Event::MouseButtonPressed && mouseHoveringVerticalScrollBar(app))
    {
        setVerticalScrolling((float)(Mouse::getPosition(app).y - top()) / (float)height() * 100);
        _flags |= VerticalScrollBarInUse;
    }
    else if (event.type == Event::MouseMoved &&
        (mouseHovering(app) || mouseHoveringVerticalScrollBar(app)) &&
        (_flags & VerticalScrollBarInUse) == VerticalScrollBarInUse)
    {
        setVerticalScrolling((float)(Mouse::getPosition(app).y - top()) / (float)height() * 100);
    }
    else if (event.type == Event::MouseButtonReleased)
    {
        _flags &= ~VerticalScrollBarInUse;
    }
    else if (event.type == Event::MouseWheelScrolled)
    {
        _verticalScrolling -= 10 * event.mouseWheelScroll.delta;
    }

    _verticalScrolling = min(states[currentState].text.height() - height(), _verticalScrolling);
    _verticalScrolling = max(0, _verticalScrolling);

    return false;
}

void TextWidget::setValue(const string& d)
{
    addEmbeddedData("value", d);
    String32 text = textManager::fromStdString(d);
    setAllText(text);
}

string TextWidget::value()
{
    return embeddedData("value");
}

void TextWidget::display(RenderWindow& app)
{
    if ((_flags & Hidden) == Hidden)
        return;

    const auto& state = states.find(currentState);

    if (!state->second.background.empty())
    {
        if ((_flags & AdjustBackgroundToSize) == AdjustBackgroundToSize)
        {
            imageManager::Image* image = imageManager::getImage("gui", state->second.background);
            View currentView = app.getView();

            View newView(FloatRect(0, 0, image->getSize().x, image->getSize().y));
            newView.setViewport(FloatRect((float)left()/(float)app.getSize().x, (float)top()/(float)app.getSize().y, width()/(float)app.getSize().x, height()/(float)app.getSize().y));

            app.setView(newView);
            imageManager::display(app, "gui", state->second.background, 0, 0);

            app.setView(currentView);
        }
        else if ((_flags & RepeatBackgroundToFitSize) == RepeatBackgroundToFitSize)
        {
            imageManager::Image* image = imageManager::getImage("gui", state->second.background);
            View currentView = app.getView();

            View newView(FloatRect(0, 0, width(), height()));
            newView.setViewport(FloatRect((float)left()/(float)app.getSize().x, (float)top()/(float)app.getSize().y, width()/(float)app.getSize().x, height()/(float)app.getSize().y));

            app.setView(newView);
            for (unsigned i = 0 ; i <= width() / image->getSize().x + 1 ; ++i)
                    for (unsigned j = 0 ; j <= height() / image->getSize().y + 1 ; ++j)
                        imageManager::display(app, "gui", state->second.background, i * image->getSize().x, j * image->getSize().y);

            app.setView(currentView);
        }
        else
        {
            imageManager::display(app, "gui", state->second.background, left(), top());
        }
    }

    if (state->second.backgroundShader != nullptr)
        state->second.backgroundShader(app, left(), top(), width(), height());
    else if (!state->second.bShader.empty())
        gui::style::displayShader(app, state->second.bShader, left(), top(), width(), height());

    if ((_flags & VerticalScrollBar) == VerticalScrollBar)
    {
        RectangleShape bar(Vector2f(4, height() - 16));
        bar.setPosition(left() + width() + 8, top() + 8);
        bar.setFillColor(Color::White);
        app.draw(bar);

        CircleShape cursor(6);
        float maxScroll = max(1, states[currentState].text.height() - height());
        cursor.setPosition(left() + width() + 4, top() + (float)_verticalScrolling / maxScroll * (height()-12));
        cursor.setFillColor(Color::White);
        cursor.setOutlineColor(Color(48, 48, 48, 255));
        cursor.setOutlineThickness(1);
        app.draw(cursor);

        View newView(FloatRect(0, _verticalScrolling, width(), height()));
        newView.setViewport(FloatRect((float)left()/(float)app.getSize().x, (float)top()/(float)app.getSize().y,
                                      width()/(float)app.getSize().x, height()/(float)app.getSize().y));

        state->second.text.displayToView(app, newView);
    }
    else
    {
        state->second.text.displayFullText(app, left(), top());
    }

    if (state->second.foregroundShader != nullptr)
        state->second.foregroundShader(app, left(), top(), width(), height());
    else if (!state->second.fShader.empty())
        gui::style::displayShader(app, state->second.fShader, left(), top(), width(), height());
}

} //namespace gui
