
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

//Constantes de débogage
#define DEBOGAGE 1

#define FICHIER	1
#define IMAGE	2
#define LISTE	3
#define LUA		4
#define MUSIC	5

//Macro for Verbose mode
#ifdef DEBOGAGE
  #define MESSAGE(a, b) Verbose(a, b);
#else
  #define MESSAGE(a, b) ;
#endif

//Value used to say "not found"
#define MAX_VALUE 2147483647

//Touches
#define HAUT		0
#define BAS			1
#define DROITE		2
#define GAUCHE		3

/** CONSTANTES RELATIVES AUX ÉLÉMENTS **/

//Diplomaties
#define DIPLOM_NEUTRE	0
#define DIPLOM_ALLIE	1
#define DIPLOM_ENNEMI	2

//Types de classement
#define CLASSEMENT_SOL		0
#define CLASSEMENT_CADAVRE	1
#define CLASSEMENT_NORMAL	2
#define CLASSEMENT_HAUT		3
#define CLASSEMENT_NUAGE	4

//Activités
#define STOP		"0"
#define MARCHE 		"1"
#define PAUSE		"2"
#define COURSE		"3"
#define ATTAQUE		"4"
#define BLESSE		"5"
#define MORT		"6"

//Comportements des individus
#define COMPORTEMENT_ALEATOIRE	10
#define COMPORTEMENT_DEFENSE	11
#define COMPORTEMENT_CHASSE		12
#define COMPORTEMENT_ATTAQUE	13
#define COMPORTEMENT_REGEN		14

//Numéros de classe pour les paysages, types pour les Elements spéciaux
#define TYPE_ACTIONNEUR	1001

//Types des Actionneurs
#define ACTION_RECUP		2
#define ACTION_LIEU			7	//	class Trigger
#define ACTION_VILLAGE		8	//	class Trigger
#define ACTION_DEFENSE		9
#define ACTION_CGMT_CARTE	10	//	class Trigger
#define ACTION_CHAPITRE		11	//	class Trigger
#define ACTION_TEXTE_PERSO	17	//	class Trigger
#define ACTION_TEXTE_P_N	18	//	class Trigger
#define ACTION_TEXTE_P_S	19	//	class Trigger
#define ACTION_TEXTE_P_E	20	//	class Trigger
#define ACTION_TEXTE_P_O	21	//	class Trigger


/** CONSTANTES RELATIVES AUX OBJETS **/

//Clefs d'inventaire
#define CLEF_INVENTAIRE	100
#define CLEF_STUFF		200
#define CLEF_COFFRE		300


/** CONSTANTES DIVERSES **/

//Types de carte
#define TYPE_CARTE	0
#define TYPE_LISTE	1

//Retours de Collision
#define COLL_OK			0
#define COLL_END		1
#define COLL_PRIM		2
#define COLL_PRIM_MVT	3
#define COLL_INTER		4
#define COLL_ATT		6
#define COLL_VIS		8

//Action de l'utilisateur & états des individus
#define ACTION_MORT		5
#define ETAT_NORMAL		6
#define ETAT_MORT		8
#define ETAT_CONTINUER	9

//Compétences
#define NOMBRE_COMPETENCES	4
#define COMPETENCE_AUCUNE	-1
#define COMPETENCE_CTRL		0
#define COMPETENCE_SHIFT	1
#define COMPETENCE_TAB		2
#define COMPETENCE_SPACE	3

#endif
