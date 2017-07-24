
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
// 	Equips.clear();
// 	for (int c = 0 ; c < NOMBRE_COMPETENCES ; ++c)
// 		CompetencesRapides[c] = NULL;
	Activites.clear();
// 	for (MapPointeursCompetences::iterator i = Competences.begin() ; i != Competences.end() ; ++i)
// 		delete i->second;
}

void Individu_Unique::Ajouter_Activite(string Id)
{
	Activite act;
	Activites.insert(MapActivites::value_type(Id, act));
	MapActivites::iterator i = Activites.find(Id);
	i->second.Id = Id;
}

void Individu_Unique::Ajouter_EmplacementEquip(int x, int y, int w, int h, string Categorie, string Type, string Classe)
{
	EmplacementEquipement emp;
	EmplacementsEquip.push_back(std::move(emp));
	EmplacementsEquip.back().Set(x, y, w, h);
	EmplacementsEquip.back().CategorieObjet = Categorie;
	EmplacementsEquip.back().TypeObjet = Type;
	EmplacementsEquip.back().ClasseObjet = Classe;
}

String32& Individu_Unique::Get_Nom()
{
	return Nom;
}

int Individu_Unique::Get_Experience()
{
	return Experience;
}

Caracteristiques* Individu_Unique::Get_Caracs()
{
	return &Caracs;
}

Statistiques* Individu_Unique::Get_Stats()
{
	return &Stats;
}

Activite* Individu_Unique::Get_Activite(string act)
{
	MapActivites::iterator i = Activites.find(act);
	if (i == Activites.end()) return NULL;
	return &i->second;
}

void Individu_Unique::Gestion_Recuperation()
{
	//Cette fonction s'occupe de la récupération, de l'énergie, et de la durée de vie des objets
	if (!RecuperationFixe) Individu::Gestion_Recuperation();
	else
	{
		if (get("Recuperation") > 0)
		{
			if (100*buf_rec <= 3*get("Recuperation"))
			{
				Lag_Vitalite(1);
				if (get("Recuperation") > 80) Lag_Vitalite(3);
				if (get("Recuperation") > 90) Lag_Vitalite(6);
				if (get("Recuperation") > 95) Lag_Vitalite(6);
				Lag_Energie(1);
			}
		}
		if (get("Recuperation") < 0)
		{
			if (100*buf_rec <= -3*get("Recuperation"))
			{
				Lag_Vitalite(-1);
				Lag_Energie(-1);
			}
		}
	}

	if (EnergieMax) Stats["Energie"] = 1000;

	//Diminue la durée de vie des objets utilisés -- Should maybe be placed in LUA scripts
	for (mapObjects::iterator i = Get_Caracs()->objects.objects.begin() ; i != Get_Caracs()->objects.objects.end() ; ++i)
	{
		if (getStringFromLUA(i->second, "getIdEmplacement") == i->first)
		{
			if (getDoubleFromLUA(i->second, "getDuree") > 0)
			{
                setDoubleToLUA(i->second, "setDuree", getDoubleFromLUA(i->second, "getDuree") - tools::timeManager::I(1));
				if (getDoubleFromLUA(i->second, "getDuree") <= 0)
				{
					lua_State *j = i->second;
					i = Get_Caracs()->objects.objects.erase(i);
					Get_Caracs()->objects.deleteObject(j);
					continue;
				}
			}
		}
	}
}

void Individu_Unique::Lag_Recuperation(float lag)
{
	if (!RecuperationFixe) Individu::Lag_Recuperation(lag);
}

float Individu_Unique::get(string field)
{
	float& valueFloat = (*Get_Stats())[field];
	if (valueFloat != Jeu.floatNotFound)
		return valueFloat;

	int valueInt = (*Get_Caracs())[field];
	if (valueInt != Jeu.intNotFound)
	{
		if (field != "RecuperationMoyenne") valueInt *= 1./2. * (1. + 1.2*get("Vitalite")/1000.);

		pair<int, int> addedChar = Get_Caracs()->getFromObjectsAndSkills(field);
		valueInt += addedChar.first;

		return valueInt + addedChar.second*valueInt/100.;
	}

	return Jeu.floatNotFound;
}

int Individu_Unique::Get_Vitesse(string act)
{
	int Total = Get_Activite(act)->speed;

	//Workaround to make 'speedpotion' work. To be changed!

	string speedType = "";

    if (act == "3") speedType = "getVitesseCourse";
    else if (act == "6") speedType = "getVitesseBlesse";
    else if (act == "101" || act == "301") speedType = "getVitesseAttaque";

	if (speedType != "")
	{
		for (auto& i : Get_Caracs()->objects.objects)
			Total += getIntFromLUA(i.second, speedType);

		for (auto& i : Get_Caracs()->skills)
			Total += getIntFromLUA(i.second, speedType);
	}

	return Total;
}

