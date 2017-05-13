
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

#include "imageManager/imageManager.h"
#include "musicManager/musicManager.h"

#include "../Bibliotheque/Constantes.h"
#include "../Jeu/Jeu.h"
#include "Carte.h"
#include "../ElementsCarte/ElementsCarte.h"

void Load_Carte(string Id, bool TypeCarte)
{
	string fileName = INSTALL_DIR + "carte/" + Id;
	if (TypeCarte == TYPE_CARTE) fileName += ".map";
	if (TypeCarte == TYPE_LISTE) fileName += ".lst";

	Carte *carte = NULL;

	if (TypeCarte == TYPE_CARTE)
	{
		if (Get_Carte(Id) == NULL) Ajouter_Carte(Id);
		else return;
		carte = Get_Carte(Id);
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
			carte = Get_Carte(bufferString);
		}
		if (TypeDonnee == "AMBIENCE")
		{
			Fichier >> bufferString;
            musicManager::addMusic(bufferString);
			if (carte != NULL) carte->ambience = bufferString;
		}
		if (TypeDonnee == "LISTE_IMMUABLE") Immuable = true;
		if (TypeDonnee == "SANS_COLLISION") SansCollision = true;

		if (TypeDonnee == "backgroundImage")
		{
			Fichier >> bufferString;
			if (carte != NULL) carte->setBackgroundImage(bufferString);
			else Erreur("Load_Carte() :", "FOND_CARTE demandé alors que la carte n'est pas initialisée");
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
			if (lastElementLoaded != NULL && SansCollision)	lastElementLoaded->collisionType = NoCollision;
		}
		if (TypeDonnee == "PAYSAGE-REPEAT" && carte != NULL)
		{
			Fichier >> Ind >> X >> Y;
			lastElementLoaded = carte->AjouterPaysage(Ind, list, X, Y);
			if (lastElementLoaded != NULL && Immuable)		lastElementLoaded->TypeClassement = CLASSEMENT_CADAVRE;
			if (lastElementLoaded != NULL && SansCollision)	lastElementLoaded->collisionType = NoCollision;
			Paysage* paysage = dynamic_cast<Paysage*>(lastElementLoaded);
			Fichier >> paysage->repeatX >> paysage->repeatY;
			paysage->calculateCollisionRadius();
			carte->setMaxRadius(paysage->RayX);
			carte->setMaxRadius(paysage->RayY);
		}
		if (TypeDonnee == "DOOR" && carte != NULL)
		{
			Fichier >> X >> Y;
			lastElementLoaded = carte->addDoor(list, X, Y);
			Door* door = dynamic_cast<Door*>(lastElementLoaded);
			Fichier >> door->RayX >> door->RayY >> door->deniedDiplomacy;
		}
		if (TypeDonnee == "PAYSAGE_MOUVANT" && carte != NULL)
		{
			Fichier >> Ind >> X >> Y;
			lastElementLoaded = carte->AjouterPaysageMouvant(Ind, list, X, Y);
		}
		if (TypeDonnee == "LANCEUR" && carte != NULL)
		{
			Fichier >> Ind >> X >> Y;
			lastElementLoaded = carte->AjouterPaysageLanceur(Ind, list, X, Y);
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
			int NumeroObjet = 0;
			while (!FinCoffre)
			{
				Fichier >> TypeDonnee2;
				if (TypeDonnee2 == "RAY_COL")
				{
					coffre->collisionType = CircleCollision;
					Fichier >> coffre->RayonCollision;
				}
				if (TypeDonnee2 == "PAYSAGE")
				{
					string PaysageCoffre;
					Fichier >> PaysageCoffre;

					lastElementLoaded = carte->AjouterPaysage(PaysageCoffre, list, X, Y);
					if (lastElementLoaded != NULL && Immuable)		lastElementLoaded->TypeClassement = CLASSEMENT_CADAVRE;
					if (lastElementLoaded != NULL && SansCollision)	lastElementLoaded->collisionType = NoCollision;
				}
				if (TypeDonnee2 == "NOM")
				{
					Fichier >> coffre->NumeroNom;
					coffre->Nom = getTranslatedNameOfElement(coffre->NumeroNom);
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
							coffre->objects.addObject(IdObjet, intToString(CLEF_COFFRE + NumeroObjet), Qualite);
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
	Carte *Orig = Get_Carte(IdOrig);
	Carte *Cible = Get_Carte(IdCible);

	if (elem == NULL || Orig == NULL || Cible == NULL) return;

	Orig->elements.remove(elem);
	Cible->elements.push_back(elem);
}

Element_Carte* Get_Element(int id)
{
	for (auto& map : Partie.maps)
		for (auto& element : map.second.elements)
			if (element->Id == id) return element;

	return NULL;
}

Individu_Unique* Get_IndividuUnique(string type)
{
	for (auto& map : Partie.maps)
		for (auto& element : map.second.elements)
			if (element->Type == type) return dynamic_cast<Individu_Unique*>(element);

	return NULL;
}

/** FONCTIONS DE LA CLASSE Carte **/

Carte::Carte()
{
}

Carte::~Carte()
{
	for (auto& element : elements)
		delete element;

    for (auto& tmp : triggers)
        delete tmp;

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
	setMaxRadius(elem->RayonCollision);
	setMaxRadius(elem->RayX);
	setMaxRadius(elem->RayY);
}

Individu_Unique* Carte::AjouterElement_Unique(string Type, string liste, int x, int y)
{
	Individu_Unique *ind = new Individu_Unique;

	ind->Id = NouveauId();
	ind->Liste = liste;
	ind->Type = Type;

	Load_IndividuUnique(Type, ind);

	ind->PosX = x;
	ind->PosY = y;
	ind->Set_Controle(AI);

	MESSAGE("Un individu unique a été ajouté - Type = " + Type, FICHIER)

	AjouterElementEnListe(ind);
	setMaxRadius(ind->ChampVision);
	return ind;
}

Individu_Commun* Carte::AjouterElement_Commun(string Type, string liste, int x, int y)
{
	Individu_Commun *ind = new Individu_Commun;

	ind->Id = NouveauId();
	ind->Liste = liste;
	ind->Type = Type;

	Load_ClasseCommune(Type);
	ind->Classe = getCommonClass(Type);

	if (ind->Classe == NULL)
	{
		Erreur("La classe de l'élément commun suivant n'a pas pu être chargée", Type);
		delete ind;
		return NULL;
	}

	ind->PosX = x;
	ind->PosY = y;
	ind->Set_Controle(AI);

	ind->Classe->Copie_Element(ind);

	MESSAGE("Un individu commun a été ajouté - Classe = " + Type, FICHIER)

	AjouterElementEnListe(ind);
	setMaxRadius(ind->Classe->ChampVision);
	return ind;
}

Joueur* Carte::AjouterJoueur(string Type, string liste, int x, int y)
{
	Joueur *ind = new Joueur;

	ind->Id = NouveauId();
	ind->Liste = liste;
	ind->Type = Type;

	Load_IndividuUnique(Type, ind);

	ind->PosX = x;
	ind->PosY = y;
	ind->Set_Controle(HUMAIN); /** NE DISTINGUE PAS LE JOUEUR PRINCIPAL D'UN JOUEUR EN RÉSEAU **/

	MESSAGE("Un joueur a été ajouté - Type = " + Type, FICHIER)

	AjouterElementEnListe(ind);
	setMaxRadius(ind->ChampVision);
	return ind;
}

Paysage* Carte::AjouterPaysage(string Type, string liste, int x, int y)
{
	Paysage *ind = new Paysage;

	ind->Id = NouveauId();
	ind->Liste = liste;
	ind->Type = Type;

	Load_ClassePaysage(Type);
	ind->Classe = getLandsClass(Type);

	if (ind->Classe == NULL)
	{
		Erreur("La classe du paysage suivant a été demandée sans avoir été chargée :", Type);
		delete ind;
		return NULL;
	}

	ind->PosX = x;
	ind->PosY = y;
	ind->Set_Controle(AI);

	ind->Classe->Copie_Element(ind);

	MESSAGE("Un paysage a été ajouté - Classe = " + Type, FICHIER)

	AjouterElementEnListe(ind);
	return ind;
}

Door* Carte::addDoor(string liste, int x, int y)
{
	Door *ind = new Door;

	ind->Id = NouveauId();
	ind->Liste = liste;

	ind->PosX = x;
	ind->PosY = y;
	ind->Set_Controle(AI);

	MESSAGE("A door has been added.", FICHIER)

	AjouterElementEnListe(ind);
	return ind;
}

Paysage_Mouvant* Carte::AjouterPaysageMouvant(string Type, string liste, int x, int y)
{
	Paysage_Mouvant *ind = new Paysage_Mouvant;

	ind->Id = NouveauId();
	ind->Liste = liste;
	ind->Type = Type;

	Load_ClassePaysageMouvant(Type);
	ind->Classe = getMovingLandsClass(Type);

	if (ind->Classe == NULL)
	{
		Erreur("La classe du paysage mouvant suivant a été demandée sans avoir été chargée :", Type);
		delete ind;
		return NULL;
	}

	ind->PosX = x;
	ind->PosY = y;
	ind->Set_Controle(AI);

	ind->Classe->Copie_Element(ind);

	MESSAGE("Un paysage mouvant a été ajouté - Classe = " + Type, FICHIER)

	AjouterElementEnListe(ind);
	return ind;
}

Paysage_Lanceur* Carte::AjouterPaysageLanceur(string Type, string liste, int x, int y)
{
	Paysage_Lanceur *ind = new Paysage_Lanceur;

	ind->Id = NouveauId();
	ind->Liste = liste;
	ind->Type = Type;

	ind->PosX = x;
	ind->PosY = y;
	ind->Set_Controle(AI);

	Load_ClassePaysageMouvant(Type);
	ind->Classe = getMovingLandsClass(Type);	//Données du paysage

	if (ind->Classe == NULL)
	{
		Erreur("La classe du paysage mouvant lanceur suivant a été demandée sans avoir été chargée", Type);
		delete ind;
		return NULL;
	}
	ind->Classe->Copie_Element(ind);
	Load_PaysageLanceur(Type, ind);		//Données du projectile

	MESSAGE("Un paysage lanceur a été ajouté - Classe = " + Type, FICHIER)

	AjouterElementEnListe(ind);
	return ind;
}

Projectile* Carte::AjouterProjectile(Projectile &prj)
{
	Projectile *ind = new Projectile(prj);

	ind->Id = NouveauId();
	ind->PosX = ind->OrigineX;
	ind->PosY = ind->OrigineY;
	ind->collisionType = RectangleCollision;

	MESSAGE("Un projectile a été crée - Classe = " + ind->Type, FICHIER)

	AjouterElementEnListe(ind);
	return ind;
}

Actionneur* Carte::AjouterActionneur(string liste, int x, int y)
{
	Actionneur *ind = new Actionneur;

	ind->Id = NouveauId();
	ind->Liste = liste;
	ind->Type = TYPE_ACTIONNEUR;

	ind->PosX = x;
	ind->PosY = y;
	ind->Set_Controle(AI);

	MESSAGE("Un actionneur a été ajouté", FICHIER)

	AjouterElementEnListe(ind);
	return ind;
}

Trigger* Carte::addTrigger(string liste)
{
	Trigger *ind = new Trigger;

	ind->Id = NouveauId();
	ind->Liste = liste;
	ind->Set_Controle(AI);

	MESSAGE("A trigger has been added", FICHIER)

	triggers.push_back(ind);
	return ind;
}

Coffre* Carte::AjouterCoffre(string liste, int x, int y)
{
	Coffre *ind = new Coffre;

	ind->Id = NouveauId();
	ind->Liste = liste;
	ind->Type = "TYPE_COFFRE";

	ind->PosX = x;
	ind->PosY = y;
	ind->Set_Controle(AI);

	MESSAGE("Un coffre a été ajouté", FICHIER)

	AjouterElementEnListe(ind);
	return ind;
}

Cadavre* Carte::AjouterCadavre(string liste, float x, float y)
{
	Cadavre *ind = new Cadavre;

	ind->Id = NouveauId();
	ind->Liste = liste;
	ind->Type = "TYPE_CADAVRE";

	ind->PosX = x;
	ind->PosY = y;
	ind->Set_Controle(AI);
	ind->collisionType = CircleCollision;
	ind->RayonCollision = 1;
	ind->Duree = 500;

	MESSAGE("Un cadavre a été ajouté", FICHIER)

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

	if (a->PosY >= b->PosY) return false;

	return true;
}

int Carte::GestionElements()
{
	int Retour = ACTION_JEU;

	int RetourElement = 0;
	Element_Carte* ASupprimer = NULL;

	for (auto& tmp : elements)
	{
		RetourElement = tmp->Gestion();

		switch(RetourElement)
		{
			case ACTION_JEU		: break;
			case ACTION_QUITTER	: Retour = ACTION_QUITTER; break;
			case ACTION_PAUSE	: Retour = ACTION_PAUSE; break;
			case ACTION_SAUVEG	: Retour = ACTION_SAUVEG; break;
			case ACTION_REPOS	: Retour = ACTION_REPOS; break;
			case ACTION_MORT	: Retour = ACTION_MORT; break;
			case ETAT_NORMAL	: tmp->Disp(Partie.PosCarteX, Partie.PosCarteY); break;
			case ETAT_DESACTIVE	: break;
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

	for (auto& tmp : triggers)
		tmp->Disp(Partie.PosCarteX, Partie.PosCarteY);

	return Retour;
}

void Carte::setBackgroundImage(string path)
{
    backgroundImage = path;

    imageManager::addContainer("paysage");
    imageManager::addImage("paysage", path, INSTALL_DIR + path);

    MESSAGE("Fond de la carte choisi", IMAGE)
}

void Carte::displayBackground()
{
	if (backgroundImage.empty()) return;

	while (PosFondX > 0)
		PosFondX -= 135;

	while (PosFondX < -134)
		PosFondX += 135;

	while (PosFondY > 24)
		PosFondY -= 144;

	while (PosFondY < -119)
		PosFondY += 144;

	for (unsigned short a = 0 ; a < Options.ScreenW/135 + 2 ; ++a)
	{
		for (unsigned short b = 0 ; b < Options.ScreenH/144 + 2 ; ++b)
		{
            imageManager::display(Jeu.App, "paysage", backgroundImage, PosFondX+135*a, PosFondY+144*b);
		}
	}
}

/** AUTRES FONCTIONS */

void Lag_PosFondCartes(float lagX, float lagY)
{
	for (auto& map : Partie.maps)
	{
		map.second.PosFondX += lagX;
		map.second.PosFondY += lagY;
	}
}

void Set_PosCarte(float posX, float posY, bool MaJ)		//DEVRAIT APPARTENIR À CLASSE_PARTIE
{
	static float bufX, bufY;
	if (!MaJ) {bufX = posX; bufY = posY;}
	else {Partie.PosCarteX = bufX; Partie.PosCarteY = bufY;}
}

