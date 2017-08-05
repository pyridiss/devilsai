
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2017  Quentin Henriet

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

#include "gui/window.h"
#include "imageManager/imageManager.h"

#include "Attributs/Attributs.h"
#include "ElementsCarte/ElementsCarte.h"
#include "Bibliotheque/Bibliotheque.h"
#include "Jeu/Jeu.h"

#include "gamedata.h"

#include "skillbarManager.h"

void manageSkillbar(gui::Window& window, RenderWindow& target, Event& event)
{
}

void displaySkillbar(gui::Window& window, RenderWindow& target)
{
    window.display(target);

    const auto& skills = window.getWidgets();

    for (const auto& skill : skills)
    {
        string skillName = skill.second->value();
        if (!skillName.empty())
        {
            imageManager::display(target, "skills", skillName, skill.second->getXTopLeft(), skill.second->getYTopLeft());
        }
    }
}
