
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

    setTextFont(gui::style::defaultTextFont(), gui::style::defaultTextSize());

    setTextColor("normal", Color::Black);
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

bool TextWidget::activated(RenderWindow& app, Event event)
{
    return false;
}

void TextWidget::setData(tools::signals::SignalData& data)
{
}

void TextWidget::display(RenderWindow& app)
{
    const auto& state = states.find("normal");

    if (!state->second.background.empty())
        imageManager::display(app, "misc", state->second.background, getXTopLeft(), getYTopLeft());

    if (state->second.backgroundShader != nullptr)
        state->second.backgroundShader(app, getXTopLeft(), getYTopLeft(), width, height);

    app.draw(state->second.text);

    if (state->second.foregroundShader != nullptr)
        state->second.foregroundShader(app, getXTopLeft(), getYTopLeft(), width, height);
}

} //namespace gui