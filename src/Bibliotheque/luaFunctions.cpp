
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

#include <string>
#include <sstream>

#include <lua5.2/lua.hpp>

#include "../Carte/Carte.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "../Jeu/Jeu.h"
#include "Bibliotheque.h"


int LUA_panic(lua_State* L)
{
	Erreur("LUA PANIC: ", lua_tostring(L, -1));

	return 0;
}

int LUA_cout(lua_State* L)
{
	cout << "LUA says: " << lua_tostring(L, 1) << endl;
	return 0;
}

int LUA_getElementById(lua_State* L)
{
	lua_pushnumber(L, (long int)Get_Element(lua_tonumber(L, 1)));
	return 1;
}

int LUA_getElementInteraction(lua_State* L)
{
	Joueur* elem = (Joueur*)(int)lua_tonumber(L, 1);

	int inter = 0;
	if (elem != NULL && elem->ElementInteraction != -1) inter = (long int)Get_Element(elem->ElementInteraction);
	lua_pushnumber(L, inter);

	return 1;
}

int LUA_isIndividu(lua_State* L)
{
	long int id = lua_tonumber(L, 1);

	Individu* ind = dynamic_cast<Individu*>((Element_Carte*)id);

	if (ind != NULL) lua_pushboolean(L, true);
	else lua_pushboolean(L, false);

	return 1;
}

int LUA_collisionCCwithRayon(lua_State* L)
{
	long int a = lua_tonumber(L, 1);
	string rayA = lua_tostring(L, 2);
	long int b = lua_tonumber(L, 3);
	string rayB = lua_tostring(L, 4);
	Individu* indA = (Individu*)a;
	Individu* indB = (Individu*)b;

	bool result = false;
	if (rayA == "RayonInteraction" && rayB == "RayonInteraction")
		result = Collision_cercle_cercle(indA->PosX, indA->PosY, indA->Get_RayonInteraction(), indB->PosX, indB->PosY, indB->Get_RayonInteraction());
	if (rayA == "RayonInteraction" && rayB == "RayonCollision")
		result = Collision_cercle_cercle(indA->PosX, indA->PosY, indA->Get_RayonInteraction(), indB->PosX, indB->PosY, indB->RayonCollision);
	if (rayA == "RayonCollision" && rayB == "RayonInteraction")
		result = Collision_cercle_cercle(indA->PosX, indA->PosY, indA->RayonCollision, indB->PosX, indB->PosY, indB->Get_RayonInteraction());
	if (rayA == "RayonCollision" && rayB == "RayonCollision")
		result = Collision_cercle_cercle(indA->PosX, indA->PosY, indA->RayonCollision, indB->PosX, indB->PosY, indB->RayonCollision);

	lua_pushboolean(L, result);

	return 1;
}

int LUA_testAngle(lua_State* L)
{
	long int a = lua_tonumber(L, 1);
	long int b = lua_tonumber(L, 2);
	Individu* indA = dynamic_cast<Individu*>((Element_Carte*)a);
	Individu* indB = dynamic_cast<Individu*>((Element_Carte*)b);

	bool result = false;

	if (indA != NULL && indB != NULL)
		result = TestAngle(indA->PosX, indA->PosY, indA->Get_Dir(), indB->PosX, indB->PosY, indA->Get_NombreDir());

	lua_pushboolean(L, result);

	return 1;
}

int LUA_combat(lua_State* L)
{
	long int a = lua_tonumber(L, 1);
	long int b = lua_tonumber(L, 2);
	Individu* indA = dynamic_cast<Individu*>((Element_Carte*)a);
	Individu* indB = dynamic_cast<Individu*>((Element_Carte*)b);

	Combat(indA, indB, L);

	return 0;
}

int LUA_createProjectile(lua_State* L)
{
	string type = lua_tostring(L, 1);
	Projectile prj;
	prj.Type = type;
	Projectile *ptr = Partie.CarteCourante->AjouterProjectile(prj);
	int result = 0;
	if (ptr != NULL) result = (long int)ptr;

	lua_pushnumber(L, result);
	return 1;
}

