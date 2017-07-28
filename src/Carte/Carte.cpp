
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

#include <lua5.2/lua.hpp>
#include <tinyxml2.h>

#include "tools/filesystem.h"
#include "tools/textManager.h"

#include "imageManager/imageManager.h"
#include "musicManager/musicManager.h"

#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/Bibliotheque.h"
#include "Carte.h"
#include "../ElementsCarte/ElementsCarte.h"

#include "gamedata.h"

using namespace tinyxml2;

void Load_Carte(string Id, bool TypeCarte)
{
	string fileName = tools::filesystem::dataDirectory() + "carte/" + Id;
	if (TypeCarte == TYPE_CARTE) fileName += ".map";
	if (TypeCarte == TYPE_LISTE) fileName += ".lst";

	Carte *carte = NULL;

	if (TypeCarte == TYPE_CARTE)
	{
		if (gamedata::world(Id) == NULL) gamedata::addWorld(Id);
		else return;
		carte = gamedata::world(Id);
	}

	ifstream fileStream(fileName, ios_base::in);

	if (!fileStream.good()) Erreur("Le fichier suivant n'a pu être chargé :", fileName);
	if (fileStream.good()) MESSAGE(" Fichier \"" + fileName + "\" ouvert", FICHIER)

	loadElementsFromStream(fileStream, carte, (TypeCarte == TYPE_LISTE) ? Id : "DEFAUT");

	fileStream.close();
}

