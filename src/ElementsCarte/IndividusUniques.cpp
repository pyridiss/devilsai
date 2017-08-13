
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
#include "tools/timeManager.h"
#include "tools/textManager.h"

#include "imageManager/imageManager.h"

#include "../Bibliotheque/Constantes.h"
#include "../Carte/Carte.h"
#include "../Jeu/Jeu.h"
#include "ElementsCarte.h"

#include "gamedata.h"

using namespace tinyxml2;

/** FONCTIONS DE LA CLASSE Individu_Unique **/

Individu_Unique::Individu_Unique() : Individu()
{
	Act = -1;
}

Individu_Unique::~Individu_Unique()
{
	Activites.clear();
}

void Individu_Unique::Ajouter_Activite(string Id)
{
	Activite act;
	Activites.insert(MapActivites::value_type(Id, act));
	MapActivites::iterator i = Activites.find(Id);
	i->second.Id = Id;
}

String32& Individu_Unique::Get_Nom()
{
	return Nom;
}

int Individu_Unique::Get_Experience()
{
	return Experience;
}

Caracteristiques& Individu_Unique::attributes()
{
	return Caracs;
}

Activite* Individu_Unique::Get_Activite(string act)
{
	MapActivites::iterator i = Activites.find(act);
	if (i == Activites.end()) return NULL;
	return &i->second;
}

void Individu_Unique::modifyHealthStatus(Statistiques::Attribute a, double value)
{
    if (RecuperationFixe && a == Statistiques::Healing) return;

    Individu::modifyHealthStatus(a, value);
}

bool Individu_Unique::Set_Activite(string nv)
{
	//Cette redéfinition permet d'adopter un comportement particulier lors de la mort

	bool Resultat = Individu::Set_Activite(nv);

	if (Get_Act() == MORT)
	{
		setHealthStatus(Statistiques::Life, 0);
		setHealthStatus(Statistiques::Energy, 0);
		setHealthStatus(Statistiques::Healing, 0);
	}
	if (Get_Act() == MORT && Get_Num() == Get_Activite(MORT)->numberOfImages-2)
	{
		int key = 1;

        Coffre *corpse = gamedata::currentWorld()->AjouterCoffre("storage-boxes", position().x, position().y);
        corpse->Set_Individu(Type, corpseImageKey);
        corpse->size.circle(tools::math::Vector2d(0, 0), 20);
        corpse->size.setOrigin(&corpse->position());

		TypeClassement = CLASSEMENT_CADAVRE;
		Diplomatie = DIPLOM_NEUTRE;

		for (mapObjects::iterator i = inventory.objects.begin() ; i != inventory.objects.end() ; ++i)
		{
			corpse->objects.addObject(getStringFromLUA(i->second, "getFileName"), "storagebox" + intToString(key, 2));
			++key;
		}

        corpse->close();

		IncrementNum();
	}

	return Resultat;
}

bool Individu_Unique::angleFixed()
{
    return false;
}

