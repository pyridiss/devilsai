
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

#include "../Bibliotheque/Constantes.h"
#include "../Jeu/Jeu.h"
#include "Carte.h"
#include "../ElementsCarte/ElementsCarte.h"


/** LISTE CHAINEE EN Carte **/

void Ajouter_Carte(string Id)
{
	if (Partie.Carte_head == NULL)
	{
		Partie.Carte_head = new Carte;
		Partie.Carte_last = Partie.Carte_head;
	}
	else
	{
		Partie.Carte_last->next = new Carte;
		Partie.Carte_last = Partie.Carte_last->next;
	}
	Partie.Carte_last->Id = Id;

	MESSAGE("Carte " + Id + " ajoutée", LISTE)
}

void SupprimerListe_Carte()
{
	if (Partie.Carte_head != NULL)	delete Partie.Carte_head;
	Partie.Carte_head = NULL; Partie.Carte_last = NULL;

	MESSAGE("Liste des Cartes supprimée", LISTE)
}

Carte* Get_Carte(string Id)
{
	Carte *tmp = Partie.Carte_head;
	while (tmp != NULL && tmp->Id != Id)
	{
		tmp = tmp->next;
	}
	return tmp;
}


void addCommonClass(string type)
{
	Classe_Commune* _new = new Classe_Commune;
	Partie.commonClasses.insert(map<string, Classe_Commune*>::value_type(type, _new));

	MESSAGE("Classe Commune " + type + " ajoutée", LISTE)
}

void deleteCommonClasses()
{
	for (auto i : Partie.commonClasses)
		delete i.second;

	Partie.commonClasses.clear();

	MESSAGE("Liste des Classes Communes supprimée", LISTE)
}

Classe_Commune* getCommonClass(string type)
{
	map<string, Classe_Commune*>::iterator i = Partie.commonClasses.find(type);

	if (i != Partie.commonClasses.end()) return i->second;

	return NULL;
}


void addLandsClass(string type)
{
	Classe_Paysage* _new = new Classe_Paysage;
	Partie.landsClasses.insert(map<string, Classe_Paysage*>::value_type(type, _new));

	MESSAGE("Classe Paysage " + type + " ajoutée", LISTE)
}

void deleteLandsClasses()
{
	for (auto i : Partie.landsClasses)
		delete i.second;

	Partie.landsClasses.clear();

	MESSAGE("Liste des Classes Paysages supprimée", LISTE)
}

Classe_Paysage* getLandsClass(string type)
{
	map<string, Classe_Paysage*>::iterator i = Partie.landsClasses.find(type);

	if (i != Partie.landsClasses.end()) return i->second;

	return NULL;
}


/** LISTE CHAINEE EN Classe_Paysage_Mouvant **/

void addMovingLandsClass(string type)
{
	Classe_Paysage_Mouvant* _new = new Classe_Paysage_Mouvant;
	Partie.movingLandsClasses.insert(map<string, Classe_Paysage_Mouvant*>::value_type(type, _new));

	MESSAGE("Classe Paysage Mouvant " + type + " ajoutée", LISTE)
}

void deleteMovingLandsClasses()
{
	for (auto i : Partie.movingLandsClasses)
		delete i.second;

	Partie.movingLandsClasses.clear();

	MESSAGE("Liste des Classes Paysages Mouvants supprimée", LISTE)
}

Classe_Paysage_Mouvant* getMovingLandsClass(string type)
{
	map<string, Classe_Paysage_Mouvant*>::iterator i = Partie.movingLandsClasses.find(type);

	if (i != Partie.movingLandsClasses.end()) return i->second;

	return NULL;
}


void Ajouter_ElementCollision(Element_Carte *elem)
{
	Partie.colliders.push_back(elem);

	MESSAGE("Element collision " + intToString(elem->Id) + " ajouté", LISTE)
}

void Supprimer_ElementCollision(int id)
{
	Partie.colliders.remove(Get_Element(id));

	MESSAGE("Element collision " + intToString(id) + " supprimé", LISTE)
}

void SupprimerListe_Collision()
{
	Partie.colliders.clear();

	MESSAGE("Liste des Elements Collisions suppprimée", LISTE)
}