int LUA_set(lua_State* L)
{
	long int a = lua_tonumber(L, 1);
	string field = lua_tostring(L, 2);
	double value = lua_tonumber(L, 3);

	Element_Carte*	elem = (Element_Carte*)a;
	Individu*		ind  = dynamic_cast<Individu*>(elem);
	Projectile*		prj  = dynamic_cast<Projectile*>(ind);

	if (field == "vitality")		if (ind != NULL) ind->Set_Vitalite(value);
	if (field == "energy")			if (ind != NULL) ind->Set_Energie(value);
	if (field == "recuperation")	if (ind != NULL) ind->Set_Recuperation(value);
	if (field == "Diplomatie")		if (elem != NULL) elem->Diplomatie = (int)value;
	if (field == "Controle")		if (elem != NULL) elem->Set_Controle((int)value);
	if (field == "RayX")			if (elem != NULL) elem->RayX = (int)value;
	if (field == "RayY")			if (elem != NULL) elem->RayY = (int)value;
	if (field == "Num_Max_0")		if (ind != NULL) ind->Get_Activite(ind->Get_Act())->Num_Max[0] = (int)value;
	if (field == "Num_Max_1")		if (ind != NULL) ind->Get_Activite(ind->Get_Act())->Num_Max[1] = (int)value;
	if (field == "Num_Max_2")		if (ind != NULL) ind->Get_Activite(ind->Get_Act())->Num_Max[2] = (int)value;
	if (field == "Num_Max_3")		if (ind != NULL) ind->Get_Activite(ind->Get_Act())->Num_Max[3] = (int)value;
	if (field == "Num_Max_4")		if (ind != NULL) ind->Get_Activite(ind->Get_Act())->Num_Max[4] = (int)value;
	if (field == "Num_Max_5")		if (ind != NULL) ind->Get_Activite(ind->Get_Act())->Num_Max[5] = (int)value;
	if (field == "Num_Max_6")		if (ind != NULL) ind->Get_Activite(ind->Get_Act())->Num_Max[6] = (int)value;
	if (field == "Num_Max_7")		if (ind != NULL) ind->Get_Activite(ind->Get_Act())->Num_Max[7] = (int)value;
	if (field == "Vitesse")			if (prj != NULL) prj->Deplacement.speed = value;
	if (field == "MaJ")				if (prj != NULL) prj->Deplacement.step = value;
	if (field == "ChampAttaque")	if (prj != NULL) prj->ChampAttaque = value;
	if (field == "Force")			if (ind != NULL) (*ind->Get_Caracs())["Force"] = value;
	if (field == "Puissance")		if (ind != NULL) (*ind->Get_Caracs())["Puissance"] = value;
	if (field == "Agilite")			if (ind != NULL) (*ind->Get_Caracs())["Agilite"] = value;
	if (field == "Intelligence")	if (ind != NULL) (*ind->Get_Caracs())["Intelligence"] = value;
	if (field == "Dir")				if (ind != NULL) ind->Set_Dir(value);
	if (field == "OrigineX")		if (prj != NULL) prj->OrigineX = value;
	if (field == "OrigineY")		if (prj != NULL) prj->OrigineY = value;
	if (field == "PosX")			if (elem != NULL) elem->PosX = value;
	if (field == "PosY")			if (elem != NULL) elem->PosY = value;

	return 0;
}

int LUA_get(lua_State* L)
{
	float result = 0;

	long int a = lua_tonumber(L, 1);
	string field = lua_tostring(L, 2);

	Element_Carte* elem = (Element_Carte*)a;
	Individu* ind = dynamic_cast<Individu*>(elem);
	Individu_Unique* ind_unique = dynamic_cast<Individu_Unique*>(elem);

	//First, we get statistics and characteristics
	if (ind_unique != NULL)	result = ind_unique->get(field);
	else if (ind != NULL)	result = ind->get(field);

	if (result == Jeu.floatNotFound)
	{
		if (field == "PosX")	if (ind != NULL) result = ind->PosX;
		if (field == "PosY")	if (ind != NULL) result = ind->PosY;
		if (field == "Dir")		if (ind != NULL) result = ind->Get_Dir();
	}

	lua_pushnumber(L, result);
	return 1;
}

