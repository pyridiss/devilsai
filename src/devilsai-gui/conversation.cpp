
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2018  Quentin Henriet

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

#include <SFML/Graphics.hpp>

#include "tools/signals.h"
#include "tools/math.h"
#include "tools/timeManager.h"
#include "textManager/richText.h"
#include "gui/button.h"
#include "gui/textWidget.h"
#include "gui/window.h"

#include "gamedata.h"
#include "Jeu/options.h"

#include "devilsai-gui/conversation.h"

using namespace std;
using namespace sf;

gui::Window Gui;

textManager::RichText* _currentDialog = nullptr;
float _x = 0, _y = 0, _w = 0, _h = 0;
float _currentOffset = 0;
int _currentSpeed = 0;
gui::Button *_restart = nullptr, *_ok = nullptr, *_faster = nullptr, *_slower = nullptr;

void initConversation(RenderWindow& target)
{
    Gui.startWindow(target);
    Gui.loadFromFile("gui/conversation.xml");

    _restart = dynamic_cast<gui::Button*>(Gui.widget("restart"));
    _ok = dynamic_cast<gui::Button*>(Gui.widget("ok"));
    _faster = dynamic_cast<gui::Button*>(Gui.widget("faster"));
    _slower = dynamic_cast<gui::Button*>(Gui.widget("slower"));

    gui::TextWidget* text = dynamic_cast<gui::TextWidget*>(Gui.widget("text"));

    _x = text->left();
    _y = text->top();
    _w = text->width();
    _h = text->height();

    options::addOption<unsigned>(tools::math::sdbm_hash("dialog-width"), _w);

    _w += 2;

    _currentSpeed = options::option<unsigned>(tools::math::sdbm_hash("dialog-speed"));
}

void manageConversation(RenderWindow& target, Event& event)
{
    if (_currentDialog != nullptr)
    {
        if (_restart != nullptr && _restart->activated(target, event))
        {
            _currentOffset = -100;
        }
        if (_ok != nullptr && _ok->activated(target, event))
        {
            _currentDialog = nullptr;
            gamedata::listDialogs().erase(gamedata::listDialogs().begin());
            tools::signals::addSignal("disable-cinematic-mode");
        }
        if (_faster != nullptr && _faster->activated(target, event))
        {
            if (_currentSpeed < 50)
            {
                _currentSpeed += 2;
                options::addOption<unsigned>(tools::math::sdbm_hash("dialog-speed"), _currentSpeed);
            }
        }
        if (_slower != nullptr && _slower->activated(target, event))
        {
            if (_currentSpeed > 2)
            {
                _currentSpeed -= 2;
                options::addOption<unsigned>(tools::math::sdbm_hash("dialog-speed"), _currentSpeed);
            }
        }
    }
}

void manageConversation(RenderWindow& target)
{
    if (!gamedata::listDialogs().empty() && _currentDialog == nullptr)
    {
        _currentDialog = &gamedata::listDialogs().front();
        _currentOffset = -100;
        tools::signals::addSignal("enable-cinematic-mode");
    }

    if (_currentDialog != nullptr)
    {
        _currentOffset += tools::timeManager::I((double)_currentSpeed/100.0);

        if (_currentOffset > _currentDialog->height() + 5)
        {
            _currentDialog = nullptr;
            gamedata::listDialogs().erase(gamedata::listDialogs().begin());
            tools::signals::addSignal("disable-cinematic-mode");
        }
    }
}

void displayConversation(RenderWindow& target)
{
    if (_currentDialog == nullptr)
        return;

    Gui.display(target);

    View newView(FloatRect(0, (int)_currentOffset, _w, _h));
    newView.setViewport(FloatRect(_x/(float)target.getSize().x, _y/(float)target.getSize().y, _w/(float)target.getSize().x, _h/(float)target.getSize().y));

    _currentDialog->displayToView(target, newView);
}
