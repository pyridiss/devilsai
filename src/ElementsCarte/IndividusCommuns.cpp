
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

#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/Bibliotheque.h"
#include "../Carte/Carte.h"
#include "ElementsCarte.h"

#include "tools/debug.h"
#include "tools/textManager.h"

#include "imageManager/imageManager.h"

#include "gamedata.h"

using namespace tinyxml2;


/** FONCTIONS DE LA CLASSE Individu_Commun **/

Individu_Commun::Individu_Commun() : Individu()
{
	Classe = NULL;
}

String32& Individu_Commun::Get_Nom()
{
	return Classe->Nom;
}

int Individu_Commun::Get_Experience()
{
	return Classe->Experience;
}

Caracteristiques& Individu_Commun::attributes()
{
	return Classe->Caracs;
}

Activite* Individu_Commun::Get_Activite(string act)
{
	return Classe->Get_Activite(act);
}

string& Individu_Commun::behavior(Behaviors b)
{
    return Classe->_behaviors[b];
}

vector<string>& Individu_Commun::attacks()
{
    return Classe->_attacks;
}

bool Individu_Commun::Set_Activite(string nv)
{
	//Cette redéfinition permet d'adopter un comportement particulier lors de la mort

	bool Resultat = Individu::Set_Activite(nv);

    if (Get_Act() == behavior(Behaviors::Dying) && Get_Num() == Get_Activite(behavior(Behaviors::Dying))->numberOfImages-2)
	{
		int key = 1;

        Coffre *corpse = gamedata::currentWorld()->AjouterCoffre("storage-boxes", position().x, position().y);
        corpse->Set_Individu(Type, Classe->corpseImageKey);
        corpse->size.circle(tools::math::Vector2d(0, 0), 20);
        corpse->size.setOrigin(&corpse->position());

		TypeClassement = CLASSEMENT_CADAVRE;
		Diplomatie = DIPLOM_NEUTRE;

		//First, we add equipment:
		for (mapObjects::iterator i = Classe->inventory.objects.begin() ; i != Classe->inventory.objects.end() ; ++i)
		{
			corpse->objects.addObject(getStringFromLUA(i->second, "getFileName"), "storagebox" + intToString(key, 2));
			++key;
		}

		//Finally, we create stuff according to probabilities and add it:
		for (auto& d : Classe->inventory.designs)
		{
			if (rand()%100 < d.probability)
			{
				corpse->objects.addObject(d.file, "storagebox" + intToString(key, 2), d.quality);
				++key;
			}
		}

        corpse->close();

		IncrementNum();
	}

	return Resultat;
}

bool Individu_Commun::angleFixed()
{
    return Classe->angleFixed;
}

void Individu_Commun::loadFromXML(XMLHandle &handle)
{
    XMLElement *elem = handle.ToElement();

    if (elem->Attribute("species"))
    {
        Type = elem->Attribute("species");
        Classe = gamedata::species(Type);
        if (Classe == nullptr)
        {
            tools::debug::error("This class has not been loaded: " + Type, "files", __FILENAME__, __LINE__);
            return;
        }
        Classe->Copie_Element(this);
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

        if (elemName == "properties")
        {
            elem->QueryAttribute("id", &Id);
            elem->QueryAttribute("lifetime", &lifetime);
            elem->QueryAttribute("ignoreCollision", &ignoreCollision);
            elem->QueryAttribute("classement", &TypeClassement);
        }
        if (elemName == "statistics")
        {
            Stats.loadFromXML(elem);
        }

        elem = elem->NextSiblingElement();
    }
}

void Individu_Commun::saveToXML(XMLDocument& doc, XMLHandle& handle)
{
    XMLElement* root = handle.ToElement();

    XMLElement* individual = doc.NewElement("individual");
    individual->SetAttribute("species", Type.c_str());
    individual->SetAttribute("x", position().x);
    individual->SetAttribute("y", position().y);
    individual->SetAttribute("tag", Liste.c_str());

    XMLElement* properties = doc.NewElement("properties");
    properties->SetAttribute("id", Id);
    properties->SetAttribute("lifetime", lifetime);
    properties->SetAttribute("ignoreCollision", ignoreCollision);
    properties->SetAttribute("classement", TypeClassement);
    individual->InsertEndChild(properties);

    XMLElement* stats = doc.NewElement("statistics");
    individual->InsertEndChild(stats);
    XMLHandle statsHandle(stats);
    Stats.saveToXML(doc, statsHandle);

    root->InsertEndChild(individual);
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
    elem->Set_Activite(_behaviors[Behaviors::Random]);
    elem->size = size;
    elem->interactionField = interactionField;
    elem->viewField = viewField;
    //Restore origins, as the copy breaks the link.
    elem->size.setOrigin(&elem->position());
    elem->interactionField.setOrigin(&elem->position());
    elem->viewField.setOrigin(&elem->position());

	elem->Diplomatie = Diplomatie;
    elem->setHealthStatus(Statistiques::Healing, Caracs[Caracteristiques::HealingPower]);
    elem->lifetime = lifetime;

    if (angleFixed) elem->angle = fixedAngle;

    //Force the updating of attributes
    elem->currentHealthStatus(Caracteristiques::Strength, true);
}

void Classe_Commune::setAngleFixed(double angle)
{
    angleFixed = true;
    fixedAngle = angle;
}

void Classe_Commune::loadFromXML(XMLHandle &handle)
{
    Type = handle.ToElement()->Attribute("name");
    Nom = tools::textManager::getText("species", Type);

    string archiveFile;

    XMLElement *elem = handle.FirstChildElement().ToElement();
    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "addImageArchiveFile")
        {
            archiveFile = elem->Attribute("file");
            imageManager::addArchiveFile(archiveFile);
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
            Caracs.loadFromXML(elem);
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
            skill->loadFromXML(hdl2);
        }
        if (elemName == "skillsManagement")
        {
            if (elem->Attribute("randomBehavior"))
                _behaviors[Behaviors::Random] = elem->Attribute("randomBehavior");
            if (elem->Attribute("blocked"))
                _behaviors[Behaviors::Blocked] = elem->Attribute("blocked");
            if (elem->Attribute("hunting"))
                _behaviors[Behaviors::Hunting] = elem->Attribute("hunting");
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
