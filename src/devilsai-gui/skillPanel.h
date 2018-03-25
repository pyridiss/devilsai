
/*
    devilsai - An Action-RPG game
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

#ifndef DEVILSAI_GUI_SKILLSPANEL
#define DEVILSAI_GUI_SKILLSPANEL

namespace sf
{
    class RenderWindow;
    class Event;
}

namespace gui
{
    class Window;
}

void initSkillPanel(sf::RenderWindow& target);
void checkSkillPanelTriggers();
void manageSkillbar(RenderWindow& target, Event& event);
void manageSkillPanel(sf::RenderWindow& target, sf::Event& event);
void displaySkillbar(RenderWindow& target);
void displaySkillPanel(sf::RenderWindow& target);
const string& leftClickSkill();
const string& rightClickSkill();

#endif // DEVILSAI_GUI_SKILLSPANEL
