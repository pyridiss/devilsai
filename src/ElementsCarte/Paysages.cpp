
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

#include "../Bibliotheque/Bibliotheque.h"
#include "../Bibliotheque/Constantes.h"
#include "../Jeu/Jeu.h"
#include "ElementsCarte.h"

#include "imageManager/imageManager.h"
#include "imageManager/image.h"

/** FONCTIONS DE LA CLASSE Paysage **/

Paysage::Paysage() : Element_Carte()
{
	TypeClassement = CLASSEMENT_NORMAL;
}

Paysage::~Paysage()
{
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

void Paysage::calculateCollisionRadius()
{
	if (repeatX == 1 && repeatY == 1) return;
	if (collisionType == NoCollision) return;

	Vector2u imageDimension = imageManager::getImage("paysage", Type)->getSize();
	int border = 0;

	if (repeatX != 1)
	{
		if (collisionType == RectangleCollision) border = RayX - imageDimension.x/2;
		if (collisionType == CircleCollision) border = RayonCollision - imageDimension.x/2;
		RayX = repeatX * imageDimension.x / 2 + border;
	}

	if (repeatY != 1)
	{
		if (collisionType == RectangleCollision) border = RayY - imageDimension.y/2;
		if (collisionType == CircleCollision) border = RayonCollision - imageDimension.y/2;
		RayY = repeatY * imageDimension.y / 2 + border;
	}
}

void Paysage::Disp(RenderTarget& target, float RefX, float RefY)
{
	if (Controle == AI_IMG_HORSCHAMP) return;

	if (repeatX == 1 && repeatY == 1)
	{
        imageManager::display(target, "paysage", Type, target.getSize().x/2 - (RefX - PosX), target.getSize().y/2 - (RefY - PosY), true);
	}
	else
	{
		Vector2u imageDimension = imageManager::getImage("paysage", Type)->getSize();
		if (repeatX > 1)
		{
			if (repeatX % 2 == 0)
				for (float i = -repeatX/2 ; i < repeatX/2 ; ++i)
                    imageManager::display(target, "paysage", Type, target.getSize().x/2 - (RefX - (PosX + (i+0.5)*imageDimension.x)), target.getSize().y/2 - (RefY - PosY), true);
			else
				for (float i = -(repeatX-1)/2 ; i <= (repeatX-1)/2 ; ++i)
                    imageManager::display(target, "paysage", Type, target.getSize().x/2 - (RefX - (PosX + i*imageDimension.x)), target.getSize().y/2 - (RefY - PosY), true);
		}
		else if (repeatY > 1)
		{
			if (repeatY % 2 == 0)
				for (float i = -repeatY/2 ; i < repeatY/2 ; ++i)
                    imageManager::display(target, "paysage", Type, target.getSize().x/2 - (RefX - PosX), target.getSize().y/2 - (RefY - (PosY + (i+0.5)*imageDimension.y)), true);
			else
				for (float i = -(repeatY-1)/2 ; i <= (repeatY-1)/2 ; ++i)
					imageManager::display(target, "paysage", Type, target.getSize().x/2 - (RefX - PosX), target.getSize().y/2 - (RefY - (PosY + i*imageDimension.y)), true);
		}
	}

	#ifdef DEBOGAGE
	if (Arguments.Masks)
		if (abs(RefX - PosX) <= Options.ScreenW/2 && abs(RefY - PosY) <= Options.ScreenH/2)
			Disp_Masks(RefX, RefY);
	#endif
}

void Paysage::Disp_Masks(float RefX, float RefY)
{
	if (collisionType == CircleCollision)
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

/** Class Door **/

Door::Door() : Element_Carte()
{
	TypeClassement = CLASSEMENT_NORMAL;
	collisionType = RectangleCollision;
}
Door::~Door()
{
}

int Door::Gestion()
{
	//Test de proximité au joueur
	int Retour = Element_Carte::Gestion();
	if (Retour != ETAT_CONTINUER) return Retour;

	return ETAT_NORMAL;
}

int Door::Collision(Individu *elem, int TypeCollision)
{
	if (TypeCollision == COLL_VIS) return COMPORTEMENT_ALEATOIRE;
	if (deniedDiplomacy == elem->Diplomatie) return COLL_PRIM;

	return COLL_OK;
}

void Door::Disp(RenderTarget& target, float RefX, float RefY)
{
	#ifdef DEBOGAGE
	if (Arguments.Masks)
		if (abs(RefX - PosX) <= Options.ScreenW/2 && abs(RefY - PosY) <= Options.ScreenH/2)
			Disp_Masks(RefX, RefY);
	#endif

	return;
}

void Door::Disp_Masks(float RefX, float RefY)
{
	RectangleShape MasqueCollision(Vector2f(2*RayX, 2*RayY));
	MasqueCollision.setPosition(Options.ScreenW/2 - (RefX -PosX) - RayX, Options.ScreenH/2 + 12 - (RefY - PosY) - RayY);
	MasqueCollision.setFillColor(Color(0, 200, 200, 200));
	Jeu.App.draw(MasqueCollision);
}


/** FONCTIONS DE LA CLASSE Classe_Paysage **/

void Classe_Paysage::Copie_Element(Paysage *elem)
{
	elem->collisionType = collisionType;
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
		if (Num == Get_Activite(Act)->numberOfImages) Num = 0;

		if (Num == 0 && Act == ACTIVATION) ActEffectue = true;
		else ActEffectue = false;
	}
}

int Paysage_Mouvant::Collision(Individu *elem, int TypeCollision)
{
	if (TypeCollision == COLL_VIS) return COMPORTEMENT_ALEATOIRE;
	return COLL_PRIM;
}

void Paysage_Mouvant::Disp(RenderTarget& target, float RefX, float RefY)
{
	if (Controle == AI_IMG_HORSCHAMP) return;

    Activite* act = Get_Activite(Act);
    imageManager::display(target, "movingObjects", act->getImageKey(angle, Num), target.getSize().x/2 - (RefX - PosX), target.getSize().y/2 - (RefY - PosY), true);

	#ifdef DEBOGAGE
	if (Arguments.Masks)
		if (abs(RefX - PosX) <= Options.ScreenW/2 && abs(RefY - PosY) <= Options.ScreenH/2)
			Disp_Masks(RefX, RefY);
	#endif
}

void Paysage_Mouvant::Disp_Masks(float RefX, float RefY)
{
	if (collisionType == CircleCollision)
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

void Classe_Paysage_Mouvant::Copie_Element(Element_Carte *elem)
{
	elem->collisionType = ModeCollision;
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
