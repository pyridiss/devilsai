
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
#include <cmath>

#include <lua5.2/lua.hpp>

#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/luaFunctions.h"
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
	switch(Type)
	{
		case ACTION_RECUP :			if (TypeCollision == COLL_PRIM)
										elem->Set_Recuperation(100);
									if (TypeCollision == COLL_VIS)
									{
										if (elem->Comportement <= COMPORTEMENT_REGEN && elem->get("Vitalite") < 140)
											return COMPORTEMENT_REGEN;
									}
									break;
		case ACTION_DEFENSE :		if (elem->Comportement <= COMPORTEMENT_DEFENSE)
										return COMPORTEMENT_DEFENSE;
									break;
	}

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
            int a;
            Fichier >> a;
            size.circle(tools::math::Vector2d(0, 0), a);
            size.setOrigin(&position());
		}
		if (TypeDonnee == "RECT_COL")
		{
            int a, b;
            Fichier >> a >> b;
            size.rectangle(tools::math::Vector2d(-a, -b), tools::math::Vector2d(a, -b), tools::math::Vector2d(-a, b));
            size.setOrigin(&position());
		}
		if (TypeDonnee == "RECUPERATION")
		{
			Type = ACTION_RECUP;
		}
		if (TypeDonnee == "DEFENSE")
		{
			Type = ACTION_DEFENSE;		Fichier >> DonneeInt;
		}
		if (TypeDonnee == "FIN_ACTIONNEUR") FinActionneur = true;
		TypeDonnee = "";
	}
}

void Actionneur::Disp(RenderTarget& target)
{
	return;
}


/** FONCTIONS DE LA CLASSE Trigger **/

Trigger::Trigger() : Element_Carte()
{
	Element_Carte::Type = "ACTIONNEUR";
}
Trigger::~Trigger()
{
}

bool Trigger::activated()
{
    return tools::math::intersection(Partie.perso->size, size);
}

int Trigger::Gestion()
{
	if (activated() == false) return ETAT_NORMAL;

// 	switch(triggerFunction)
// 	{
// 		case ACTION_LIEU :	break;
// 		case ACTION_VILLAGE :
// 		case ACTION_CGMT_CARTE :
// 		case ACTION_CHAPITRE :
// 		case ACTION_TEXTE_PERSO :
// 		case ACTION_TEXTE_P_N :
// 		case ACTION_TEXTE_P_S :
// 		case ACTION_TEXTE_P_E :
// 		case ACTION_TEXTE_P_O : break;
// 	}

// 	if (Type == ACTION_CGMT_CARTE && elem == Partie.perso && Get_Carte(DonneeString) != NULL && DonneeString != Partie.CarteCourante->Id)
// 	{
// 		stopMusic(Partie.CarteCourante->ambience);
// 		Disp_Chargement(255);
// 
// 		//1. Suppression des collisions
// 		Partie.CarteCourante->resetCollisionManager();
// 
// 		//2. Changement de carte du personnage
// 		ChangerCarte(Partie.perso, Partie.CarteCourante->Id, DonneeString);
// 
// 		//3. Mise à jour de CarteCourante et des collisions
// 		Partie.CarteCourante = Get_Carte(DonneeString);
// 		Partie.CarteCourante->resetCollisionManager();
// 		playMusic(Partie.CarteCourante->ambience);
// 	}

	return ETAT_NORMAL;
}

int Trigger::Collision(Individu* elem, int TypeCollision)
{
	return COLL_OK;
}

void Trigger::load(istream &Fichier, Carte *carte)
{
	/**
SHARED_TRIGGER
type: change_place
position: 342 1
size: rectangle 1 100
argument: 1010
argument: village
END_SHARED_TRIGGER
**/
	bool endTrigger = false;
	string TypeDonnee = "";

	lua_State* L = NULL;// = luaL_newstate();
	//luaL_openlibs(L);

	while (!endTrigger)
	{
		Fichier >> TypeDonnee;
		if (TypeDonnee == "type:")
		{
			Fichier >> triggerFunction;
			if (carte->luaTriggers.find(triggerFunction) == carte->luaTriggers.end())
			{
				L = luaL_newstate();
				luaL_openlibs(L);
				luaL_dofile(L, (INSTALL_DIR + "shared_trigger/" + triggerFunction + ".lua").c_str());
				carte->luaTriggers.insert(map<string, lua_State*>::value_type(triggerFunction, L));
			}
			else L = carte->luaTriggers.at(triggerFunction);
			lua_getglobal(L, "newTriggerSetId");
			lua_pushnumber(L, Id);
			lua_call(L, 1, 0);
		}
		if (TypeDonnee == "position:")
        {
            double a, b;
			Fichier >> a >> b;
            move(a, b);
        }
		if (TypeDonnee == "size:")
		{
			string type;
			Fichier >> type;
			if (type == "rectangle")
			{
                int a, b;
                Fichier >> a >> b;
                size.rectangle(tools::math::Vector2d(-a, -b), tools::math::Vector2d(a, -b), tools::math::Vector2d(-a, b));
			}
			if (type == "circle")
			{
                int a;
                Fichier >> a;
                size.circle(tools::math::Vector2d(0, 0), a);
			}
		}
		if (TypeDonnee == "argument:")
		{
			string arg;
			Fichier >> arg;
			lua_getglobal(L, "newTriggerSetArgument");
			lua_pushstring(L, arg.c_str());
			lua_call(L, 1, 0);
		}
		if (TypeDonnee == "END_SHARED_TRIGGER")
			endTrigger = true;

		TypeDonnee = "";
	}

	lua_getglobal(L, "newTriggerEnd");
	lua_call(L, 0, 0);

	lua_atpanic(L, LUA_panic);

	lua_register(L, "cout", LUA_cout);
	lua_register(L, "triggerActivated", LUA_triggerActivated);
	lua_register(L, "changePlace", LUA_changePlace);
}

// void Trigger::Load_Dialogue()
// {
// 	dialogue = new Dialog;
// 	string chemin = "actionneur_" + Partie.CarteCourante->Id + "_" + intToString(DonneeInt) + ".lng";
// 	dialogue->load(chemin);
// }

void Trigger::Disp(RenderTarget& target)
{
	return;
}
