
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

#include "../Bibliotheque/Constantes.h"
#include "../Jeu/Jeu.h"
#include "../Carte/Carte.h"
#include "ElementsCarte.h"

#include "imageManager/imageManager.h"

/** FONCTIONS DE LA CLASSE Paysage_Lanceur **/

Paysage_Lanceur::Paysage_Lanceur()
{
	ActivationEnCours = false;
}

int Paysage_Lanceur::Gestion()
{
	//Test de proximité au joueur
	int Retour = Element_Carte::Gestion();
	if (Retour != ETAT_CONTINUER) return Retour;

	//Gestion du temps :
	Retour = Element_Mouvant::Gestion();
	if (Retour != ETAT_CONTINUER) return Retour;

    return ETAT_NORMAL;

	int Resultat = COLL_OK;
	Element_Carte* tmp = NULL;

	//1. Recherche de cible

	Partie.CarteCourante->resetCollisionManager();

	while(!ActivationEnCours && Resultat != COLL_PRIM_MVT)
	{
		Resultat = Partie.CarteCourante->browseCollisionList(Jeu.Ind_AI);

		if (Resultat == COLL_END)
		{
			return ETAT_NORMAL;
		}
		if (Resultat == COLL_PRIM_MVT) 
		{
			tmp = Partie.CarteCourante->getCurrentCollider();
			if (tmp != NULL && tmp->Diplomatie == ProjectileLance.Diplomatie)
				Resultat = COLL_OK;
		}
		if (Resultat == COLL_PRIM)
		{
			tmp = Partie.CarteCourante->getCurrentCollider();
			//On vérifie que le lanceur n'est pas devant un paysage qui ne laisse pas passer les missiles
			if (tmp != NULL && tmp->TypeClassement == CLASSEMENT_HAUT)
				ProjectileLance.ChampAttaque -= 10;
		}
	}

	//2. Cible trouvée

	ActivationEnCours = true;
	Activation();
	IncrementNum();

	if (!ActEffectue) return ETAT_NORMAL;

	//3. Création du projectile

	Partie.CarteCourante->AjouterProjectile(ProjectileLance);
	ActivationEnCours = false;
	Set_Activite(STOP);

	return ETAT_NORMAL;
}


/** FONCTIONS DE LA CLASSE Projectile **/

Activite* Projectile::Get_Activite(string act)
{
	return &Deplacement;
}

bool Projectile::Set_Activite(string nv)
{
	return true;
}

int Projectile::Gestion()
{
	//Test de proximité au joueur
	int Retour = Element_Carte::Gestion();
	if (Retour != ETAT_CONTINUER) return Retour;

	//Gestion du temps :
	Retour = Element_Mouvant::Gestion();
	if (Retour != ETAT_CONTINUER) return Retour;

	//Test de fin de course :

	//Déplacement et vérification de collision


	//On n'autorise pas à blesser un ennemi trop près de la position d'origine

	Element_Carte *tmp = NULL;
	int Resultat = COLL_OK;

	Partie.CarteCourante->resetCollisionManager();

	while(Resultat != COLL_PRIM_MVT)
	{
		Resultat = Partie.CarteCourante->browseCollisionList(this);

		if (Resultat == COLL_END)
		{
			return ETAT_NORMAL;
		}
		if (Resultat == COLL_PRIM_MVT) 
		{
			tmp = Partie.CarteCourante->getCurrentCollider();
			if (tmp != NULL && tmp->Diplomatie == Diplomatie)
				Resultat = COLL_OK;
		}
		if (Resultat == COLL_PRIM)
		{
			tmp = Partie.CarteCourante->getCurrentCollider();
			//On vérifie qu'on n'entre pas en collision avec un paysage HAUT
			if (tmp != NULL && tmp->TypeClassement == CLASSEMENT_HAUT)
				return ETAT_MORT;
		}
	}

	Individu *ennemi = dynamic_cast<Individu*>(tmp);
	Combat(this, ennemi);

	return ETAT_MORT;
}

void Projectile::Disp(RenderTarget& target, float RefX, float RefY)
{
	if (Controle == AI_IMG_HORSCHAMP) return;

}
