
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

#include <string>
#include <list>

#include "textManager/plainText.h"

using namespace std;

namespace tinyxml2{
    class XMLElement;
    class XMLHandle;
    class XMLDocument;
}

namespace sf
{
    class RenderWindow;
    class Event;
}

namespace gui
{
    class Window;
}

class JournalPage
{
    public:
        string quest;
        list<string> paragraphs;
        textManager::PlainText plainText;

    public:
        JournalPage()
          : quest(),
            paragraphs(),
            plainText()
        {
        }

    public:
        void addParagraph(string newParagraph);
};

void initJournal(RenderWindow& target);
void addJournalEntry(string world, string questName, string newEntry);
void manageJournal(RenderWindow& target, Event& event);
void displayJournal(RenderWindow& target);
void loadJournalFromXML(tinyxml2::XMLHandle &handle);
void saveJournalToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);
