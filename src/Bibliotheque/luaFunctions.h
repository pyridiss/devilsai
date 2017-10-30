
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

#ifndef header_luafunction
#define header_luafunction

using namespace std;

class lua_State;

int LUA_panic(lua_State*);
int LUA_cout(lua_State*);
int LUA_getElementById(lua_State*);
int LUA_getElementInteraction(lua_State*);
int LUA_isIndividu(lua_State*);
int LUA_combat(lua_State*);
int LUA_set(lua_State*);
int LUA_get(lua_State*);
int LUA_individual_copy(lua_State*);
int LUA_useObject(lua_State*);
int LUA_dispText(lua_State*);
int LUA_getQuantityOf(lua_State*);
int LUA_getNumberOfItemsByTag(lua_State*);
int LUA_getElement(lua_State*);
int LUA_pushDialog(lua_State*);
int LUA_popDialog(lua_State*);
int LUA_dialogDisplayed(lua_State*);
int LUA_interact(lua_State*);
int LUA_loadWorld(lua_State* L);
int LUA_deleteList(lua_State*);
int LUA_I(lua_State*);
int LUA_addCheckPoint(lua_State*);
int LUA_deleteElement(lua_State*);
int LUA_setActivity(lua_State*);
int LUA_possess(lua_State*);
int LUA_transferObject(lua_State*);
int LUA_questRunning(lua_State*);
int LUA_addSound(lua_State*);
int LUA_playSound(lua_State*);
int LUA_createIndividual(lua_State* L);
int LUA_changeCurrentSkill(lua_State* L);
int LUA_moveItemTo(lua_State* L);

#endif
