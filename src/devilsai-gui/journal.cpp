
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

#include <tinyxml2.h>

#include "textManager/textManager.h"

#include "gui/widget.h"
#include "gui/window.h"

#include "devilsai-gui/journal.h"

using namespace tinyxml2;

void updateState();

gui::Window _window;
gui::Widget* _text = nullptr, *_number = nullptr;
gui::Widget* _first = nullptr, *_previous = nullptr, *_next = nullptr, *_last = nullptr;

vector<JournalPage> _journal;
unsigned _currentPage = 0;
string currentWorld;

void initJournal(RenderWindow& target)
{
    _window.startWindow(target);
    _window.loadFromFile("gui/journal.xml");

    _text = _window.widget("text");
    _number = _window.widget("number");
    _first = _window.widget("first");
    _previous = _window.widget("previous");
    _next = _window.widget("next");
    _last = _window.widget("last");
}

void addJournalEntry(string world, string questName, string newEntry)
{
    currentWorld = world;

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
        _first->addFlags(gui::Disabled);
        _previous->addFlags(gui::Disabled);
    }
    else
    {
        _first->addFlags(gui::Disabled);
        _previous->addFlags(gui::Disabled);
    }
    if (_currentPage == _journal.size() - 1)
    {
        _next->addFlags(gui::Disabled);
        _last->addFlags(gui::Disabled);
    }
    else
    {
        _next->addFlags(gui::Disabled);
        _last->addFlags(gui::Disabled);
    }

    if (_journal.size() > _currentPage)
    {
        gui::optionType o;
        o.set<textManager::PlainText>(_journal[_currentPage].plainText);
        _text->setValue(o);
    }

    textManager::PlainText number("* / *");
    number.addParameter((int)(_currentPage + 1));
    number.addParameter((int)(_journal.size()));
    gui::optionType o;
    o.set<textManager::PlainText>(number);
    _number->setValue(o);
}

void manageJournal(RenderWindow& target, Event& event)
{
    unsigned oldPage = _currentPage;

    if (_first->activated(event))
        _currentPage = 0;

    if (_previous->activated(event))
        _currentPage = max((unsigned)0, _currentPage - 1);

    if (_next->activated(event))
        _currentPage = min((unsigned)_journal.size() - 1, _currentPage + 1);

    if (_last->activated(event))
        _currentPage = _journal.size() - 1;

    if (_currentPage != oldPage)
        updateState();

    _text->activated(event);
    _window.checkTriggers();
}

void displayJournal(RenderWindow& target)
{
    _window.display(target);
}

void loadJournalFromXML(XMLHandle &handle)
{
    XMLElement *elem = handle.FirstChildElement().ToElement();

    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "entry")
        {
            string w = elem->Attribute("world");
            string q = elem->Attribute("quest");
            string v = elem->Attribute("value");

            addJournalEntry(w, q, v);
        }

        elem = elem->NextSiblingElement();
    }
}

void saveJournalToXML(XMLDocument& doc, XMLHandle& handle)
{
    XMLElement* root = handle.ToElement();

    for (auto& p : _journal)
        for (auto& pg : p.paragraphs)
        {
            XMLElement* entry = doc.NewElement("entry");

            entry->SetAttribute("world", currentWorld.c_str());
            entry->SetAttribute("quest", p.quest.c_str());
            entry->SetAttribute("value", pg.c_str());

            root->InsertEndChild(entry);
        }
}
