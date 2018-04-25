
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

#include "devilsai-gui/skillPanel.h"

#include "tools/math.h"
#include "imageManager/imageManager.h"
#include "gui/style.h"
#include "gui/window.h"

#include "ElementsCarte/ElementsCarte.h"
#include "gamedata.h"

gui::Window SkillPanel_Gui;
gui::Window SkillPanel_Bar;
string SkillPanel_SelectedSkillName;
string SkillPanel_SelectedSkillObject;

void initSkillPanel(RenderWindow& target)
{
    SkillPanel_Gui.startWindow(target);
    SkillPanel_Gui.loadFromFile("gui/skill-panel.xml");
    SkillPanel_Bar.startWindow(target);
    SkillPanel_Bar.loadFromFile("gui/ingame-skillbar.xml");
}

void checkSkillPanelTriggers()
{
    SkillPanel_Gui.checkTriggers();
    SkillPanel_Bar.checkTriggers();
}

void manageSkillbar(Event& event)
{
    if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Button::Right)
    {
        for (const auto& slot : SkillPanel_Bar.getWidgets())
        {
            if (slot.second->activated(event))
                slot.second->addEmbeddedData<string>("value", string());
        }
    }

    SkillPanel_Bar.manage(event);
}

void manageSkillPanel(Event& event)
{
    for (const auto& slot : SkillPanel_Gui.getWidgets())
    {
        const string& skillName = slot.second->embeddedData<string>("skill");
        if (skillName.empty())
            continue;

        Individu::SkillAccess currentSkill = gamedata::player()->skill(skillName);

        if (currentSkill.none()) continue;

        if (currentSkill.level() != 0)
        {
            slot.second->show();
            const string& relatedObject = slot.second->embeddedData<string>("related-object");
            if (!relatedObject.empty())
            {
                if (gamedata::player()->inventory.quantityOf(relatedObject) == 0)
                    slot.second->hide();
            }
        }
        else slot.second->hide();

        if (slot.second->activated(event))
        {
            if (skillName == SkillPanel_SelectedSkillName)
            {
                SkillPanel_SelectedSkillName.clear();
                SkillPanel_SelectedSkillObject.clear();
            }
            else
            {
                SkillPanel_SelectedSkillName = skillName;
                SkillPanel_SelectedSkillObject = slot.second->embeddedData<string>("related-object");
            }
        }
    }

    if (!SkillPanel_SelectedSkillName.empty())
    {
        for (const auto& slot : SkillPanel_Bar.getWidgets())
        {
            if (slot.second->activated(event))
            {
                slot.second->addEmbeddedData<string>("value", SkillPanel_SelectedSkillName);
                slot.second->addEmbeddedData<string>("related-object", SkillPanel_SelectedSkillObject);
                SkillPanel_SelectedSkillName.clear();
                SkillPanel_SelectedSkillObject.clear();
            }
        }
    }

    manageSkillbar(event);
}

void displaySkillbar(RenderWindow& target)
{
    SkillPanel_Bar.display(target);

    for (const auto& slot : SkillPanel_Bar.getWidgets())
    {
        const string& skillName = slot.second->embeddedData<string>("value");
        if (!skillName.empty())
        {
            const Shader* shader = nullptr;

            Individu::SkillAccess currentSkill = gamedata::player()->skill(skillName);

            if (!currentSkill.none() && currentSkill.unavailability() > 0)
                shader = gui::style::getContrastShader(0.1, 0.1, 0.1);

            imageManager::display(target, tools::hash("skills"), skillName, slot.second->left(), slot.second->top(), false, shader);
        }
        const string& relatedObject = slot.second->embeddedData<string>("related-object");
        if (!relatedObject.empty())
        {
            textManager::RichText number;
            number.setSize(50, 0);
            number.setDefaultProperties("liberation", 12, Color(255, 255, 255));
            number.create(gamedata::player()->inventory.quantityOf(relatedObject));
            number.displayFullText(target, slot.second->left() + 30, slot.second->top() + 30);
        }
    }
}

void displaySkillPanel(RenderWindow& target)
{
    SkillPanel_Gui.display(target);

    for (const auto& slot : SkillPanel_Gui.getWidgets())
    {
        if (slot.second->hasFlags(gui::Hidden))
            continue;

        const string& skillName = slot.second->embeddedData<string>("skill");
        if (!skillName.empty())
        {
            imageManager::display(target, tools::hash("skills"), skillName, slot.second->left(), slot.second->top());
        }
        const string& relatedObject = slot.second->embeddedData<string>("related-object");
        if (!relatedObject.empty())
        {
            textManager::RichText number;
            number.setSize(50, 0);
            number.setDefaultProperties("liberation", 12, Color(255, 255, 255));
            number.create(gamedata::player()->inventory.quantityOf(relatedObject));
            number.displayFullText(target, slot.second->left() + 30, slot.second->top() + 30);
        }
    }

    displaySkillbar(target);

    if (!SkillPanel_SelectedSkillName.empty())
    {
        imageManager::display(target, tools::hash("skills"), SkillPanel_SelectedSkillName, gui::mousePosition().x, gui::mousePosition().y, true);
    }
}

const string& leftClickSkill()
{
    return SkillPanel_Bar.widget("left-click")->embeddedData<string>("value");
}

const string& rightClickSkill()
{
    return SkillPanel_Bar.widget("right-click")->embeddedData<string>("value");
}