Element_Carte* loadElementsFromStream(istream& Fichier, Carte *carte, string list)
{
	//int bufferInt;
	string bufferString;
	string Ind; int X, Y;

	bool Immuable = false, SansCollision = false;

	Element_Carte *lastElementLoaded = NULL;

	string TypeDonnee;

	while (Fichier.rdstate() == 0)
	{
		Fichier >> TypeDonnee;

		if (TypeDonnee == "CARTE_MERE")
		{
			Fichier >> bufferString;
			carte = gamedata::world(bufferString);
		}
		if (TypeDonnee == "LISTE_IMMUABLE") Immuable = true;
		if (TypeDonnee == "SANS_COLLISION") SansCollision = true;

        if (TypeDonnee == "loadXMLFile")
        {
            string file, b2;
            Fichier >> file >> b2;
            carte->loadFromFile(tools::filesystem::dataDirectory() + file, b2);
        }

		if (TypeDonnee == "IND_UNIQUE" && carte != NULL)
		{
			Fichier >> Ind >> X >> Y;
			lastElementLoaded = carte->AjouterElement_Unique(Ind, list, X, Y);
		}
		if (TypeDonnee == "IND_COMMUN" && carte != NULL)
		{
			Fichier >> Ind >> X >> Y;
			lastElementLoaded = carte->AjouterElement_Commun(Ind, list, X, Y);
		}
		if (TypeDonnee == "JOUEUR" && carte != NULL)
		{
			Fichier >> Ind >> X >> Y;
			lastElementLoaded = carte->AjouterJoueur(Ind, list, X, Y);
		}
		if (TypeDonnee == "PAYSAGE" && carte != NULL)
		{
			Fichier >> Ind >> X >> Y;
			lastElementLoaded = carte->AjouterPaysage(Ind, list, X, Y);
			if (lastElementLoaded != NULL && Immuable)		lastElementLoaded->TypeClassement = CLASSEMENT_CADAVRE;
			if (lastElementLoaded != NULL && SansCollision)	lastElementLoaded->ignoreCollision = true;
		}
		if (TypeDonnee == "PAYSAGE-REPEAT" && carte != NULL)
		{
			Fichier >> Ind >> X >> Y;
			lastElementLoaded = carte->AjouterPaysage(Ind, list, X, Y);
			if (lastElementLoaded != NULL && Immuable)		lastElementLoaded->TypeClassement = CLASSEMENT_CADAVRE;
			if (lastElementLoaded != NULL && SansCollision)	lastElementLoaded->ignoreCollision = true;
            Paysage* paysage = dynamic_cast<Paysage*>(lastElementLoaded);
            Fichier >> paysage->extent.x >> paysage->extent.y;
		}
		if (TypeDonnee == "DOOR" && carte != NULL)
		{
			Fichier >> X >> Y;
			lastElementLoaded = carte->addDoor(list, X, Y);
			Door* door = dynamic_cast<Door*>(lastElementLoaded);
            int a, b;
            Fichier >> a >> b >> door->deniedDiplomacy;
            door->size.rectangle(tools::math::Vector2d(-a, -b), tools::math::Vector2d(a, -b), tools::math::Vector2d(-a, b));
            door->size.setOrigin(&door->position());
		}
		if (TypeDonnee == "ACTIONNEUR" && carte != NULL)
		{
			Fichier >> X >> Y;
			Actionneur *actionneur = carte->AjouterActionneur(list, X, Y);
			actionneur->Load(Fichier);
			lastElementLoaded = actionneur;
		}
		if (TypeDonnee == "SHARED_TRIGGER" && carte != NULL)
		{
			Trigger *trigger = carte->addTrigger(list);
			trigger->load(Fichier, carte);
			lastElementLoaded = trigger;
		}
		if (TypeDonnee == "COFFRE" && carte != NULL)
		{
			Fichier >> X >> Y;
			Coffre *coffre = carte->AjouterCoffre(list, X, Y);

			string TypeDonnee2;
			bool FinCoffre = false;
			int NombreObjets = 1;
			int NumeroObjet = 1;
			while (!FinCoffre)
			{
				Fichier >> TypeDonnee2;
				if (TypeDonnee2 == "RAY_COL")
				{
                    int a;
                    Fichier >> a;
                    coffre->size.circle(tools::math::Vector2d(0, 0), a);
                    coffre->size.setOrigin(&coffre->position());
				}
				if (TypeDonnee2 == "PAYSAGE")
				{
                    Fichier >> coffre->Type;
                    coffre->imageContainer = "paysage";
                    coffre->TypeClassement = CLASSEMENT_NORMAL;
				}
				if (TypeDonnee2 == "NOM")
				{
					Fichier >> coffre->NumeroNom;
					coffre->Nom = tools::textManager::getText("species", coffre->NumeroNom);
				}
				if (TypeDonnee2 == "NOMBRE_OBJETS") Fichier >> NombreObjets;
				if (TypeDonnee2 == "NOMBRE_OBJETS_ALEATOIRE")
				{
					int min = 0, max = 0;
					Fichier >> min >> max;
					NombreObjets = min + rand()%(max-min+1);
				}
				if (TypeDonnee2 == "OBJET")
				{
					ifstream::pos_type PosObjet = Fichier.tellg();
					string TypeDonnee3;
					bool FinObjet = false;
					string IdObjet = ""; int Qualite = 0;
					int NombreTypesAleatoires = 1;
					while (!FinObjet)
					{
						Fichier >> TypeDonnee3;

						if (TypeDonnee3 == "TYPE")
						{
							Fichier >> IdObjet >> Qualite;
						}
						if (TypeDonnee3 == "TYPE_ALEATOIRE")
						{
							int test = rand()%NombreTypesAleatoires;
							if (!test)
								Fichier >> IdObjet >> Qualite;
							++NombreTypesAleatoires;
						}
						if (TypeDonnee3 == "FIN_OBJET")
						{
							--NombreObjets;
							NombreTypesAleatoires = 1;
							coffre->objects.addObject(IdObjet, "storagebox" + intToString(NumeroObjet, 2), Qualite);
							++NumeroObjet;
							if (NombreObjets <= 0)
							{
								FinObjet = true;
								NombreObjets = 1;
							}
							else Fichier.seekg(PosObjet);
						}
						TypeDonnee3 = "";
					}
				}
				if (TypeDonnee2 == "FIN_COFFRE")
				{
					FinCoffre = true;
					lastElementLoaded = coffre;
				}
				TypeDonnee2 = "";
			}
		}
		TypeDonnee = "";
	}

	return lastElementLoaded;
}

