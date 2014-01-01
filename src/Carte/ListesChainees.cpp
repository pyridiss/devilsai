
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


/** LISTE CHAINEE EN Classe_Commune **/

void Ajouter_ClasseCom(string Type)
{
	if (Partie.ClasseCom_head == NULL)
	{
		Partie.ClasseCom_head = new Classe_Commune;
		Partie.ClasseCom_last = Partie.ClasseCom_head;
	}
	else
	{
		Partie.ClasseCom_last->next = new Classe_Commune;
		Partie.ClasseCom_last = Partie.ClasseCom_last->next;
	}
	Partie.ClasseCom_last->Type = Type;

	MESSAGE("Classe Commune " + Type + " ajoutée", LISTE)
}

void SupprimerListe_ClasseCom()
{
	if (Partie.ClasseCom_head != NULL) delete Partie.ClasseCom_head;
	Partie.ClasseCom_head = NULL; Partie.ClasseCom_last = NULL;

	MESSAGE("Liste des Classes Communes supprimée", LISTE)
}

Classe_Commune* Get_ClasseCom(string Type)
{
	Classe_Commune *tmp = Partie.ClasseCom_head;
	while (tmp != NULL && tmp->Type != Type)
	{
		tmp = tmp->next;
	}
	return tmp;
}


/** LISTE CHAINEE EN Classe_Paysage **/

void Ajouter_ClassePay(string Type)
{
	if (Partie.ClassePay_head == NULL)
	{
		Partie.ClassePay_head = new Classe_Paysage;
		Partie.ClassePay_last = Partie.ClassePay_head;
	}
	else
	{
		Partie.ClassePay_last->next = new Classe_Paysage;
		Partie.ClassePay_last = Partie.ClassePay_last->next;
	}
	Partie.ClassePay_last->Type = Type;

	MESSAGE("Classe Paysage " + Type + " ajoutée", LISTE)
}

void SupprimerListe_ClassePay()
{
	if (Partie.ClassePay_head != NULL) delete Partie.ClassePay_head;
	Partie.ClassePay_head = NULL; Partie.ClassePay_last = NULL;

	MESSAGE("Liste des Classes Paysages supprimée", LISTE)
}

Classe_Paysage* Get_ClassePay(string Type)
{
	Classe_Paysage *tmp = Partie.ClassePay_head;
	while (tmp != NULL && tmp->Type != Type)
	{
		tmp = tmp->next;
	}
	return tmp;
}


/** LISTE CHAINEE EN Classe_Paysage_Mouvant **/

void Ajouter_ClassePayMvt(string Type)
{
	if (Partie.ClassePayMvt_head == NULL)
	{
		Partie.ClassePayMvt_head = new Classe_Paysage_Mouvant;
		Partie.ClassePayMvt_last = Partie.ClassePayMvt_head;
	}
	else
	{
		Partie.ClassePayMvt_last->next = new Classe_Paysage_Mouvant;
		Partie.ClassePayMvt_last = Partie.ClassePayMvt_last->next;
	}
	Partie.ClassePayMvt_last->Type = Type;

	MESSAGE("Classe Paysage Mouvant " + Type + " ajoutée", LISTE)
}

void SupprimerListe_ClassePayMvt()
{
	if (Partie.ClassePayMvt_head != NULL) delete Partie.ClassePayMvt_head;
	Partie.ClassePayMvt_head = NULL; Partie.ClassePayMvt_last = NULL;

	MESSAGE("Liste des Classes Paysages Mouvants supprimée", LISTE)
}

Classe_Paysage_Mouvant* Get_ClassePayMvt(string Type)
{
	Classe_Paysage_Mouvant *tmp = Partie.ClassePayMvt_head;
	while (tmp != NULL && tmp->Type != Type)
	{
		tmp = tmp->next;
	}
	return tmp;
}


/** LISTE CHAINEE EN Element_Collision **/

void Ajouter_ElementCollision(Element_Carte *elem)
{
	if (elem == NULL) return;

	if (Partie.Collision_head == NULL)
	{
		Partie.Collision_head = new Element_Collision;
		Partie.Collision_head->elem = elem;
		Partie.Collision_last = Partie.Collision_head;
	}
	else
	{
		Element_Collision *tmp = Partie.Collision_head;
		while (tmp != NULL && tmp->elem->PosY < elem->PosY) tmp = tmp->next;
		if (tmp == NULL) tmp = Partie.Collision_last;
		Element_Collision *next = tmp->next;
		tmp->next = NULL;

		tmp->next = new Element_Collision;
		tmp->next->elem = elem;
		tmp->next->next = next;

		if (tmp == Partie.Collision_last) Partie.Collision_last = Partie.Collision_last->next;
	}

	MESSAGE("Element collision " + intToString(elem->Id) + " ajouté", LISTE)
}

void Supprimer_ElementCollision(int id)
{
	Element_Collision *tmp = Partie.Collision_head;
	if (tmp == NULL) return;

	//Le premier élément a une gestion particulière
	if (tmp->elem->Id == id)
	{
		Partie.Collision_head = tmp->next;
		tmp->next = NULL;
		delete tmp;

		MESSAGE("Element collision " + intToString(id) + " supprimé", LISTE)

		return;
	}

	//Il ne s'agit pas du premier élément
	while (tmp->next->elem->Id != id)
	{
		tmp = tmp->next;
		if (tmp->next == NULL) break;
	}
	if (tmp != NULL) if (tmp->next != NULL) //Suppression de tmp->next
	{
		Element_Collision *tmp2 = tmp->next;
		if (tmp->next == Partie.Collision_last) Partie.Collision_last = tmp;
		tmp->next = tmp->next->next;
		tmp2->next = NULL;
		delete tmp2;

		MESSAGE("Element collision " + intToString(id) + " supprimé", LISTE)
	}
}

void SupprimerListe_Collision()
{
	if (Partie.Collision_head != NULL) delete Partie.Collision_head;
	Partie.Collision_head = NULL; Partie.Collision_last = NULL;

	MESSAGE("Liste des Elements Collisions suppprimée", LISTE)
}
