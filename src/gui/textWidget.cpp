
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

bool TextWidget::mouseHovering(RenderWindow& app)
{
    if (state == Disabled) return false;

    if (Mouse::getPosition(app).x >= getXTopLeft() && Mouse::getPosition(app).x <= getXTopLeft() + width &&
        Mouse::getPosition(app).y >= getYTopLeft() && Mouse::getPosition(app).y <= getYTopLeft() + height)
    {
        if (state == Normal) state = Hover;
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
    switch (state)
    {
        case Normal:
            if (!normal.background.empty())
                imageManager::display(app, "misc", normal.background, getXTopLeft(), getYTopLeft());
            app.draw(normal.text);
            if (normal.shader != NULL)
                normal.shader(app, getXTopLeft(), getYTopLeft(), width, height);
            break;
        case Active:
            if (!active.background.empty())
                imageManager::display(app, "misc", active.background, getXTopLeft(), getYTopLeft());
            app.draw(active.text);
            if (active.shader != NULL)
                active.shader(app, getXTopLeft(), getYTopLeft(), width, height);
            break;
        case Hover:
            if (!hover.background.empty())
                imageManager::display(app, "misc", hover.background, getXTopLeft(), getYTopLeft());
            app.draw(hover.text);
            if (hover.shader != NULL)
                hover.shader(app, getXTopLeft(), getYTopLeft(), width, height);
            break;
        case Disabled:
            if (!disabled.background.empty())
                imageManager::display(app, "misc", disabled.background, getXTopLeft(), getYTopLeft());
            app.draw(disabled.text);
            if (disabled.shader != NULL)
                disabled.shader(app, getXTopLeft(), getYTopLeft(), width, height);
            break;
    }
}

} //namespace gui
