
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
#include "../Carte/Carte.h"
#include "../Jeu/Jeu.h"
#include "ElementsCarte.h"


/** FONCTIONS DE GESTION DE LA CLASSE Individu **/

int Individu::Gestion()
{
	//Test de proximité au joueur
	int Retour = Element_Carte::Gestion();
	if (Retour != ETAT_CONTINUER) return Retour;

	// 0. Vérifie que l'individu n'est pas mort...

	if (Get_Act() == MORT && Get_Num() == Get_Activite(Get_Act())->Num_Max[Dir]-1)
	{
		if (DureeCadavre <= 0)
			return ETAT_MORT;
		else
		{
			DureeCadavre -= I(1/60.); //App.GetFrameTime();
			return ETAT_NORMAL;
		}
	}
	if (Get_Act() != MORT && get("Vitalite") == 0)
	{
		if (!Set_Activite(MORT)) return ETAT_MORT;
		else return ETAT_NORMAL;
	}

	Gestion_Recuperation();

	//Gestion du temps :
	Retour = Element_Mouvant::Gestion();
	if (Retour != ETAT_CONTINUER) return Retour;


	if (Get_Act() == MORT)
	{
		Set_Activite(MORT); //Permet de gérer les situations spéciales (dépose des inventaires, etc…)
		IncrementNum();
		return ETAT_NORMAL;
	}

	if (NouveauComportement != -1) Comportement = NouveauComportement;
	if (ArreterChasse) Comportement = COMPORTEMENT_ALEATOIRE;

	int Resultat = COLL_OK;
	int EnAttente = 0;
	int Iteration = 0;
	Element_Carte* tmp1 = NULL, *tmp2 = NULL;

	Element_Carte *Elem = Get_Element(ElementVision);

	if (Elem == NULL)
	{
		ElementVision = -1;
		Comportement = COMPORTEMENT_ALEATOIRE;
	}

	while(Resultat != COLL_END)
	{
		Resultat = COLL_OK;

		//Aucun déplacement : pas besoin de tests ; on termine d'abord l'activité.
		//En revanche, s'il s'agit de la dernière image, on a besoin de vérifier le comportement suivant.
		if (!Set_Activite(COURSE) && !Set_Activite(MARCHE) && Get_Num() != Get_Activite(Act)->Num_Max[Dir])
		{
			//Si l'ActDefaut est autre chose que MARCHE ou COURSE, on essaye de lui mettre
			Set_Activite(ActDefaut);
			Resultat = COLL_END;
			EnAttente = -1;
			//En cas d'attaque, on cherche quand même la meilleure direction
			if (Act == ATTAQUE && Elem != NULL && !TestAngle(PosX, PosY, Dir, Elem->PosX, Elem->PosY, Get_NombreDir()))
			{
				Set_Dir(NORD);
				while (!TestAngle(PosX, PosY, Dir, Elem->PosX, Elem->PosY, Get_NombreDir()))
				{
					++Dir;
					if (Dir == Get_NombreDir()) break;
				}
			}
			break;
		}

		//Déplacement selon le Comportement de l'individu
		switch (Comportement)
		{
			case COMPORTEMENT_ALEATOIRE :	MouvementAleatoire(Iteration); break;
			case COMPORTEMENT_DEFENSE :		if (Collision_cercle_cercle(PosX, PosY, Get_ChampVision()/4,
					                                                    Elem->PosX, Elem->PosY, Elem->RayonCollision))
												MouvementAleatoire(Iteration);
											else
												MouvementChasse(Elem);
											break;
			case COMPORTEMENT_CHASSE :		if (Collision_cercle_cercle(PosX, PosY, Get_RayonInteraction(),
					                                                    Elem->PosX, Elem->PosY, Elem->RayonCollision))
											{
												Resultat = COLL_END; EnAttente = COLL_ATT;
												if (Elem->Diplomatie != DIPLOM_NEUTRE && Elem->Diplomatie != Diplomatie)
												{
													NouveauComportement = COMPORTEMENT_ATTAQUE;
												}
											}
											else
												MouvementChasse(Elem);
											break;
			case COMPORTEMENT_ATTAQUE :		break;
			case COMPORTEMENT_REGEN :		MouvementChasse(Elem); break;
		}

		//Test de collision dans la nouvelle position de l'individu
		NouveauComportement = -1;
		RaZ_Coll();
		while(Resultat != COLL_END && Resultat != COLL_PRIM)
		{
			Resultat = ParcoursCollisions(this);

			if (Resultat == COLL_PRIM_MVT) Resultat = COLL_PRIM;

			//Les Collisions INTER et INTER_ARR sont pour le moment inutiles ici
			if (Resultat == COLL_INTER || Resultat == COLL_INTER_ARR) Resultat = COLL_OK;

			if (Resultat == COLL_ATT_ARR && tmp1 == NULL)
			{
				//On cherche la direction qui va donner le bon angle
				tmp2 = Get_Current_Coll();
				Set_Dir(NORD);
				while (tmp2 != NULL && !TestAngle(PosX, PosY, Dir, tmp2->PosX, tmp2->PosY, Get_NombreDir()))
				{
					++Dir;
					if (Dir == Get_NombreDir()) break;
				}
				if (Dir != Get_NombreDir())
				{
					Resultat = COLL_ATT;
					NouveauComportement = COMPORTEMENT_ATTAQUE;
				}
				else Resultat = COLL_OK;
			}
			if (Resultat == COLL_ATT)
			{
				tmp2 = Get_Current_Coll();
				if (tmp1 == NULL || EnAttente == COLL_VIS) tmp1 = tmp2;
				EnAttente = COLL_ATT;
				NouveauComportement = COMPORTEMENT_ATTAQUE;
			}
			if (Resultat == COLL_VIS)
			{
				tmp2 = Get_Current_Coll();
				if (tmp2 == NULL) break;

				Resultat = tmp2->Collision(this, COLL_VIS); //Permet de définir, si nécessaire, le nouveau comportement

				if (Resultat > NouveauComportement)
				{
					NouveauComportement = Resultat;
					tmp1 = tmp2;
					EnAttente = COLL_VIS;
				}
				if (Resultat == NouveauComportement)
				{
					if (tmp1 == NULL) tmp1 = tmp2;

					if ((PosX-tmp2->PosX)*(PosX-tmp2->PosX)+(PosY-tmp2->PosY)*(PosY-tmp2->PosY) < (PosX-tmp1->PosX)*(PosX-tmp1->PosX)+(PosY-tmp1->PosY)*(PosY-tmp1->PosY))
						tmp1 = tmp2;

					EnAttente = COLL_VIS;
				}
			}
		} //Fin du while de parcours de la liste de collisions

		if (Resultat == COLL_PRIM)
		{
			//Le mouvement précédent a entraîné une collision primaire : retour en arrière.
			Lag_Pos(-DirToCoeff_X(Dir)*Get_Activite(Act)->MaJ, -DirToCoeff_Y(Dir)*Get_Activite(Act)->MaJ);

			if (Iteration == 4) //Aucun mouvement valable n'a été trouvé
			{
				Resultat = COLL_END;
				Set_Activite(PAUSE);
				break;
			}
		}

		++Iteration;
	}	//Fin du while d'AI

	if (EnAttente == COLL_VIS)
	{
		if (tmp1 != NULL) ElementVision = tmp1->Id;
		else ElementVision = tmp2->Id;
	}
	if (EnAttente == COLL_ATT && ElementVision != -1) 
	{
		Set_Activite(ATTAQUE);
	}
	if (EnAttente == 0) NouveauComportement = COMPORTEMENT_ALEATOIRE;

	--FinMouvement;
	IncrementNum();

	if (Get_Num() == 0 && Get_Act() == ATTAQUE)
	{
		Individu *ennemi = dynamic_cast<Individu*>(Elem);

		//On vérifie que l'ennemi est toujours à portée, avant de lancer le combat :
		if (ennemi != NULL)
		{
			if (Collision_cercle_cercle(PosX, PosY, Get_RayonInteraction(), ennemi->PosX, ennemi->PosY, ennemi->RayonCollision))
				Combat(this, ennemi);
			NouveauComportement = COMPORTEMENT_CHASSE;
		}
		else NouveauComportement = COMPORTEMENT_ALEATOIRE;
	}

	return ETAT_NORMAL;
}

