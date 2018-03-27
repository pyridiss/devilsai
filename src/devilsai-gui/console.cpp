
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

#include "gui/textWidget.h"
#include "gui/window.h"

#include "devilsai-gui/console.h"

gui::Window Console_Gui;
gui::TextWidget* Console_TextWidget = nullptr;

textManager::PlainText Console_PlainText;


void initConsole(RenderWindow& target)
{
    Console_Gui.startWindow(target);
    Console_Gui.loadFromFile("gui/console.xml");

    Console_TextWidget = dynamic_cast<gui::TextWidget*>(Console_Gui.widget("text"));
}

void addConsoleEntry(const textManager::PlainText& newEntry)
{
    Console_PlainText += " @n";
    Console_PlainText += newEntry;
    gui::optionType v;
    v.set<textManager::PlainText>(Console_PlainText);
    Console_TextWidget->setValue(v);
    Console_TextWidget->setVerticalScrolling(100);
}

void manageConsole(RenderWindow& target, Event& event)
{
    Console_TextWidget->activated(event);
    Console_Gui.manage(event);
    Console_Gui.checkTriggers();
}

void displayConsole(RenderWindow& target)
{
    Console_Gui.display(target);
}