int LUA_dirToCoeff(lua_State *L)
{
	string coord = lua_tostring(L, 1);
	int dir = lua_tonumber(L, 2);

	double result = 0;
	if (coord == "X") result = DirToCoeff_X(dir);
	if (coord == "Y") result = DirToCoeff_Y(dir);

	lua_pushnumber(L, result);
	return 1;
}

int LUA_useObject(lua_State* L)
{
	long int a = lua_tonumber(L, 1);
	string object = lua_tostring(L, 2);

	Individu_Unique* ind = (Individu_Unique*)a;
	mapObjects* objects = &(ind->Get_Caracs()->objects.objects);

	mapObjects::iterator i = objects->begin();
	string key = intToString(CLEF_INVENTAIRE);
	while (i != objects->end() && (getStringFromLUA(i->second, "getFileName") != object || i->first[0] != key[0])) ++i;

	if (i != objects->end())
	{
		mapObjects::iterator j = objects->find(getStringFromLUA(i->second, "getIdEmplacement"));
		//On vérifie que l'emplacement correspondant dans l'Équipement est vide
		if (j == objects->end())
		{
			ind->Get_Caracs()->objects.addObject(getStringFromLUA(i->second, "getFileName"), getStringFromLUA(i->second, "getIdEmplacement"));

			j = objects->find(getStringFromLUA(i->second, "getIdEmplacement"));
			setIntToLUA(j->second, "setQuantite", 1);

 			mapObjects::iterator i = objects->begin();
			while (i != objects->end() && getStringFromLUA(i->second, "getFileName") != object) ++i;

			if (getIntFromLUA(i->second, "getQuantite") > 1)
				setIntToLUA(i->second, "setQuantite", getIntFromLUA(i->second, "getQuantite") - 1);
			else
			{
				ind->Get_Caracs()->objects.deleteObject(i->second);
				objects->erase(i);
			}
		}
	}
	return 0;
}

int LUA_dispText(lua_State* L)
{
	Disp_Texte(lua_tostring(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), Color(255,255,255), 12.);
	return 0;
}

int LUA_getQuantityOf(lua_State* L)
{
	long int a = lua_tonumber(L, 1);
	string object = lua_tostring(L, 2);
	int result = 0;

	Individu_Unique* ind = (Individu_Unique*)a;
	mapObjects* objects = &(ind->Get_Caracs()->objects.objects);

	mapObjects::iterator i = objects->begin();
	string key = intToString(CLEF_INVENTAIRE);
	while (i != objects->end() && (getStringFromLUA(i->second, "getFileName") != object || i->first[0] != key[0])) ++i;

	if (i != objects->end())
		result = getIntFromLUA(i->second, "getQuantite");

	lua_pushnumber(L, result);
	return 1;
}

int LUA_getNumberEnemiesInList(lua_State* L)
{
	string list = lua_tostring(L, 1);
	int qty = 0;
	for (auto& tmp : Partie.CarteCourante->elements)
		if (tmp->Liste == list && tmp->Type != "TYPE_CADAVRE") ++qty;

	lua_pushnumber(L, qty);
	return 1;
}

int LUA_getElement(lua_State* L)
{
	Individu_Unique* ind = Get_IndividuUnique(lua_tostring(L, 1));
	//A quest can ask for an element already dead (KillStolas for example)
//	if (ind == NULL) Erreur("Un élément non chargé ou non Individu_Unique a été demandé", "");

	lua_pushnumber(L, (long int)ind);
	return 1;
}

int LUA_pushDialog(lua_State* L)
{
	string newDialog = lua_tostring(L, 1);
	Dialog dialog;
	Partie.listDialogs.push_back(dialog);
	Partie.listDialogs.back().load(newDialog + ".lng");
	return 0;
}

