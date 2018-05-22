
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

#include <cmath>

#include <tinyxml2.h>

#include "tools/debug.h"
#include "tools/filesystem.h"
#include "textManager/textManager.h"

#include "imageManager/imageManager.h"

#include "ElementsCarte.h"

#include "devilsai-resources/manager.h"
#include "devilsai-resources/shaders.h"

#include "gamedata.h"

using namespace tinyxml2;


void Individu::loadFromXML(XMLHandle &handle)
{
    XMLElement *elem = handle.ToElement();

    string archiveFile;
    int hue = 0, saturation = 0, luminance = 0;
    Shader* shader = nullptr;


    if (elem->Attribute("name"))
    {
        Type = elem->Attribute("name");
        setCustomDisplayedName(textManager::getText("species", Type));
    }

    if (elem->Attribute("species"))
    {
        Type = elem->Attribute("species");
        _species = devilsai::getResource<Classe_Commune>(Type);
        if (_species == nullptr)
        {
            tools::debug::error("This class has not been loaded: " + Type, "files", __FILENAME__, __LINE__);
            return;
        }
        _species->Copie_Element(this);
    }

    double x = 0, y = 0;
    elem->QueryAttribute("x", &x);
    elem->QueryAttribute("y", &y);
    move(x, y);

    if (elem->Attribute("tag"))
        Liste = elem->Attribute("tag");

    if (elem->Attribute("loadFromDataFile"))
    {
        //Another file must be loaded to complete the unique profile.
        _extraDataFile = new string;
        *_extraDataFile = elem->Attribute("loadFromDataFile");

        //First, give this file to gamedata because it can contain species used by the unique
        gamedata::loadFromXML(tools::filesystem::dataDirectory(), *_extraDataFile);

        //We need to load this file and find the XMLElement named "uniqueData" to continue the loading
        XMLDocument file;
        file.LoadFile((tools::filesystem::dataDirectory() + *_extraDataFile).c_str());
        XMLHandle hdl(file);
        XMLHandle hdl2 = hdl.FirstChildElement().FirstChildElement("uniqueData");

        //We can call loadFromXML() with this other file
        loadFromXML(hdl2);
    }

    elem = handle.FirstChildElement().ToElement();

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

        if (elemName == "shape")            size.loadFromXML(elem);
        if (elemName == "viewField")        viewField.loadFromXML(elem);
        if (elemName == "interactionField") interactionField.loadFromXML(elem);

        if (elemName == "currentHealthStatus")
        {
            _currentHealthStatus.loadFromXML(elem);
        }
        if (elemName == "attributes")
        {
            _attributes.loadFromXML(elem);
            //Force the updating of _currentHealthStatus
            currentHealthStatus(Life, true);
        }

        if (elemName == "properties")
        {
            elem->QueryAttribute("id", &Id);
            elem->QueryAttribute("lifetime", &lifetime);
            elem->QueryAttribute("ignoreCollision", &ignoreCollision);
            elem->QueryAttribute("classement", &TypeClassement);
            elem->QueryAttribute("diplomacy", &Diplomatie);
            elem->QueryAttribute("fixedHealing", &RecuperationFixe);
            elem->QueryAttribute("maximumEnergy", &EnergieMax);
            elem->QueryAttribute("experience", &_experience);
            if (elem->Attribute("corpseImageKey"))
                corpseImageKey() = elem->Attribute("corpseImageKey");
            if (int o = 0 ; elem->QueryAttribute("owner", &o) == XML_SUCCESS)
                _owner.set<int>(o);
        }
        if (elemName == "skill")
        {
            string skillName = elem->Attribute("name");

            Skill *s = devilsai::getResource<Skill>(Type + ":" + skillName);

            if (s == nullptr)
            {
                s = devilsai::addResource<Skill>(Type + ":" + skillName);

                XMLHandle hdl2(elem);
                s->loadFromXML(hdl2, shader, hue, saturation, luminance);
            }

            SkillProperties p(s);

            elem->QueryAttribute("level", &p.level);
            elem->QueryAttribute("unavailability", &p.unavailability);

            _skills.emplace(skillName, std::move(p));
        }
        if (elemName == "skillsManagement")
        {
            createCustomBehaviors();
            if (elem->Attribute("randomBehavior"))
                behavior(Behaviors::Random) = elem->Attribute("randomBehavior");
            if (elem->Attribute("blocked"))
                behavior(Behaviors::Blocked) = elem->Attribute("blocked");
            if (elem->Attribute("hunting"))
            {
                behavior(Behaviors::Hunting) = elem->Attribute("hunting");
                behavior(Behaviors::LookingForLife) = elem->Attribute("hunting");
            }
            if (elem->Attribute("hurt"))
                behavior(Behaviors::Hurt) = elem->Attribute("hurt");
            if (elem->Attribute("dying"))
                behavior(Behaviors::Dying) = elem->Attribute("dying");
            if (elem->Attribute("attacks"))
            {
                string total = elem->Attribute("attacks");
                if (total.find(',') == string::npos)
                {
                    attacks().push_back(total);
                }
                else
                {
                    size_t first = 0, second = total.find(',');
                    while (second != string::npos)
                    {
                        attacks().push_back(total.substr(first, second - first));
                        first = second + 1;
                        second = total.find(',', first);
                    }
                    attacks().push_back(total.substr(first));
                }
            }

            Set_Activite(behavior(Behaviors::Random));
        }
        if (elemName == "inventory")
        {
            inventory.loadFromXML(elem);
        }

        elem = elem->NextSiblingElement();
    }

    imageManager::removeArchiveFile(archiveFile);
}