void ChangerCarte(Element_Carte *elem, string IdOrig, string IdCible)
{
	Carte *Orig = gamedata::world(IdOrig);
	Carte *Cible = gamedata::world(IdCible);

	if (elem == NULL || Orig == NULL || Cible == NULL) return;

	Orig->elements.remove(elem);
	Cible->elements.push_back(elem);
}


/** FONCTIONS DE LA CLASSE Carte **/

Carte::Carte()
{
    commonDataLoaded = false;
}

Carte::~Carte()
{
	for (auto& element : elements)
		delete element;

    for (auto& tmp : triggers)
        delete tmp;

    for (auto& tmp : places)
        delete tmp.first;

    for (auto& tmp : luaTriggers)
        lua_close(tmp.second);

    elements.clear();
    triggers.clear();
    luaTriggers.clear();
}

void Carte::AjouterElementEnListe(Element_Carte *elem)
{
	if (elem == NULL)
	{
		Erreur("Carte::AjouterElementEnListe() appelé avec NULL", "");
		return;
	}

	elements.push_back(elem);
}

Individu_Unique* Carte::AjouterElement_Unique(string Type, string liste, int x, int y)
{
	Individu_Unique *ind = new Individu_Unique;

	ind->Liste = liste;
	ind->Type = Type;

	Load_IndividuUnique(Type, ind);

    ind->move(x, y);

	MESSAGE("Un individu unique a été ajouté - Type = " + Type, FICHIER)

	AjouterElementEnListe(ind);
	return ind;
}

Individu_Commun* Carte::AjouterElement_Commun(string Type, string liste, int x, int y)
{
	Individu_Commun *ind = new Individu_Commun;

	ind->Liste = liste;
	ind->Type = Type;

	Load_ClasseCommune(Type);
    ind->Classe = gamedata::species(Type);

    if (ind->Classe == nullptr)
	{
		Erreur("La classe de l'élément commun suivant n'a pas pu être chargée", Type);
		delete ind;
		return NULL;
	}

	ind->Classe->Copie_Element(ind);
    ind->move(x, y);

	MESSAGE("Un individu commun a été ajouté - Classe = " + Type, FICHIER)

	AjouterElementEnListe(ind);
	return ind;
}

Joueur* Carte::AjouterJoueur(string Type, string liste, int x, int y)
{
	Joueur *ind = new Joueur;

	ind->Liste = liste;
	ind->Type = Type;

	Load_IndividuUnique(Type, ind);

    ind->move(x, y);

	MESSAGE("Un joueur a été ajouté - Type = " + Type, FICHIER)

    gamedata::setPlayer(ind, this);

	AjouterElementEnListe(ind);
	return ind;
}

Paysage* Carte::AjouterPaysage(string Type, string liste, int x, int y)
{
	Paysage *ind = new Paysage;

	ind->Liste = liste;
	ind->Type = Type;

    gamedata::copyInertItemFromDesign(Type, ind);

    ind->move(x, y);

	MESSAGE("Un paysage a été ajouté - Classe = " + Type, FICHIER)

	AjouterElementEnListe(ind);
	return ind;
}

Door* Carte::addDoor(string liste, int x, int y)
{
	Door *ind = new Door;

	ind->Liste = liste;

    ind->move(x, y);
    ind->size.setOrigin(&ind->position());

	MESSAGE("A door has been added.", FICHIER)

	AjouterElementEnListe(ind);
	return ind;
}

Actionneur* Carte::AjouterActionneur(string liste, int x, int y)
{
	Actionneur *ind = new Actionneur;

	ind->Liste = liste;
	ind->Type = TYPE_ACTIONNEUR;

    ind->move(x, y);
    ind->size.setOrigin(&ind->position());

	MESSAGE("Un actionneur a été ajouté", FICHIER)

	AjouterElementEnListe(ind);
	return ind;
}

Trigger* Carte::addTrigger(string liste)
{
	Trigger *ind = new Trigger;

	ind->Liste = liste;
    ind->size.setOrigin(&ind->position());

	MESSAGE("A trigger has been added", FICHIER)

	triggers.push_back(ind);
	return ind;
}