void Individu_Unique::loadFromXML(XMLHandle &handle)
{
    XMLElement *elem = handle.ToElement();

    //May be needed if loadFromDataFile is set.
    XMLDocument file;

    if (elem->Attribute("name"))
    {
        Type = elem->Attribute("name");
        Nom = tools::textManager::getText("species", Type);
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
        dataFile = elem->Attribute("loadFromDataFile");

        //First, give this file to gamedata because it can contain species used by the unique
        gamedata::loadFromXML(tools::filesystem::dataDirectory(), dataFile);

        //We need to load this file and find the XMLElement named "uniqueData" to continue the loading
        file.LoadFile((tools::filesystem::dataDirectory() + dataFile).c_str());

        XMLHandle hdl(file);
        elem = hdl.FirstChildElement().FirstChildElement("uniqueData").FirstChildElement().ToElement();

        if (elem == nullptr)
        {
            tools::debug::error("Cannot find unique data", "files", __FILENAME__, __LINE__);
            return;
        }
    }
    else
    {
        //No other file needed, we can continue with this file
        elem = handle.FirstChildElement().ToElement();
    }

    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "addImageArchiveFile")
        {
            imagePrefix = elem->Attribute("file");
            imageManager::addArchiveFile(imagePrefix);
        }

        if (elemName == "shape")            size.loadFromXML(elem);
        if (elemName == "viewField")        viewField.loadFromXML(elem);
        if (elemName == "attackField")      interactionField.loadFromXML(elem);

        if (elemName == "characteristics")
        {
            Caracs.loadFromXML(elem);
            //Force the updating of attributes
            currentHealthStatus(Caracteristiques::Strength, true);
        }
        if (elemName == "statistics")       Stats.loadFromXML(elem);

        if (elemName == "properties")
        {
            elem->QueryAttribute("id", &Id);
            elem->QueryAttribute("lifetime", &lifetime);
            elem->QueryAttribute("ignoreCollision", &ignoreCollision);
            elem->QueryAttribute("classement", &TypeClassement);
            elem->QueryAttribute("diplomacy", &Diplomatie);
            elem->QueryAttribute("fixedHealing", &RecuperationFixe);
            elem->QueryAttribute("maximumEnergy", &EnergieMax);
            elem->QueryAttribute("experience", &Experience);
            if (elem->Attribute("corpseImageKey"))
                corpseImageKey = elem->Attribute("corpseImageKey");
        }
        if (elemName == "skill")
        {
            string skillName = elem->Attribute("name");
            Ajouter_Activite(skillName);
            Activite *skill = Get_Activite(skillName);

            XMLHandle hdl2(elem);
            skill->loadFromXML(hdl2, this);
        }
        if (elemName == "skillsManagement")
        {
            ActDefaut = elem->Attribute("default");
            Set_Activite(ActDefaut);

            if (elem->Attribute("onAttack"))
            {
                //TODO
            }
        }
        if (elemName == "inventory")
        {
            inventory.loadFromXML(elem);
        }

        elem = elem->NextSiblingElement();
    }

    imageManager::removeArchiveFile(imagePrefix);
}

void Individu_Unique::saveToXML(XMLDocument& doc, XMLHandle& handle)
{
    XMLElement* root = handle.ToElement();

    XMLElement* unique = doc.NewElement("unique");
    unique->SetAttribute("loadFromDataFile", dataFile.c_str());
    unique->SetAttribute("name", Type.c_str());
    unique->SetAttribute("x", position().x);
    unique->SetAttribute("y", position().y);
    unique->SetAttribute("tag", Liste.c_str());

    XMLElement* characteristicsElem = doc.NewElement("characteristics");
    unique->InsertEndChild(characteristicsElem);
    XMLHandle characteristicsHandle(characteristicsElem);
    Caracs.saveToXML(doc, characteristicsHandle);

    XMLElement* statisticsElem = doc.NewElement("statistics");
    unique->InsertEndChild(statisticsElem);
    XMLHandle statisticsHandle(statisticsElem);
    Stats.saveToXML(doc, statisticsHandle);

    XMLElement* properties = doc.NewElement("properties");
    properties->SetAttribute("id", Id);
    properties->SetAttribute("lifetime", lifetime);
    properties->SetAttribute("ignoreCollision", ignoreCollision);
    properties->SetAttribute("classement", TypeClassement);
    properties->SetAttribute("diplomacy", Diplomatie);
    properties->SetAttribute("fixedHealing", RecuperationFixe);
    properties->SetAttribute("maximumEnergy", EnergieMax);
    properties->SetAttribute("experience", Experience);
    properties->SetAttribute("corpseImageKey", corpseImageKey.c_str());
    unique->InsertEndChild(properties);

    XMLHandle hdl(unique);
    inventory.saveToXML(doc, hdl);

    root->InsertEndChild(unique);
}
