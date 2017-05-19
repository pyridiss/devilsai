
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

#include "gui/textWidget.h"
#include "gui/style.h"
#include "imageManager/imageManager.h"

namespace gui{

TextWidget::TextWidget()
{
    minimalistWidget w;

    states.insert(map<string,minimalistWidget>::value_type("normal", w));

    setTextFont(gui::style::buttonTextFont(), gui::style::buttonTextSize());

    setTextColor("normal", gui::style::normalButtonTextColor());;
}

bool TextWidget::mouseHovering(RenderWindow& app)
{
    if (Mouse::getPosition(app).x >= getXTopLeft() && Mouse::getPosition(app).x <= getXTopLeft() + width &&
        Mouse::getPosition(app).y >= getYTopLeft() && Mouse::getPosition(app).y <= getYTopLeft() + height)
    {
        return true;
    }

    return false;
}

bool TextWidget::activated(RenderWindow& app, Event::EventType event)
{
    return false;
}

void TextWidget::display(RenderWindow& app)
{
    const auto& state = states.find("normal");

    if (!state->second.background.empty())
        imageManager::display(app, "misc", state->second.background, getXTopLeft(), getYTopLeft());

    FloatRect rect = state->second.text.getGlobalBounds();
    state->second.text.setPosition((int)(getXCenter() - rect.width/2 - 1), (int)(getYCenter() - rect.height/2 - 3));

    app.draw(state->second.text);

    if (state->second.shader != NULL)
        state->second.shader(app, getXTopLeft(), getYTopLeft(), width, height);
}

} //namespace gui