int LUA_popDialog(lua_State* L)
{
	string dialog = lua_tostring(L, 1);
	for (auto i = Partie.listDialogs.begin() ; i != Partie.listDialogs.end() ; ++i)
		if (i->id == dialog + ".lng")
		{
			Partie.listDialogs.erase(i);
			break;
		}
	return 0;
}

int LUA_dialogDisplayed(lua_State* L)
{
	string dialog = lua_tostring(L, 1);
	for (auto& i : Partie.listDialogs)
		if (i.id == dialog + ".lng")
		{
			lua_pushboolean(L, false);
			return 1;
		}

	lua_pushboolean(L, true);
	return 1;
}

int LUA_collisionCC(lua_State* L)
{
	Individu_Unique* indA = (Individu_Unique*)(int)lua_tonumber(L, 1);
	Individu_Unique* indB = (Individu_Unique*)(int)lua_tonumber(L, 2);
	bool result = Collision_cercle_cercle(indA->PosX, indA->PosY, indA->RayonCollision+2, indB->PosX, indB->PosY, indB->RayonCollision+2);
	lua_pushboolean(L, result);
	return 1;
}

int LUA_collisionCR(lua_State* L)
{
	Element_Carte* indA = (Element_Carte*)(int)lua_tonumber(L, 1);
	Element_Carte* indB = (Element_Carte*)(int)lua_tonumber(L, 2);
	bool result = Collision_cercle_rectangle(indA->PosX, indA->PosY, indA->RayonCollision+2, indB->PosX, indB->PosY, indB->RayX+2, indB->RayY+2);
	lua_pushboolean(L, result);
	return 1;
}

int LUA_loadList(lua_State* L)
{
	string idList = lua_tostring(L, 1);
	Load_Carte(idList, TYPE_LISTE);
	Partie.CarteCourante->AjouterListeEnCollision(idList);
	return 0;
}

int LUA_deleteList(lua_State* L)
{
	Partie.CarteCourante->SupprimerListe(lua_tostring(L, 1));
	return 0;
}

int LUA_I(lua_State* L)
{
	lua_pushnumber(L, I(1./60.));
	return 1;
}

int LUA_addActionneur(lua_State* L)
{
	int x = lua_tonumber(L, 1);
	int y = lua_tonumber(L, 2);
	int w = lua_tonumber(L, 3);
	int h = lua_tonumber(L, 4);

	Actionneur *newActionneur = Partie.CarteCourante->AjouterActionneur("NO_SAVE", x, y);
	newActionneur->RayX = w;
	newActionneur->RayY = h;
	lua_pushnumber(L, (long int)newActionneur);
	return 1;
}

int LUA_deleteElement(lua_State* L)
{
	long int a = lua_tonumber(L, 1);
	Element_Carte* elem = (Element_Carte*)a;
	Partie.CarteCourante->SupprimerElement(elem);
	return 0;
}

int LUA_setActivity(lua_State* L)
{
	Individu_Unique* ind = (Individu_Unique*)(int)lua_tonumber(L, 1);

	if (ind != NULL) ind->Set_Activite(lua_tonumber(L, 2));

	return 0;
}

int LUA_possess(lua_State* L)
{
	Individu_Unique* ind = (Individu_Unique*)(int)lua_tonumber(L, 1);

	int object = lua_tonumber(L, 2);

	bool result = false;
	for (auto& i : ind->Get_Caracs()->objects.objects)
	{
		if (getIntFromLUA(i.second, "getInternalNumber") == object)
		{
			result = true;
			break;
		}
	}

	lua_pushboolean(L, result);
	return 1;
}

