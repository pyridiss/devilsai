
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

#ifndef header_constantes
#define header_constantes

using namespace std;

/** CONSTANTES RELATIVES AU JEU **/

//Value used to say "not found"
#define MAX_VALUE 2147483647


/** CONSTANTES RELATIVES AUX ÉLÉMENTS **/

//Diplomaties
#define DIPLOM_NEUTRE	0
#define DIPLOM_ALLIE	1
#define DIPLOM_ENNEMI	2
#define DIPLOM_BREAKABLE 3

//Types de classement
#define CLASSEMENT_SOL		0
#define CLASSEMENT_CADAVRE	1
#define CLASSEMENT_NORMAL	2
#define CLASSEMENT_HAUT		3
#define CLASSEMENT_NUAGE	4


/** CONSTANTES DIVERSES **/

//Retours de Collision
#define COLL_OK			0
#define COLL_END		1
#define COLL_PRIM		2
#define COLL_PRIM_MVT	3
#define COLL_INTER		4
#define COLL_ATT		6
#define COLL_LIFE       7
#define COLL_VIS		8

//Action de l'utilisateur & états des individus
#define ETAT_NORMAL		6
#define ETAT_MORT		8
#define ETAT_CONTINUER	9

#endif