void Individu::saveToXML(XMLDocument& doc, XMLHandle& handle)
{
    XMLElement* root = handle.ToElement();

    XMLElement* unique = doc.NewElement("individual");

    if (_extraDataFile != nullptr)
        unique->SetAttribute("loadFromDataFile", _extraDataFile->c_str());

    if (_species != nullptr)
        unique->SetAttribute("species", Type.c_str());
    else
        unique->SetAttribute("name", Type.c_str());

    unique->SetAttribute("x", position().x);
    unique->SetAttribute("y", position().y);
    unique->SetAttribute("tag", Liste.c_str());

    XMLElement* stats = doc.NewElement("currentHealthStatus");
    unique->InsertEndChild(stats);
    XMLHandle statsHandle(stats);
    _currentHealthStatus.saveToXML(doc, statsHandle);

    XMLElement* attributesElem = doc.NewElement("attributes");
    unique->InsertEndChild(attributesElem);
    XMLHandle attributesHandle(attributesElem);
    _attributes.saveToXML(doc, attributesHandle);

    XMLElement* properties = doc.NewElement("properties");
    properties->SetAttribute("id", Id);

    if (lifetime != -1)
        properties->SetAttribute("lifetime", lifetime);

    if (ignoreCollision)
        properties->SetAttribute("ignoreCollision", ignoreCollision);
    properties->SetAttribute("classement", TypeClassement);
    properties->SetAttribute("diplomacy", Diplomatie);

    if (RecuperationFixe)
        properties->SetAttribute("fixedHealing", RecuperationFixe);

    if (EnergieMax)
        properties->SetAttribute("maximumEnergy", EnergieMax);

    properties->SetAttribute("experience", _experience);

    if (_corpseImageKey != nullptr)
        properties->SetAttribute("corpseImageKey", _corpseImageKey->c_str());

    if (owner() != nullptr)
        properties->SetAttribute("owner", owner()->Id);

    unique->InsertEndChild(properties);

    for (auto i : _skills)
    {
        XMLElement* skill = doc.NewElement("skill");
        skill->SetAttribute("name", i.first.c_str());
        skill->SetAttribute("level", i.second.level);
        skill->SetAttribute("unavailability", i.second.unavailability);
        unique->InsertEndChild(skill);
    }

    XMLHandle hdl(unique);
    inventory.saveToXML(doc, hdl);

    root->InsertEndChild(unique);
}