Coffre* Carte::AjouterCoffre(string liste, int x, int y)
{
	Coffre *ind = new Coffre;

	ind->Liste = liste;

    ind->move(x, y);
    ind->size.setOrigin(&ind->position());

	MESSAGE("Un coffre a été ajouté", FICHIER)

	AjouterElementEnListe(ind);
	return ind;
}

void Carte::SupprimerElement(Element_Carte* elem)
{
	MESSAGE("Element_Carte " + intToString(elem->Id) + " va être supprimé", FICHIER)

	delete elem;
	elements.remove(elem);
}

void Carte::SupprimerListe(string num)
{
	for (auto tmp = elements.begin() ; tmp != elements.end() ; ++tmp)
	{
		if ((*tmp)->Liste == num)
		{
			SupprimerElement(*tmp);
			tmp = elements.begin();
		}
	}
}

bool comparisonBetweenElements(Element_Carte* a, Element_Carte* b)
{
	/* Classement des Élements selon leur TypeClassement
	 * - CLASSEMENT_SOL
	 * - CLASSEMENT_CADAVRE
	 * - CLASSEMENT_NORMAL & CLASSEMENT_HAUT
	 * - CLASSEMENT_NUAGE
	*/
	int aClass = a->TypeClassement;	if (aClass == CLASSEMENT_HAUT) aClass = CLASSEMENT_NORMAL;
	int bClass = b->TypeClassement;	if (bClass == CLASSEMENT_HAUT) bClass = CLASSEMENT_NORMAL;

	if (aClass < bClass) return true;
	if (aClass > bClass) return false;

	if (a->position().y >= b->position().y) return false;

	return true;
}

void Carte::GestionElements(const View& worldView)
{
	int RetourElement = 0;
	Element_Carte* ASupprimer = NULL;

    int max = worldView.getSize().x + 1500;

	for (auto& tmp : elements)
	{
        if (abs((int)(tmp->position().x - worldView.getCenter().x)) <= max &&
            abs((int)(tmp->position().y - worldView.getCenter().y)) <= max)
            RetourElement = tmp->Gestion();

		switch(RetourElement)
		{
			case ETAT_MORT		: ASupprimer = tmp; break;
		}
	}

	elements.sort(comparisonBetweenElements);

	if (ASupprimer != NULL) SupprimerElement(ASupprimer);

	for (map<string, lua_State*>::reference L : luaTriggers)
	{
		lua_getglobal(L.second, "triggerManage");
		lua_call(L.second, 0, 0);
	}
}

void Carte::display(RenderTarget& target)
{
    int max = target.getView().getSize().x + 300;
    for (auto& tmp : elements)
    {
        if (abs((int)(tmp->position().x - target.getView().getCenter().x)) <= max &&
            abs((int)(tmp->position().y - target.getView().getCenter().y)) <= max)
            tmp->Disp(target);
    }
}

void Carte::displayBackground(RenderTarget& target)
{
	if (backgroundImage.empty()) return;

    Vector2f origin = target.getView().getCenter();
    origin.x -= target.getSize().x / 2;
    origin.y -= target.getSize().y / 2;

    for (int a = -1 ; a < (int)(target.getSize().x/135 + 2) ; ++a)
    {
        for (int b = -1 ; b < (int)(target.getSize().y/144 + 2) ; ++b)
        {
            imageManager::display(target, "paysage", backgroundImage, origin.x + 135*a - (int)(origin.x)%135, origin.y + 144*b - (int)(origin.y)%144);
        }
    }
}

/** AUTRES FONCTIONS */

