
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
	Carte _new;
	Partie.maps.insert(map<string, Carte>::value_type(Id, _new));

	Partie.maps[Id].Id = Id;

	MESSAGE("Carte " + Id + " ajoutée", LISTE)
}

void SupprimerListe_Carte()
{
	Partie.maps.clear();

	MESSAGE("Liste des Cartes supprimée", LISTE)
}

Carte* Get_Carte(string Id)
{
	auto i = Partie.maps.find(Id);

	if (i != Partie.maps.end()) return &(i->second);

	return NULL;
}


void addCommonClass(string type)
{
	Classe_Commune _new;
	Partie.commonClasses.insert(map<string, Classe_Commune>::value_type(type, _new));

	MESSAGE("Classe Commune " + type + " ajoutée", LISTE)
}

void deleteCommonClasses()
{
	Partie.commonClasses.clear();

	MESSAGE("Liste des Classes Communes supprimée", LISTE)
}

Classe_Commune* getCommonClass(string type)
{
	auto i = Partie.commonClasses.find(type);

	if (i != Partie.commonClasses.end()) return &(i->second);

	return NULL;
}


void addLandsClass(string type)
{
	Classe_Paysage _new;
	Partie.landsClasses.insert(map<string, Classe_Paysage>::value_type(type, _new));

	MESSAGE("Classe Paysage " + type + " ajoutée", LISTE)
}

void deleteLandsClasses()
{
	Partie.landsClasses.clear();

	MESSAGE("Liste des Classes Paysages supprimée", LISTE)
}

Classe_Paysage* getLandsClass(string type)
{
	auto i = Partie.landsClasses.find(type);

	if (i != Partie.landsClasses.end()) return &(i->second);

	return NULL;
}


/** LISTE CHAINEE EN Classe_Paysage_Mouvant **/

void addMovingLandsClass(string type)
{
	Classe_Paysage_Mouvant _new;
	Partie.movingLandsClasses.insert(map<string, Classe_Paysage_Mouvant>::value_type(type, _new));

	MESSAGE("Classe Paysage Mouvant " + type + " ajoutée", LISTE)
}

void deleteMovingLandsClasses()
{
	Partie.movingLandsClasses.clear();

	MESSAGE("Liste des Classes Paysages Mouvants supprimée", LISTE)
}

Classe_Paysage_Mouvant* getMovingLandsClass(string type)
{
	auto i = Partie.movingLandsClasses.find(type);

	if (i != Partie.movingLandsClasses.end()) return &(i->second);

	return NULL;
}


void addCollider(Element_Carte *elem)
{
	Partie.colliders.push_back(elem);

	MESSAGE("Element collision " + intToString(elem->Id) + " ajouté", LISTE)
}

void removeCollider(Element_Carte *elem)
{
	Partie.colliders.remove(elem);

	MESSAGE("Element collision " + intToString(elem->Id) + " supprimé", LISTE)
}

void removeColliders()
{
	Partie.colliders.clear();

	MESSAGE("Liste des Elements Collisions suppprimée", LISTE)
}
