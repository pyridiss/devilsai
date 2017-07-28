
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

#include <tinyxml2.h>

#include <lua5.2/lua.hpp>

#include "tools/filesystem.h"

#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/luaFunctions.h"
#include "../Jeu/Jeu.h"
#include "../Carte/Carte.h"
#include "ElementsCarte.h"

#include "gamedata.h"

using namespace tinyxml2;

/** FONCTIONS DE LA CLASSE Actionneur **/

Actionneur::Actionneur() : Element_Carte()
{
	Element_Carte::Type = "ACTIONNEUR";
    inert = true;
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

void Actionneur::loadFromXML(tinyxml2::XMLHandle &handle)
{
}

void Actionneur::saveToXML(XMLDocument& doc, XMLHandle& handle)
{
}

void Actionneur::Disp(RenderTarget& target)
{
    if (Options.displayShapes)
        size.display(target, Color(255, 0, 0, 50));
	return;
}


/** FONCTIONS DE LA CLASSE Trigger **/

Trigger::Trigger() : Element_Carte()
{
	Element_Carte::Type = "ACTIONNEUR";
    inert = true;
}
Trigger::~Trigger()
{
}

bool Trigger::activated()
{
    return tools::math::intersection(gamedata::player()->size, size);
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
    lua_getglobal(script, "collision");
    lua_pushlightuserdata(script, (void*)elem);
    lua_pushnumber(script, TypeCollision);
    lua_pushstring(script, data.c_str());
    lua_call(script, 3, 1);

    int value = lua_tonumber(script, -1);

    return value;
}

void Trigger::load(istream &Fichier, Carte *carte)
{
	bool endTrigger = false;
	string TypeDonnee = "";

	while (!endTrigger)
	{
		Fichier >> TypeDonnee;
		if (TypeDonnee == "type:")
		{
			Fichier >> triggerScript;
			if (carte->triggersScripts.find(triggerScript) == carte->triggersScripts.end())
			{
				lua_State* L = luaL_newstate();
				luaL_openlibs(L);
				luaL_dofile(L, (tools::filesystem::dataDirectory() + "shared_trigger/" + triggerScript + ".lua").c_str());
                carte->triggersScripts.emplace(triggerScript, L);

                lua_atpanic(L, LUA_panic);
                lua_register(L, "cout", LUA_cout);
                lua_register(L, "set", LUA_set);

                script = L;
			}
            else script = carte->triggersScripts.at(triggerScript);
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
			Fichier >> data;
		}
		if (TypeDonnee == "END_SHARED_TRIGGER")
			endTrigger = true;

		TypeDonnee = "";
	}
}

// void Trigger::Load_Dialogue()
// {
// 	dialogue = new Dialog;
// 	string chemin = "actionneur_" + Partie.CarteCourante->Id + "_" + intToString(DonneeInt) + ".lng";
// 	dialogue->load(chemin);
// }

void Trigger::loadFromXML(tinyxml2::XMLHandle &handle)
{
}

void Trigger::saveToXML(XMLDocument& doc, XMLHandle& handle)
{
}

void Trigger::Disp(RenderTarget& target)
{
    if (Options.displayShapes)
        size.display(target, Color(0, 255, 0, 50));
	return;
}
