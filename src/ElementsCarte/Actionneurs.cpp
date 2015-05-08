
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

#include <typeinfo>

#include "../Bibliotheque/Constantes.h"
#include "../Jeu/Jeu.h"
#include "../Carte/Carte.h"
#include "ElementsCarte.h"


/** FONCTIONS DE LA CLASSE Actionneur **/

Actionneur::Actionneur() : Element_Carte()
{
	Element_Carte::Type = "ACTIONNEUR";
}

int Actionneur::Gestion()
{
	return ETAT_NORMAL;
}

int Actionneur::Collision(Individu* elem, int TypeCollision)
{
//	Activite *act = NULL;

	switch(Type)
	{
		case ACTION_TERMINEE :		return COLL_OK;
		case ACTION_INTERACTION :	return COLL_INTER;
		case ACTION_RECUP :			if (TypeCollision == COLL_PRIM)
										elem->Set_Recuperation(100);
									if (TypeCollision == COLL_VIS)
									{
										if (elem->Comportement <= COMPORTEMENT_REGEN && elem->get("Vitalite") < 140)
											return COMPORTEMENT_REGEN;
									}
									break;
		case ACTION_COLL_DIPLOM :	if (DonneeInt == elem->Diplomatie) return COLL_PRIM;
									break;
		case ACTION_COLL_CLASSE :	break;
		case ACTION_COLL_UNIQUE :	break;
		case ACTION_LIEU :			elem->IndiceLieu = DonneeInt;
									elem->LieuVillage = false;
									break;
		case ACTION_VILLAGE :		elem->IndiceLieu = DonneeInt;
									elem->LieuVillage = true;
									break;
		case ACTION_DEFENSE :		if (elem->Comportement <= COMPORTEMENT_DEFENSE)
										return COMPORTEMENT_DEFENSE;
									break;
		case ACTION_CGMT_CARTE :	if (elem != Partie.perso) return COLL_PRIM;
									break; //On développe en dehors du switch pour plus de lisibilité
		case ACTION_CHAPITRE :		if (elem == Partie.perso)
									{
										Load_Chapitre(DonneeInt);
										Type = ACTION_TERMINEE;
									}
									break;
		case ACTION_TEXTE_PERSO :
		case ACTION_TEXTE :			if (elem == Partie.perso && dialogue == NULL)
										Load_Dialogue();
									break;
		case ACTION_TEXTE_P_N :
		case ACTION_TEXTE_N :		if (elem == Partie.perso && dialogue == NULL &&
										(elem->Get_Dir() == NORD || elem->Get_Dir() == N_E || elem->Get_Dir() == N_O))
										Load_Dialogue();
									break;
		case ACTION_TEXTE_P_S :
		case ACTION_TEXTE_S :		if (elem == Partie.perso && dialogue == NULL &&
										(elem->Get_Dir() == SUD || elem->Get_Dir() == S_E || elem->Get_Dir() == S_O))
										Load_Dialogue();
									break;
		case ACTION_TEXTE_P_E :
		case ACTION_TEXTE_E :		if (elem == Partie.perso && dialogue == NULL &&
										(elem->Get_Dir() == EST || elem->Get_Dir() == N_E || elem->Get_Dir() == S_E))
										Load_Dialogue();
									break;
		case ACTION_TEXTE_P_O :
		case ACTION_TEXTE_O :		if (elem == Partie.perso && dialogue == NULL &&
										(elem->Get_Dir() == OUEST || elem->Get_Dir() == N_O || elem->Get_Dir() == S_O))
										Load_Dialogue();
									break;
	}

	if (Type == ACTION_CGMT_CARTE && elem == Partie.perso && Get_Carte(DonneeString) != NULL && DonneeString != Partie.CarteCourante->Id)
	{
		Disp_Chargement(255);

		//1. Suppression des collisions
		RaZ_Coll();
		removeColliders();

		//2. Changement de carte du personnage
		ChangerCarte(Partie.perso, Partie.CarteCourante->Id, DonneeString);

		//3. Mise à jour de CarteCourante et des collisions
		Partie.CarteCourante = Get_Carte(DonneeString);
		Partie.CarteCourante->AjouterListeEnCollision("ALL");
		RaZ_Coll();
	}

	if (Type == ACTION_CGMT_CARTE && Get_Carte(DonneeString) == NULL)
		return COLL_PRIM;

	if (TypeCollision == COLL_VIS) return COMPORTEMENT_ALEATOIRE;

	return COLL_OK;
}

