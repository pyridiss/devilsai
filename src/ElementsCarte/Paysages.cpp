
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

#include "../Bibliotheque/Bibliotheque.h"
#include "../Bibliotheque/Constantes.h"
#include "../Jeu/Jeu.h"
#include "ElementsCarte.h"


/** FONCTIONS DE LA CLASSE Paysage **/

Paysage::Paysage() : Element_Carte()
{
	next = NULL;
	TypeClassement = CLASSEMENT_NORMAL;
}

Paysage::~Paysage()
{
	if (next != NULL) delete next;
	next = NULL;
}

int Paysage::Gestion()
{
	//Test de proximité au joueur
	int Retour = Element_Carte::Gestion();
	if (Retour != ETAT_CONTINUER) return Retour;

	return ETAT_NORMAL;
}

int Paysage::Collision(Individu *elem, int TypeCollision)
{
	if (TypeCollision == COLL_VIS) return COMPORTEMENT_ALEATOIRE;
	return COLL_PRIM;
}

void Paysage::Disp(float RefX, float RefY)
{
	if (Controle == AI_IMG_HORSCHAMP) return;

	Disp_ImagePaysage(Type, (float)Options.ScreenW/2 - (int)(RefX - PosX), (float)Options.ScreenH/2 + 12 - (int)(RefY - PosY), true);

	#ifdef DEBOGAGE
	if (Arguments.Masks)
		if (Abs(RefX - PosX) <= Options.ScreenW/2 && Abs(RefY - PosY) <= Options.ScreenH/2)
			Disp_Masks(RefX, RefY);
	#endif
}

void Paysage::Disp_Masks(float RefX, float RefY)
{
	if (ModeCollision == MODE_COLLISION_CERCLE)
	{
		CircleShape MasqueCollision(RayonCollision);
		MasqueCollision.setPosition(Options.ScreenW/2 - (RefX - PosX) - RayonCollision, Options.ScreenH/2 + 12 - (RefY - PosY) - RayonCollision);
		MasqueCollision.setFillColor(Color(255, 255, 255, 100));
		Jeu.App.draw(MasqueCollision);
	}
	else
	{
		RectangleShape MasqueCollision(Vector2f(2*RayX, 2*RayY));
		MasqueCollision.setPosition(Options.ScreenW/2 - (RefX -PosX) - RayX, Options.ScreenH/2 + 12 - (RefY - PosY) - RayY);
		MasqueCollision.setFillColor(Color(255, 255, 255, 100));
		Jeu.App.draw(MasqueCollision);
	}
}


/** FONCTIONS DE LA CLASSE Classe_Paysage **/

void Classe_Paysage::Copie_Element(Paysage *elem)
{
	elem->ModeCollision = ModeCollision;
	elem->RayonCollision = RayonCollision;
	elem->RayX = RayX;
	elem->RayY = RayY;
	elem->TypeClassement = TypeClassement;
	elem->Diplomatie = 0;
}


/** FONCTIONS DE LA CLASSE Paysage_Mouvant **/

Paysage_Mouvant::Paysage_Mouvant() : Element_Mouvant()
{
	Classe = NULL;
}
Paysage_Mouvant::~Paysage_Mouvant()
{
}

void Paysage_Mouvant::Activation()
{
	Set_Activite(ACTIVATION);
}

Activite* Paysage_Mouvant::Get_Activite(short act)
{
	return Classe->Get_Activite(act);
}

int Paysage_Mouvant::Gestion()
{
	//Test de proximité au joueur
	int Retour = Element_Carte::Gestion();
	if (Retour != ETAT_CONTINUER) return Retour;

	//Gestion du temps :
	Retour = Element_Mouvant::Gestion();
	if (Retour != ETAT_CONTINUER) return Retour;

	if (Classe->ActivationNecessaire) Set_Activite(STOP);
	else Set_Activite(MARCHE);

	IncrementNum();

	return ETAT_NORMAL;
}

void Paysage_Mouvant::IncrementNum(bool RaZ)
{
	if (RaZ)
	{
		Num = 0;
		ActEffectue = false;
	}
	else
	{
		++Num;
		if (Num == Get_Activite(Act)->Num_Max[Dir]) Num = 0;

		if (Num == 0 && Act == ACTIVATION) ActEffectue = true;
		else ActEffectue = false;
	}
}

int Paysage_Mouvant::Collision(Individu *elem, int TypeCollision)
{
	if (TypeCollision == COLL_VIS) return COMPORTEMENT_ALEATOIRE;
	return COLL_PRIM;
}

void Paysage_Mouvant::Disp(float RefX, float RefY)
{
	if (Controle == AI_IMG_HORSCHAMP) return;

	Disp_ImagePaysageMouvant(Type, Act, Num, Options.ScreenW/2 - (RefX - PosX), Options.ScreenH/2 + 12 - (RefY - PosY), true);

	#ifdef DEBOGAGE
	if (Arguments.Masks)
		if (Abs(RefX - PosX) <= Options.ScreenW/2 && Abs(RefY - PosY) <= Options.ScreenH/2)
			Disp_Masks(RefX, RefY);
	#endif
}

void Paysage_Mouvant::Disp_Masks(float RefX, float RefY)
{
	if (ModeCollision == MODE_COLLISION_CERCLE)
	{
		CircleShape MasqueCollision(RayonCollision);
		MasqueCollision.setPosition(Options.ScreenW/2 - (RefX - PosX) - RayonCollision, Options.ScreenH/2 + 12 - (RefY - PosY) - RayonCollision);
		MasqueCollision.setFillColor(Color(255, 255, 255, 100));
		Jeu.App.draw(MasqueCollision);
	}
	else
	{
		RectangleShape MasqueCollision(Vector2f(2*RayX, 2*RayY));
		MasqueCollision.setPosition(Options.ScreenW/2 - (RefX -PosX) - RayX, Options.ScreenH/2 + 12 - (RefY - PosY) - RayY);
		MasqueCollision.setFillColor(Color(255, 255, 255, 100));
		Jeu.App.draw(MasqueCollision);
	}
}

/** FONCTIONS DE LA CLASSE Classe_Paysage_Mouvant **/

Classe_Paysage_Mouvant::Classe_Paysage_Mouvant()
{
	next = NULL;
}
Classe_Paysage_Mouvant::~Classe_Paysage_Mouvant()
{
	if (next != NULL) delete next;
	next = NULL;
}

void Classe_Paysage_Mouvant::Copie_Element(Element_Carte *elem)
{
	elem->ModeCollision = ModeCollision;
	elem->RayonCollision = RayonCollision;
	elem->RayX = RayX;
	elem->RayY = RayY;
	elem->TypeClassement = TypeClassement;
	elem->Diplomatie = 0;
}

void Classe_Paysage_Mouvant::Ajouter_Activite(int Id)
{
	Activite act;
	Activites.insert(MapActivites::value_type(Id, act));
	MapActivites::iterator i = Activites.find(Id);
	i->second.Id = Id;
}

Activite* Classe_Paysage_Mouvant::Get_Activite(int Id)
{
	MapActivites::iterator i = Activites.find(Id);
	if (i == Activites.end()) return NULL;
	return &i->second;
}
