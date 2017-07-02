
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

#include "tools/debug.h"

#include "../Bibliotheque/Constantes.h"
#include "../Jeu/Jeu.h"
#include "Carte.h"
#include "../ElementsCarte/ElementsCarte.h"



void addCommonClass(string type)
{
	Classe_Commune _new;
	Partie.commonClasses.insert(map<string, Classe_Commune>::value_type(type, _new));

	MESSAGE("Classe Commune " + type + " ajoutée", LISTE)
}

void deleteCommonClasses()
{
	Partie.commonClasses.clear();

	MESSAGE("Liste des Classes Communes supprimée", LISTE)
}

Classe_Commune* getCommonClass(string type)
{
	auto i = Partie.commonClasses.find(type);

	if (i != Partie.commonClasses.end()) return &(i->second);

	return NULL;
}


void addLandsClass(string type)
{
	Paysage _new;
    _new.Type = type;
	Partie.landsClasses.insert(map<string, Paysage>::value_type(type, _new));

	MESSAGE("Classe Paysage " + type + " ajoutée", LISTE)
}

void deleteLandsClasses()
{
	Partie.landsClasses.clear();

	MESSAGE("Liste des Classes Paysages supprimée", LISTE)
}

Paysage* getLandsClass(string type)
{
	auto i = Partie.landsClasses.find(type);

	if (i != Partie.landsClasses.end()) return &(i->second);

	return NULL;
}

void copyInertItemFromTemplate(string t, Paysage *elem)
{
    Paysage* itemTemplate = getLandsClass(t);
    if (itemTemplate == NULL)
    {
        tools::debug::error("Cannot initiate inertItem: template does not exist.", "lists");
        return;
    }

    elem->size = itemTemplate->size;
    //Restore size origin, as the copy breaks the link.
    elem->size.setOrigin(&elem->position());

	elem->TypeClassement = itemTemplate->TypeClassement;
	elem->Diplomatie = 0;
}