int LUA_transferObject(lua_State* L)
{
	Individu_Unique* indA = (Individu_Unique*)(int)lua_tonumber(L, 1);
	Individu_Unique* indB = (Individu_Unique*)(int)lua_tonumber(L, 2);

	int object = lua_tonumber(L, 3);

	mapObjects::iterator iObj = indA->Get_Caracs()->objects.objects.begin();

	//1. On place l'objet en question dans Partie.ObjetSelectionne
	while (iObj != indA->Get_Caracs()->objects.objects.end() && getIntFromLUA(iObj->second, "getInternalNumber") != object)
		++iObj;
	if (iObj == indA->Get_Caracs()->objects.objects.end()) return 0;

	Partie.selectedObject = iObj->second;
	setStringToLUA(iObj->second, "setKey", "0");
	indA->Get_Caracs()->objects.objects.erase(iObj);

	//2. On place l'objet dans l'inventaire du receveur
	iObj = indB->Get_Caracs()->objects.objects.begin();

	bool cumulate = false;

	if (getBoolFromLUA(Partie.selectedObject, "getCumul"))
	{
		string key = intToString(CLEF_INVENTAIRE);
		for (mapObjects::iterator k = indB->Get_Caracs()->objects.objects.begin() ; k != indB->Get_Caracs()->objects.objects.end() ; ++k)
			if (k->first[0] == key[0] && 
				getIntFromLUA(k->second, "getInternalNumber") == getIntFromLUA(Partie.selectedObject, "getInternalNumber"))
			{
				cumulate = true;
				iObj = k;
				break;
			}
	}

	if (cumulate)
	{
		setIntToLUA(iObj->second, "setQuantite", getIntFromLUA(iObj->second, "getQuantite") + getIntFromLUA(Partie.selectedObject, "getQuantite"));
		Partie.selectedObject = nullptr;
	}
	else
	{
		int cell = 0;
		string key = intToString(CLEF_INVENTAIRE);
		while (indB->Get_Caracs()->objects.objects.find(key) != indB->Get_Caracs()->objects.objects.end())
		{
			++cell;
			key = intToString(CLEF_INVENTAIRE + cell);
		}
		pair<mapObjects::iterator, bool> result = indB->Get_Caracs()->objects.objects.insert(mapObjects::value_type(key, Partie.selectedObject));
		if (result.second)
		{
			setStringToLUA(result.first->second, "setKey",key);
			Partie.selectedObject = nullptr;
		}
	}

	return 0;
}

int LUA_questRunning(lua_State* L)
{
	string quest = lua_tostring(L, 1);

	bool result = false;

	if (Partie.quests.find(quest) != Partie.quests.end())
		result = true;

	lua_pushboolean(L, result);
	return 1;
}

int LUA_loadElement(lua_State* L)
{
	//Be careful: only the last element is included in collision list
	stringstream stream;
	stream << lua_tostring(L, 1);
	string list = lua_tostring(L, 2);

	Element_Carte* elem = loadElementsFromStream(stream, Partie.CarteCourante, list);

	addCollider(elem);

	lua_pushnumber(L, (long int)elem);
	return 1;
}

int LUA_createActivite(lua_State* L)
{
	Individu_Unique* ind = (Individu_Unique*)(int)lua_tonumber(L, 1);
	int id = lua_tonumber(L, 2);

	ind->Ajouter_Activite(id);
	Activite* act = ind->Get_Activite(id);

	lua_pushnumber(L, (long int)act);
	return 1;
}

int LUA_activiteSet(lua_State* L)
{
	Activite* act = (Activite*)(int)lua_tonumber(L, 1);
	string field = lua_tostring(L, 2);
	int value = lua_tonumber(L, 3);

	if (field == "speed")		if (act != NULL)	act->speed = value;
	if (field == "step")		if (act != NULL)	act->step = value;
	if (field == "priority")	if (act != NULL)	act->priority = value;
	if (field == "numberOfDir")	if (act != NULL)
	{
		for (int i = 0 ; i < value ; ++i)
			act->Num_Max[i] = 0;
	}

	return 0;
}

int LUA_activiteAddImage(lua_State* L)
{
	Activite* act = (Activite*)(int)lua_tonumber(L, 1);
	int Act = lua_tonumber(L, 2);
	int Num = lua_tonumber(L, 3);

	if (act != NULL) act->addImage(Act, Num);

	return 0;
}

int LUA_addSound(lua_State* L)
{
	string sound = lua_tostring(L, 1);
	addSound(sound);
	return 0;
}

int LUA_playSound(lua_State* L)
{
	string sound = lua_tostring(L, 1);
	playSound(sound);
	return 0;
}