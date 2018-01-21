
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

#include "gui/widget.h"
#include "gui/window.h"

#include "devilsai-screens/journal.h"


void updateState();

gui::Window _window;
gui::Widget* _text = nullptr, *_number = nullptr;
gui::Widget* _first = nullptr, *_previous = nullptr, *_next = nullptr, *_last = nullptr;

vector<JournalPage> _journal;
unsigned _currentPage = 0;

void initJournal(RenderWindow& target)
{
    _window.startWindow(target);
    _window.loadFromFile("gui/journal.xml");

    _text = _window.widget("text");
    if (_text == nullptr)
        tools::debug::error("The file gui/journal.xml does not contain a widget named text.", "gui", __FILENAME__, __LINE__);

    _number = _window.widget("number");
    if (_text == nullptr)
        tools::debug::error("The file gui/journal.xml does not contain a widget named number.", "gui", __FILENAME__, __LINE__);

    _first = _window.widget("first");
    if (_first == nullptr)
        tools::debug::error("The file gui/journal.xml does not contain a widget named text.", "gui", __FILENAME__, __LINE__);

    _previous = _window.widget("previous");
    if (_previous == nullptr)
        tools::debug::error("The file gui/journal.xml does not contain a widget named previous.", "gui", __FILENAME__, __LINE__);

    _next = _window.widget("next");
    if (_next == nullptr)
        tools::debug::error("The file gui/journal.xml does not contain a widget named next.", "gui", __FILENAME__, __LINE__);

    _last = _window.widget("last");
    if (_last == nullptr)
        tools::debug::error("The file gui/journal.xml does not contain a widget named last.", "gui", __FILENAME__, __LINE__);

}

void addJournalEntry(string world, string questName, string newEntry)
{
    JournalPage* page = nullptr;
    for (auto& p : _journal)
    {
        if (p.quest == questName)
        {
            page = &p;
            break;
        }
    }

    if (page == nullptr)
    {
        _journal.emplace_back();
        page = &_journal.back();
        page->quest = questName;
    }

    for (auto& i : page->paragraphs)
    {
        if (i == newEntry)
            return;
    }

    page->paragraphs.push_back(newEntry);
    page->plainText += " @n[20]";
    page->plainText += textManager::getText(world, newEntry);
    tools::signals::addSignal("ingame-toolbar:warn-journal");
    updateState();
}

void updateState()
{
    if (_currentPage == 0)
    {
        _first->setCurrentState("disabled");
        _previous->setCurrentState("disabled");
    }
    else
    {
        _first->setCurrentState("normal");
        _previous->setCurrentState("normal");
    }
    if (_currentPage == _journal.size() - 1)
    {
        _next->setCurrentState("disabled");
        _last->setCurrentState("disabled");
    }
    else
    {
        _next->setCurrentState("normal");
        _last->setCurrentState("normal");
    }

    if (_journal.size() > _currentPage)
        _text->setValue(_journal[_currentPage].plainText.toStdString());

    textManager::PlainText number("* / *");
    number.addParameter((int)(_currentPage + 1));
    number.addParameter((int)(_journal.size()));
    _number->setValue(number.toStdString());
}

void manageJournal(RenderWindow& target, Event& event)
{
    unsigned oldPage = _currentPage;

    if (_first->activated(target, event))
        _currentPage = 0;

    if (_previous->activated(target, event))
        _currentPage = max((unsigned)0, _currentPage - 1);

    if (_next->activated(target, event))
        _currentPage = min((unsigned)_journal.size() - 1, _currentPage + 1);

    if (_last->activated(target, event))
        _currentPage = _journal.size() - 1;

    if (_currentPage != oldPage)
        updateState();
}

void displayJournal(RenderWindow& target)
{
    _window.display(target);
}