void Carte::loadFromFile(string path, string tag)
{
    XMLDocument file;
    file.LoadFile(path.c_str());

    XMLHandle hdl(file);
    XMLElement *elem = hdl.FirstChildElement().FirstChildElement().ToElement();

    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "properties")
        {
            if (elem->Attribute("backgroundImage"))
            {
                backgroundImage = elem->Attribute("backgroundImage");
                imageManager::addImage("paysage", backgroundImage, backgroundImage);
            }
        }
        if (elemName == "loadDataFile" && !commonDataLoaded)
        {
            string dataFile = elem->Attribute("file");
            gamedata::loadFromXML(tools::filesystem::dataDirectory(), dataFile);
            dataFiles.insert(std::move(dataFile));
        }
        if (elemName == "place" && !commonDataLoaded)
        {
            Paysage *p = new Paysage;
            string music;
            if (elem->Attribute("name")) p->Type = elem->Attribute("name");
            if (elem->Attribute("music"))
            {
                music = elem->Attribute("music");
                musicManager::addMusic(music);
            }

            XMLHandle hdl2(elem);
            XMLElement *elem2 = hdl2.FirstChildElement().ToElement();
            while (elem2)
            {
                string elem2Name = elem2->Name();
                if (elem2Name == "properties")
                {
                    elem2->QueryAttribute("diplomacy", &p->Diplomatie);
                }
                if (elem2Name == "shape")
                {
                    p->size.loadFromXML(elem2);
                }

                elem2 = elem2->NextSiblingElement();
            }
            places.push_back(pair<Element_Carte*, string>(p, music));
        }
        else if (elemName == "items")
        {
            string currentTag = "ALL";
            bool Immuable = false;
            bool ignoreCollision = false;

            if (elem->Attribute("tag")) currentTag = elem->Attribute("tag");
            elem->QueryAttribute("immuable", &Immuable);
            elem->QueryAttribute("ignoreCollision", &ignoreCollision);

            if (currentTag == tag || tag == "ALL")
            {
                XMLHandle hdl2(elem);
                XMLElement *item = hdl2.FirstChildElement().ToElement();
                while (item)
                {
                    Element_Carte* newItem = nullptr;

                    string itemName = item->Name();

                    if (itemName == "inertItem")
                        newItem = new Paysage;
                    else if (itemName == "individual")
                        newItem = new Individu_Commun;
                    else if (itemName == "unique")
                        newItem = new Individu_Unique;
                    else if (itemName == "storageBox")
                        newItem = new Coffre;

                    if (newItem != nullptr)
                    {
                        elements.push_back(newItem);

                        if (tag != "ALL") newItem->Liste = tag;
                        if (ignoreCollision) newItem->ignoreCollision = true;

                        XMLHandle hdl3(item);
                        newItem->loadFromXML(hdl3);

                        if (Immuable) newItem->TypeClassement = CLASSEMENT_CADAVRE;
                    }

                    item = item->NextSiblingElement();
                }
            }
        }
        elem = elem->NextSiblingElement();
    }

    commonDataLoaded = true;
}

void Carte::saveToXML(XMLDocument& doc, XMLHandle& handle)
{
    XMLElement* root = handle.ToElement();

    XMLElement* properties = doc.NewElement("properties");
    if (!backgroundImage.empty())
        properties->SetAttribute("backgroundImage", backgroundImage.c_str());
    root->InsertEndChild(properties);

    for (auto& tmp : dataFiles)
    {
        XMLElement* dataFile = doc.NewElement("loadDataFile");
        dataFile->SetAttribute("file", tmp.c_str());
        root->InsertEndChild(dataFile);
    }

    for (auto& tmp : places)
    {
        XMLElement* place = doc.NewElement("place");
        place->SetAttribute("name", tmp.first->Type.c_str());
        place->SetAttribute("music", tmp.second.c_str());

        XMLElement* placeProperties = doc.NewElement("properties");
        placeProperties->SetAttribute("diplomacy", tmp.first->Diplomatie);
        place->InsertEndChild(placeProperties);

        XMLElement* placeShape = doc.NewElement("shape");
        place->InsertEndChild(placeShape);
        XMLHandle shapeHandle(placeShape);
        tmp.first->size.saveToXML(doc, shapeHandle);

        root->InsertEndChild(place);
    }

    XMLElement* _items = doc.NewElement("items");
    root->InsertEndChild(_items);
    XMLHandle itemsHandle(_items);

    XMLElement* _triggers = doc.NewElement("triggers");
    root->InsertEndChild(_triggers);
    XMLHandle triggersHandle(_triggers);

    for (auto& tmp : elements)
    {
        if (tmp != gamedata::player())
            tmp->saveToXML(doc, itemsHandle);
    }

    for (auto& tmp : triggers)
        tmp->saveToXML(doc, triggersHandle);

}