void Individu::MouvementAleatoire(int Iteration)
{
	if (FinMouvement <= 0)
	{
		Set_Dir(DIR_ALEATOIRE);

		Num = 0;
		FinMouvement = 10 + (rand()%40);
		ArreterChasse = false;
	}
	//Pour éviter des pauses inutiles, on désactive le piétinement et l'arrêt de la chasse si l'individu
	//se met en PAUSE lors des mouvements aléatoires
	if (ActDefaut == PAUSE) ArreterChasse = false;

	Set_Activite(ActDefaut);

	switch (Iteration)
	{
		case 0 :	Lag_Pos(DirToCoeff_X(Dir)*Get_Activite(Act)->MaJ, DirToCoeff_Y(Dir)*Get_Activite(Act)->MaJ); break;
		case 1 :	Set_Dir((Dir + 2)%4); if (Dir % 2 == 0) Set_Dir(Dir+1); else Set_Dir(Dir-1);
					Lag_Pos(DirToCoeff_X(Dir)*Get_Activite(Act)->MaJ, DirToCoeff_Y(Dir)*Get_Activite(Act)->MaJ); break;
		case 2 :	if (Dir % 2 == 0) Set_Dir(Dir+1); else Set_Dir(Dir-1);
					Lag_Pos(DirToCoeff_X(Dir)*Get_Activite(Act)->MaJ, DirToCoeff_Y(Dir)*Get_Activite(Act)->MaJ); break;
		case 3 :	if (Dir%2) Set_Dir((Dir + 1)%4); else Set_Dir((Dir + 3)%4);
					Lag_Pos(DirToCoeff_X(Dir)*Get_Activite(Act)->MaJ, DirToCoeff_Y(Dir)*Get_Activite(Act)->MaJ); break;
	}
}