bool Individu_Unique::Set_Activite(string nv)
{
	//Cette redéfinition permet d'adopter un comportement particulier lors de la mort

	bool Resultat = Individu::Set_Activite(nv);

	if (Get_Act() == MORT)
	{
		Set_Vitalite(0);
		Set_Energie(0);
		Set_Recuperation(0);
	}
	if (Get_Act() == MORT && Get_Num() == Get_Activite(MORT)->numberOfImages-2)
	{
		int key = CLEF_COFFRE;

		Cadavre *corpse = gamedata::currentWorld()->AjouterCadavre(Liste, position().x, position().y);
        corpse->Set_Individu(Type, corpseImageKey);

		TypeClassement = CLASSEMENT_CADAVRE;
		Diplomatie = DIPLOM_NEUTRE;

		for (mapObjects::iterator i = Get_Caracs()->objects.objects.begin() ; i != Get_Caracs()->objects.objects.end() ; ++i)
		{
			corpse->objects.addObject(getStringFromLUA(i->second, "getFileName"), intToString(key));
			++key;
		}

		//If no object has been put, we can remove the corpse faster
		if (corpse->objects.objects.empty())
			corpse->lifetime = 2;

		IncrementNum();
	}

	return Resultat;
}

/* La fonction Individu_Unique::Load() est développée dans le fichier ChargementElements.cpp */

bool Individu_Unique::angleFixed()
{
    return false;
}

void Individu_Unique::loadFromXML(XMLHandle &handle)
{
    XMLElement *elem = handle.ToElement();

    if (elem->Attribute("name"))
    {
        Type = elem->Attribute("name");
        Nom = tools::textManager::getText("species", Type);
    }

    if (elem->Attribute("loadFromDataFile"))
    {
        dataFile = elem->Attribute("loadFromDataFile");
        gamedata::loadFromXML(tools::filesystem::dataDirectory(), dataFile);
    }

    double x = 0, y = 0;
    elem->QueryAttribute("x", &x);
    elem->QueryAttribute("y", &y);
    move(x, y);

    if (elem->Attribute("tag"))
        Liste = elem->Attribute("tag");

    elem = handle.FirstChildElement().ToElement();
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

        if (elemName == "characteristics")  Caracs.loadFromXML(elem);
        if (elemName == "statistics")       Stats.loadFromXML(elem);

        if (elemName == "properties")
        {
            elem->QueryAttribute("id", &Id);
            elem->QueryAttribute("lifetime", &lifetime);
            elem->QueryAttribute("ignoreCollision", &ignoreCollision);
            elem->QueryAttribute("classement", &TypeClassement);
            elem->QueryAttribute("diplomacy", &Diplomatie);
            if (elem->Attribute("corpseImageKey"))
                corpseImageKey = elem->Attribute("corpseImageKey");
        }
        if (elemName == "skill")
        {
            string skillName = elem->Attribute("name");
            Ajouter_Activite(skillName);
            Activite *skill = Get_Activite(skillName);

            XMLHandle hdl2(elem);
            skill->loadFromXML(hdl2);
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
        if (elemName == "EmplacementEquipement") //TODO: update this!
        {
            EmplacementEquipement emp;
            int x = 0, y = 0, w = 0, h= 0;
            elem->QueryAttribute("x", &x);
            elem->QueryAttribute("y", &y);
            elem->QueryAttribute("w", &w);
            elem->QueryAttribute("h", &h);
            emp.Set(x, y, w, h);
            emp.CategorieObjet = elem->Attribute("category");
            emp.TypeObjet = elem->Attribute("type");
            emp.ClasseObjet = elem->Attribute("class");
            EmplacementsEquip.push_back(std::move(emp));
        }
        if (elemName == "Equipement") //TODO: Update this!
        {
            string numero = elem->Attribute("number");
            string IdEmplacement = elem->Attribute("IdEmplacement");

            Get_Caracs()->objects.addObject(numero, IdEmplacement);
        }
        if (elemName == "inventory")
        {
            Get_Caracs()->objects.loadFromXML(elem);
        }
        if (elemName == "SKILL") //TODO: Remove this!
        {
            string numero = elem->Attribute("SKILL");
            Get_Caracs()->addSkill(numero, this);
        }
        if (elemName == "DEFAUT_CTRL") //TODO: Remove this!
        {
            string numero = elem->Attribute("DEFAUT_CTRL");
            mapSkills::iterator i = Get_Caracs()->skills.find(numero);
            if (i != Get_Caracs()->skills.end()) skillLinks[COMPETENCE_CTRL] = i->second;
        }
        if (elemName == "DEFAUT_SHIFT") //TODO: Remove this!
        {
            string numero = elem->Attribute("DEFAUT_SHIFT");
            mapSkills::iterator i = Get_Caracs()->skills.find(numero);
            if (i != Get_Caracs()->skills.end()) skillLinks[COMPETENCE_SHIFT] = i->second;
        }
        if (elemName == "DEFAUT_TAB") //TODO: Remove this!
        {
            string numero = elem->Attribute("DEFAUT_TAB");
            mapSkills::iterator i = Get_Caracs()->skills.find(numero);
            if (i != Get_Caracs()->skills.end()) skillLinks[COMPETENCE_TAB] = i->second;
        }
        if (elemName == "DEFAUT_SPACE") //TODO: Remove this!
        {
            string numero = elem->Attribute("DEFAUT_SPACE");
            mapSkills::iterator i = Get_Caracs()->skills.find(numero);
            if (i != Get_Caracs()->skills.end()) skillLinks[COMPETENCE_SPACE] = i->second;
        }

        elem = elem->NextSiblingElement();
    }

    imageManager::removeArchiveFile(imagePrefix);
}

void Individu_Unique::saveToXML(XMLDocument& doc, XMLHandle& handle)
{
}
