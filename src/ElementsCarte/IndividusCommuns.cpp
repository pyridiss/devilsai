
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

#include <tinyxml2.h>

#include "ElementsCarte.h"

#include "textManager/textManager.h"

#include "imageManager/imageManager.h"

#include "devilsai-resources/manager.h"
#include "devilsai-resources/shaders.h"

using namespace tinyxml2;


/** FONCTIONS DE LA CLASSE Classe_Commune **/

Classe_Commune::Classe_Commune(string id)
  :  _displayedName(),
    Type(std::move(id)),
    definitionFile(),
    size(),
    viewField(),
    interactionField(),
    _attacks(),
    commonStats(),
    inventory(),
    _skills()
{
}

Classe_Commune::~Classe_Commune()
{
    _skills.clear();
}

void Classe_Commune::Copie_Element(Individu *elem)
{
    for (auto i : _skills)
    {
        elem->_skills.emplace(i.first, i.second);
    }

    elem->Set_Activite(_behaviors[Behaviors::Random]);
    elem->size = size;
    elem->interactionField = interactionField;
    elem->viewField = viewField;
    //Restore origins, as the copy breaks the link.
    elem->size.setOrigin(&elem->position());
    elem->interactionField.setOrigin(&elem->position());
    elem->viewField.setOrigin(&elem->position());

	elem->Diplomatie = Diplomatie;
    elem->_attributes = commonStats;
    elem->setHealthStatus(Healing, commonStats[HealingPower]);
    elem->lifetime = lifetime;

    if (angleFixed) elem->angle = fixedAngle;

    //Create objects in the item's inventory
    int key = 1;
    for (auto& d : inventory.designs)
    {
        if (rand()%100 < d.probability)
        {
            elem->inventory.addObject(d.file, "inventory" + textManager::toString(key, 2), d.quality);
            ++key;
        }
    }

    //Force the updating of attributes
    elem->currentHealthStatus(Strength, true);
}

void Classe_Commune::setAngleFixed(double angle)
{
    angleFixed = true;
    fixedAngle = angle;
}

void Classe_Commune::loadFromXML(XMLHandle &handle, const string& file)
{
    Type = handle.ToElement()->Attribute("name");
    definitionFile = file;
    _displayedName = textManager::getText("species", Type);

    string archiveFile;
    int hue = 0, saturation = 0, luminance = 0;
    Shader* shader = nullptr;

    XMLElement *elem = handle.FirstChildElement().ToElement();
    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "addImageArchiveFile")
        {
            archiveFile = elem->Attribute("file");
            imageManager::addArchiveFile(archiveFile);
        }
        if (elemName == "colorize")
        {
            Glsl::Vec3 r, g, b;
            elem->QueryAttribute("rr", &r.x);
            elem->QueryAttribute("rg", &r.y);
            elem->QueryAttribute("rb", &r.z);
            elem->QueryAttribute("gr", &g.x);
            elem->QueryAttribute("gg", &g.y);
            elem->QueryAttribute("gb", &g.z);
            elem->QueryAttribute("br", &b.x);
            elem->QueryAttribute("bg", &b.y);
            elem->QueryAttribute("bb", &b.z);
            int i = devilsai::newColorizeShaderInstance(r, g, b);
            shader = multimedia::shader("colorize", i);
        }
        if (elemName == "changeImagesHSL")
        {
            elem->QueryAttribute("hue", &hue);
            elem->QueryAttribute("saturation", &saturation);
            elem->QueryAttribute("luminance", &luminance);
        }
        if (elemName == "shape")
        {
            size.loadFromXML(elem);
        }
        if (elemName == "viewField")
        {
            viewField.loadFromXML(elem);
        }
        if (elemName == "attackField")
        {
            interactionField.loadFromXML(elem);
        }
        if (elemName == "characteristics")
        {
            commonStats.loadFromXML(elem);
        }
        if (elemName == "properties")
        {
            if (elem->Attribute("angleFixed"))
            {
                double angle = 0;
                elem->QueryAttribute("angleFixed", &angle);
                setAngleFixed(angle);
            }
            elem->QueryAttribute("diplomacy", &Diplomatie);
            elem->QueryAttribute("lifetime", &lifetime);
            elem->QueryAttribute("experience", &_experience);
            if (elem->Attribute("corpseImageKey"))
                corpseImageKey = elem->Attribute("corpseImageKey");
        }
        if (elemName == "skill")
        {
            string skillName = elem->Attribute("name");
            Skill* s = devilsai::addResource<Skill>(Type + ":" + skillName);

            XMLHandle hdl2(elem);
            s->loadFromXML(hdl2, shader, hue, saturation, luminance);

            _skills.emplace(skillName, s);
        }
        if (elemName == "skillsManagement")
        {
            if (elem->Attribute("randomBehavior"))
                _behaviors[Behaviors::Random] = elem->Attribute("randomBehavior");
            if (elem->Attribute("blocked"))
                _behaviors[Behaviors::Blocked] = elem->Attribute("blocked");
            if (elem->Attribute("hunting"))
            {
                _behaviors[Behaviors::Hunting] = elem->Attribute("hunting");
                _behaviors[Behaviors::LookingForLife] = elem->Attribute("hunting");
            }
            if (elem->Attribute("hurt"))
                _behaviors[Behaviors::Hurt] = elem->Attribute("hurt");
            if (elem->Attribute("dying"))
                _behaviors[Behaviors::Dying] = elem->Attribute("dying");
            if (elem->Attribute("attacks"))
            {
                string total = elem->Attribute("attacks");
                if (total.find(',') == string::npos)
                {
                    _attacks.push_back(total);
                }
                else
                {
                    size_t first = 0, second = total.find(',');
                    while (second != string::npos)
                    {
                        _attacks.push_back(total.substr(first, second - first));
                        first = second + 1;
                        second = total.find(',', first);
                    }
                    _attacks.push_back(total.substr(first));
                }
            }
        }
        if (elemName == "inventory")
        {
            inventory.loadFromXML(elem);
        }

        elem = elem->NextSiblingElement();
    }

    imageManager::removeArchiveFile(archiveFile);
}