bool Individu::MouvementChasse(Element_Carte *elem)
{
	int buf = 0;
	int DirActuelle = Dir;
	int Iteration = 0;
	int coeff = 1;

	if (elem == NULL)
	{
		Erreur("Individu::MouvementChasse() a été appelée avec un élément NULL", "");
		return true;
	}

	//Première partie : Classement des directions et enregistrement des distances
	for(int dir=0 ; dir < Get_NombreDir() ; ++dir)
	{
		Set_Dir(dir);
		Lag_Pos(5*DirToCoeff_X(Dir)*Get_Activite(Act)->MaJ, 5*DirToCoeff_Y(Dir)*Get_Activite(Act)->MaJ);

		Dist[dir] = (PosX-elem->PosX)*(PosX-elem->PosX) + (PosY-elem->PosY)*(PosY-elem->PosY);
		//Afin d'éviter des changements de directions incessants, la direction actuelle a un bonus :
		if (dir == DirActuelle) Dist[dir] *= 0.95;

		switch(dir)
		{
			case NORD :
			case SUD :	if (elem->PosY != PosY) Devn[dir] = 100*((elem->PosX - PosX)/Abs(elem->PosY - PosY));
						else Devn[dir] = 0;
						break;
			case EST :
			case OUEST :	if (elem->PosX != PosX) Devn[dir] = 100*((elem->PosY - PosY)/Abs(elem->PosX - PosX));
							else Devn[dir] = 0;
							break;
			default : Devn[dir] = 0;
		}

		if (Devn[dir] > 40) Devn[dir] = 40;
		if (Devn[dir] < -40) Devn[dir] = -40;

		int i = dir, j = i;
		Pref[i]=i;
		while(j > 0)
		{
			--j;
			if (Dist[j] > Dist[i])
			{
				buf = Dist[i]; Dist[i] = Dist[j]; Dist[j] = buf;
				buf = Pref[i]; Pref[i] = Pref[j]; Pref[j] = buf;
				buf = Devn[i]; Devn[i] = Devn[j]; Devn[j] = buf;
			}
			--i;
		}
		Lag_Pos(-DirToCoeff_X(Dir)*Get_Activite(Act)->MaJ, -DirToCoeff_Y(Dir)*Get_Activite(Act)->MaJ);
	}

	int IndActuel = 0;
	while (Pref[IndActuel] != DirActuelle)
	{
		++IndActuel;
		if (IndActuel == Get_NombreDir()) break;
	}

	for ( ; Iteration < Get_NombreDir() ; ++Iteration)
	{
		Set_Dir(Pref[Iteration]);

		//Deuxième partie : Vérification que la route n'est pas trop rapidement bloquée
		float Deplacement = (DirToCoeff_X(Dir, Devn[Iteration])*Get_Activite(Act)->MaJ)*(DirToCoeff_X(Dir, Devn[Iteration])*Get_Activite(Act)->MaJ) +
							(DirToCoeff_Y(Dir, Devn[Iteration])*Get_Activite(Act)->MaJ)*(DirToCoeff_Y(Dir, Devn[Iteration])*Get_Activite(Act)->MaJ);
		float DistanceRestante = Dist[Iteration];
		coeff = 1;
		int FractionDist = 4;
		if (DistanceRestante < 10000) FractionDist = 8;

		//CETTE BOUCLE PEUT NE PAS AVOIR DE FIN
		while(DistanceRestante > Dist[Iteration]/FractionDist && DistanceRestante > 2*coeff*coeff*Deplacement)
		{
			Lag_Pos(DirToCoeff_X(Dir, Devn[Iteration])*coeff*Get_Activite(Act)->MaJ, DirToCoeff_Y(Dir, Devn[Iteration])*coeff*Get_Activite(Act)->MaJ);

			//Tests de collision :
			int Resultat = COLL_OK;
			RaZ_Coll();
			while(Resultat != COLL_END && Resultat != COLL_PRIM)
			{
				Resultat = ParcoursCollisions(this);

				//Annihile COLL_PRIM_MVT si c'est l'élément chassé qui est détecté
				if (Resultat == COLL_PRIM_MVT && Get_Current_Coll() == elem) Resultat = COLL_OK;
				
				//Les Collisions INTER et INTER_ARR sont pour le moment inutiles ici
				if (Resultat == COLL_INTER || Resultat == COLL_INTER_ARR) Resultat = COLL_OK;

				if (Resultat == COLL_PRIM_MVT && coeff <= 15) Resultat = COLL_PRIM;
				if (Resultat == COLL_PRIM_MVT && coeff > 15) Resultat = COLL_OK;
				if (Resultat == COLL_ATT) Resultat = COLL_OK;
			}
			if (Resultat == COLL_PRIM)
			{
				Lag_Pos(-DirToCoeff_X(Dir, Devn[Iteration])*coeff*Get_Activite(Act)->MaJ, -DirToCoeff_Y(Dir, Devn[Iteration])*coeff*Get_Activite(Act)->MaJ);
				Set_Dir(DirActuelle);
				break;
			}

			Lag_Pos(-DirToCoeff_X(Dir, Devn[Iteration])*coeff*Get_Activite(Act)->MaJ, -DirToCoeff_Y(Dir, Devn[Iteration])*coeff*Get_Activite(Act)->MaJ);
			DistanceRestante -= coeff*Deplacement;
			coeff *= 2;

			//On doit limiter la variable coeff qui pourrait, à cause des limitations de son type, pourrait
			//devenir nulle et pourrait transformer la boucle en boucle infinie…
			if (coeff > 10000) break;

			if (Pietinement) DistanceRestante = 0; //Court-circuite la boucle ; seul coeff=1 est ainsi testé
		}
		if (DistanceRestante <= Dist[Iteration]/FractionDist || DistanceRestante <= 2*coeff*coeff*Deplacement) break; //Mouvement OK !
	}

	//Pas de mouvement trouvé : PAUSE.
	if (Iteration >= Get_NombreDir())
	{
		Set_Activite(PAUSE);
		return false;
	}

	if (ObstacleRencontre)
	{
		--Iteration;
		ObstacleRencontre = false;
	}

	if (Iteration) ObstacleRencontre = true;

	//Mouvement correct trouvé :

	Set_Activite(COURSE);

	if (Pietinement && DerniersPas == 10)
	{
		Pietinement = false;
		for (int a = 0 ; a < 6 ; ++a) NombrePas[a] = 100;
	}

	if (Dir == DirActuelle) ++DerniersPas;

	if (Dir != DirActuelle && !Pietinement)
	{
		for (int a = 5 ; a >= 1 ; --a)
			NombrePas[a] = NombrePas[a-1];
		NombrePas[0] = DerniersPas;
		DerniersPas = 0;
		int a = 0;
		for ( ; a < 6 ; ++a)
			if (NombrePas[a] > 20) break;
		if (a == 6) Pietinement = true;
		DirPietinement = Pref[Iteration+1];
	}
	if (Pietinement)
	{
		ArreterChasse = true;
		FinMouvement = 10;
		Set_Dir(DIR_ALEATOIRE);
		for (int a = 0 ; a < 6 ; ++a) NombrePas[a] = 100;
		Pietinement = false;
	}

	Lag_Pos(DirToCoeff_X(Dir, Devn[Iteration])*Get_Activite(Act)->MaJ, DirToCoeff_Y(Dir, Devn[Iteration])*Get_Activite(Act)->MaJ);

	return true;
}