void Actionneur::Load(istream &Fichier)
{
	bool FinActionneur = false;
	string TypeDonnee;
	while (!FinActionneur)
	{
		Fichier >> TypeDonnee;
		if (TypeDonnee == "RAY_COL")
		{
			ModeCollision = MODE_COLLISION_CERCLE;
			Fichier >> RayonCollision;
		}
		if (TypeDonnee == "RECT_COL")
		{
			ModeCollision = MODE_COLLISION_RECT;
			Fichier >> RayX >> RayY;
		}
		if (TypeDonnee == "INTERACTION")
		{
			Type = ACTION_INTERACTION;
		}
		if (TypeDonnee == "RECUPERATION")
		{
			Type = ACTION_RECUP;
		}
		if (TypeDonnee == "COLLISION_DIPLOM")
		{
			Type = ACTION_COLL_DIPLOM;	Fichier >> DonneeInt;
		}
		if (TypeDonnee == "COLLISION_CLASSE")
		{
			Type = ACTION_COLL_CLASSE;	Fichier >> DonneeInt;
		}
		if (TypeDonnee == "COLLISION_UNIQUE")
		{
			Type = ACTION_COLL_UNIQUE;	Fichier >> DonneeInt;
		}
		if (TypeDonnee == "LIEU")
		{
			Type = ACTION_LIEU;			Fichier >> DonneeInt;
		}
		if (TypeDonnee == "VILLAGE")
		{
			Type = ACTION_VILLAGE;		Fichier >> DonneeInt;
		}
		if (TypeDonnee == "DEFENSE")
		{
			Type = ACTION_DEFENSE;		Fichier >> DonneeInt;
		}
		if (TypeDonnee == "CHANGEMENT_CARTE")
		{
			Type = ACTION_CGMT_CARTE;	Fichier >> DonneeString;
		}
		if (TypeDonnee == "CHARGER_CHAPITRE")
		{
			Type = ACTION_CHAPITRE;		Fichier >> DonneeInt;
		}
		if (TypeDonnee == "TEXTE")
		{
			Type = ACTION_TEXTE;		Fichier >> DonneeInt;
			TypeClassement = CLASSEMENT_NUAGE;
		}
		if (TypeDonnee == "TEXTE_NORD")
		{
			Type = ACTION_TEXTE_N;		Fichier >> DonneeInt;
			TypeClassement = CLASSEMENT_NUAGE;
		}
		if (TypeDonnee == "TEXTE_SUD")
		{
			Type = ACTION_TEXTE_S;		Fichier >> DonneeInt;
			TypeClassement = CLASSEMENT_NUAGE;
		}
		if (TypeDonnee == "TEXTE_EST")
		{
			Type = ACTION_TEXTE_E;		Fichier >> DonneeInt;
			TypeClassement = CLASSEMENT_NUAGE;
		}
		if (TypeDonnee == "TEXTE_OUEST")
		{
			Type = ACTION_TEXTE_O;		Fichier >> DonneeInt;
			TypeClassement = CLASSEMENT_NUAGE;
		}
		if (TypeDonnee == "TEXTE_PERSO")
		{
			Type = ACTION_TEXTE_PERSO;	Fichier >> DonneeInt;
			TypeClassement = CLASSEMENT_NUAGE;
		}
		if (TypeDonnee == "TEXTE_PERSO_NORD")
		{
			Type = ACTION_TEXTE_P_N;	Fichier >> DonneeInt;
			TypeClassement = CLASSEMENT_NUAGE;
		}
		if (TypeDonnee == "TEXTE_PERSO_SUD")
		{
			Type = ACTION_TEXTE_P_S;	Fichier >> DonneeInt;
			TypeClassement = CLASSEMENT_NUAGE;
		}
		if (TypeDonnee == "TEXTE_PERSO_EST")
		{
			Type = ACTION_TEXTE_P_E;	Fichier >> DonneeInt;
			TypeClassement = CLASSEMENT_NUAGE;
		}
		if (TypeDonnee == "TEXTE_PERSO_OUEST")
		{
			Type = ACTION_TEXTE_P_O;	Fichier >> DonneeInt;
			TypeClassement = CLASSEMENT_NUAGE;
		}
		if (TypeDonnee == "FIN_ACTIONNEUR") FinActionneur = true;
		TypeDonnee = "";
	}
}

void Actionneur::Load_Dialogue()
{
	dialogue = new Dialog;
	string chemin = "actionneur_" + Partie.CarteCourante->Id + "_" + intToString(DonneeInt) + ".lng";
	dialogue->load(chemin);
}

void Actionneur::Disp(float RefX, float RefY)
{
	#ifdef DEBOGAGE
	if (Arguments.Masks)
		if (Abs(RefX - PosX) <= Options.ScreenW/2 && Abs(RefY - PosY) <= Options.ScreenH/2)
			Disp_Masks(RefX, RefY);
	#endif

	if (dialogue != NULL)
	{
		if (Type == ACTION_TEXTE_PERSO || Type == ACTION_TEXTE_P_N || Type == ACTION_TEXTE_P_S || Type == ACTION_TEXTE_P_E || Type == ACTION_TEXTE_P_O)
			dialogue->setPosition(Options.ScreenW/2, Options.ScreenH/2 - 80, true);
		else dialogue->setPosition(Options.ScreenW/2 - (RefX - PosX), Options.ScreenH/2 - (RefY - PosY) - 20, true);
		if (dialogue->display())
		{
			delete dialogue;
			dialogue = NULL;
		}
	}
	return;
}

void Actionneur::Disp_Masks(float RefX, float RefY)
{
	if (ModeCollision == MODE_COLLISION_CERCLE)
	{
		CircleShape MasqueCollision(RayonCollision);
		MasqueCollision.setPosition(Options.ScreenW/2 - (RefX - PosX) - RayonCollision, Options.ScreenH/2 + 12 - (RefY - PosY) - RayonCollision);
		MasqueCollision.setFillColor(Color(0, 0, 200, 200));
		Jeu.App.draw(MasqueCollision);
	}
	else
	{
		RectangleShape MasqueCollision(Vector2f(2*RayX, 2*RayY));
		MasqueCollision.setPosition(Options.ScreenW/2 - (RefX -PosX) - RayX, Options.ScreenH/2 + 12 - (RefY - PosY) - RayY);
		MasqueCollision.setFillColor(Color(0, 0, 200, 200));
		Jeu.App.draw(MasqueCollision);
	}
}
