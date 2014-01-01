
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
#include "../Bibliotheque/Bibliotheque.h"
#include "../Jeu/Jeu.h"
#include "ElementsCarte.h"


/** FONCTIONS DE LA CLASSE Individu **/

/* CONSTRUCTEURS / DESTRUCTEURS */

Individu::Individu() : Element_Mouvant()
{
	TypeClassement = CLASSEMENT_NORMAL;
}


/* GESTION */

/* La fonction Individu::Gestion() est développée dans le fichier AI.cpp */
/* La fonction Individu::MouvementAleatoire() est développée dans le fichier AI.cpp */
/* La fonction Individu::MouvementChasse() est développée dans le fichier AI.cpp */

void Individu::Set_Dir(int nv)
{
	if (nv != DIR_ALEATOIRE)
	{
		//if (Get_Caracs()->DirDisponibles[nv])
		if (Get_Activite(Act)->Num_Max[nv] != -1)
		{
			Dir = nv;
			if (Num > Get_Activite(Act)->Num_Max[Dir]) Num = 0;
		}
	}
	else
	{
		do
		{
			Dir = rand()%4;
		}
		//while(!Get_Caracs()->DirDisponibles[Dir]);
		while (Get_Activite(Act)->Num_Max[Dir] == -1);
		if (Num > Get_Activite(Act)->Num_Max[Dir]) Num = 0;
	}
}

void Individu::Gestion_Recuperation()
{
	if (Get_Recuperation() > 0)
	{
		if (100*buf_rec <= 2*Get_Recuperation())
		{
			Lag_Vitalite(1);
			if (Get_Recuperation() > 80) Lag_Vitalite(3);
			if (Get_Recuperation() > 90) Lag_Vitalite(6);
			if (Get_Recuperation() > 95) Lag_Vitalite(6);
			Lag_Energie(1);
		}
	}
	if (Get_Recuperation() < 0)
	{
		if (100*buf_rec <= -2*Get_Recuperation())
		{
			Lag_Vitalite(-1);
			Lag_Energie(-1);
		}
	}

	//Évolution de la récupération :

	//Test de récupération forcée (potion, …)
	if (Get_RecuperationMoyenne() >= 95 || Get_RecuperationMoyenne() <= -95) Set_Recuperation(Get_RecuperationMoyenne());

	//Évolution spontanée
	if (buf_rec >= 10)
	{
		//Récuperation trop faible par rapport à la vitalité :
		if (Get_Recuperation() < Get_RecuperationMoyenne() + (Get_Vitalite()-800)/20) Lag_Recuperation(1);
		//Récupération trop importante par rapport à la vitalité :
		if (Get_Recuperation() > Get_RecuperationMoyenne() + (Get_Vitalite()-800)/20) Lag_Recuperation(-1);

		//Augmentation de la Récupération automatique en fonction du niveau d'énergie
		if (Get_Energie() > 200) if (!rand()%20) Lag_Recuperation(1);
		if (Get_Energie() > 900) if (!rand()%15) Lag_Recuperation(1);
		buf_rec = 0;
	}
	else buf_rec += I(1);
}

/* GETTERS STATS ET CARACS */

float Individu::Get_Vitalite()
{
	return Get_Stats()->Vitalite;
}

float Individu::Get_Energie()
{
	return Get_Stats()->Energie;
}

float Individu::Get_Recuperation()
{
	return Get_Stats()->Recuperation;
}

unsigned int Individu::Get_Force()
{
	return 1./2. * Get_Caracs()->Force * (1. + 1.2*Get_Vitalite()/1000.);
}
unsigned int Individu::Get_Puissance()
{
	return 1./2. * Get_Caracs()->Puissance * (1. + 1.2*Get_Vitalite()/1000.);
}
unsigned short Individu::Get_Agilite()
{
	return 1./2. * Get_Caracs()->Agilite * (1. + 1.2*Get_Vitalite()/1000.);
}
unsigned short Individu::Get_Intelligence()
{
	return 1./2. * Get_Caracs()->Intelligence * (1. + 1.2*Get_Vitalite()/1000.);
}
unsigned short Individu::Get_Constitution()
{
	return 1./2. * Get_Caracs()->Constitution * (1. + 1.2*Get_Vitalite()/1000.);
}
unsigned short Individu::Get_Esquive()
{
	return 1./2. * Get_Caracs()->Esquive * (1. + 1.2*Get_Vitalite()/1000.);
}
unsigned short Individu::Get_Charisme()
{
	return 1./2. * Get_Caracs()->Charisme * (1. + 1.2*Get_Vitalite()/1000.);
}
int Individu::Get_RecuperationMoyenne()
{
	return Get_Caracs()->RecuperationMoyenne;
}

/* SETTERS DE VITALITÉ, ÉNERGIE ET RÉCUPÉRATION */

void Individu::Set_Vitalite(float vit)
{
	if (vit >= 0 && vit <= 1000) Get_Stats()->Vitalite = vit;
	if (vit > 1000) Get_Stats()->Vitalite = 1000;
	if (vit < 0) Get_Stats()->Vitalite = 0;
}

void Individu::Lag_Vitalite(float lag)
{
	if (lag < 0)
	{
		if (Get_Vitalite() >= -lag) Get_Stats()->Vitalite += lag;
		else
		{
			Set_Vitalite(0);
			Set_Activite(MORT);
		}
	}
	if (lag > 0) Get_Stats()->Vitalite += lag;

	if (Get_Vitalite() > 1000) Set_Vitalite(1000);
}

void Individu::Set_Energie(float ene)
{
	if (ene >= 0 && ene <= 1000) Get_Stats()->Energie = ene;
}

void Individu::Lag_Energie(float lag)
{
	if (lag < 0)
	{
		if (Get_Energie() >= (unsigned)(-lag)) Get_Stats()->Energie += lag;
		else Set_Energie(0);
	}
	if (lag > 0) Get_Stats()->Energie += lag;

	if (Get_Energie() > 1000) Set_Energie(1000);
}

void Individu::Set_Recuperation(float rec)
{
	if (rec <= -100) Get_Stats()->Recuperation = -100;
	else if (rec >= 100) Get_Stats()->Recuperation = 100;
	else Get_Stats()->Recuperation = rec;
}

void Individu::Lag_Recuperation(float lag)
{
	Get_Stats()->Recuperation += lag;
	if (Get_Recuperation() < -100) Set_Recuperation(-100);
	if (Get_Recuperation() > 100) Set_Recuperation(100);
}

void Individu::Disp(float RefX, float RefY)
{
	if (Controle == AI_IMG_HORSCHAMP) return;

	#ifdef DEBOGAGE
	if (Arguments.Masks)
		if (Abs(RefX - PosX) <= Options.ScreenW/2 && Abs(RefY - PosY) <= Options.ScreenH/2)
			Disp_Masks(RefX, RefY);
	#endif

	Disp_ImageIndividu(Type, Act, Dir, Num, Options.ScreenW/2 - (RefX - PosX), Options.ScreenH/2 + 12 - (RefY - PosY), true);
}
