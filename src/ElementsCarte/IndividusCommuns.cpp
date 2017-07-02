
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

#include "../Bibliotheque/Constantes.h"
#include "../Carte/Carte.h"
#include "../Jeu/Jeu.h"
#include "ElementsCarte.h"

#include "gamedata.h"

using namespace tinyxml2;


/** FONCTIONS DE LA CLASSE Individu_Commun **/

Individu_Commun::Individu_Commun() : Individu()
{
	Classe = NULL;
	Act = -1;
}

String32& Individu_Commun::Get_Nom()
{
	return Classe->Nom;
}

int Individu_Commun::Get_Experience()
{
	return Classe->Experience;
}

Caracteristiques* Individu_Commun::Get_Caracs()
{
	return &(Classe->Caracs);
}

Statistiques* Individu_Commun::Get_Stats()
{
	return &Stats;
}

Activite* Individu_Commun::Get_Activite(string act)
{
	return Classe->Get_Activite(act);
}

bool Individu_Commun::Set_Activite(string nv)
{
	//Cette redéfinition permet d'adopter un comportement particulier lors de la mort

	bool Resultat = Individu::Set_Activite(nv);

	if (Get_Act() == MORT && Get_Num() == Get_Activite(MORT)->numberOfImages-2)
	{
		int key = CLEF_COFFRE;

		Cadavre *corpse = gamedata::currentWorld()->AjouterCadavre(Liste, position().x, position().y);
        corpse->Set_Individu(Type, Classe->corpseImageKey);

		TypeClassement = CLASSEMENT_CADAVRE;
		Diplomatie = DIPLOM_NEUTRE;

		//First, we add equipment:
		for (mapObjects::iterator i = Classe->Caracs.objects.objects.begin() ; i != Classe->Caracs.objects.objects.end() ; ++i)
		{
			corpse->objects.addObject(getStringFromLUA(i->second, "getFileName"), intToString(key));
			++key;
		}

		//Then, we create inventory and add it:
		for (list<TemplateObject>::iterator obj = Classe->inventory.begin() ; obj != Classe->inventory.end() ; ++obj)
		{
			corpse->objects.addObject(obj->fileName, intToString(key), obj->quality);
			++key;
		}

		//Finally, we create stuff according to probabilities and add it:
		for (list<TemplateObject>::iterator obj = Classe->stuff.begin() ; obj != Classe->stuff.end() ; ++obj)
		{
			if (rand()%100 < obj->probability)
			{
				corpse->objects.addObject(obj->fileName, intToString(key), obj->quality);
				++key;
			}
		}

		//If no object has been put, we can remove the corpse faster
		if (corpse->objects.objects.empty())
			corpse->Duree = 2;

		IncrementNum();
	}

	return Resultat;
}

bool Individu_Commun::angleFixed()
{
    return Classe->angleFixed;
}

/** FONCTIONS DE LA CLASSE Classe_Commune **/

Classe_Commune::Classe_Commune()
{
}

Classe_Commune::~Classe_Commune()
{
	Activites.clear();
}

void Classe_Commune::Ajouter_Activite(string Id)
{
	Activite act;
	Activites.insert(MapActivites::value_type(Id, act));
	MapActivites::iterator i = Activites.find(Id);
	i->second.Id = Id;
}

Activite* Classe_Commune::Get_Activite(string Id)
{
	MapActivites::iterator i = Activites.find(Id);
	if (i == Activites.end()) return NULL;
	return &i->second;
}

void Classe_Commune::Copie_Element(Individu_Commun *elem)
{
	elem->ActDefaut = ActDefaut;
	elem->Set_Activite(ActDefaut);
    elem->size = size;
    elem->interactionField = interactionField;
    elem->viewField = viewField;
    //Restore origins, as the copy breaks the link.
    elem->size.setOrigin(&elem->position());
    elem->interactionField.setOrigin(&elem->position());
    elem->viewField.setOrigin(&elem->position());

	elem->Diplomatie = Diplomatie;
	(*(elem->Get_Stats()))["Recuperation"] = Caracs["RecuperationMoyenne"];
    elem->lifetime = lifetime;

    if (angleFixed) elem->angle = fixedAngle;
}

void Classe_Commune::setAngleFixed(double angle)
{
    angleFixed = true;
    fixedAngle = angle;
}

void Classe_Commune::loadFromXML(XMLHandle &handle)
{
    XMLElement *elem = handle.FirstChildElement().ToElement();
    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "addImageArchiveFile")
        {
            imagePrefix = elem->Attribute("file");
            imageManager::addArchiveFile(imagePrefix);
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
            elem->QueryAttribute("strength", &Caracs["Force"]);
            elem->QueryAttribute("power", &Caracs["Puissance"]);
            elem->QueryAttribute("agility", &Caracs["Agilite"]);
            elem->QueryAttribute("intellect", &Caracs["Intelligence"]);
            //TODO
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
            if (elem->Attribute("onAttack"))
            {
                //TODO
            }
        }

        elem = elem->NextSiblingElement();
    }

    imageManager::removeArchiveFile(imagePrefix);

}
