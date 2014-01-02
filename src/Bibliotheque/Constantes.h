
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

//Version du jeu
#define VERSION "master"

//Constantes de débogage
//#define DEBOGAGE 1

#define FICHIER	1
#define IMAGE	2
#define LISTE	3
#define LUA		4

//Macro for Verbose mode
#ifdef DEBOGAGE
  #define MESSAGE(a, b) Verbose(a, b);
#else
  #define MESSAGE(a, b) ;
#endif

//Value used to say "not found"
#define MAX_VALUE 2147483647

//Constantes pour le choix du menu principal
#define NOUVEAU 1
#define CHARGER 2
#define OPTIONS 3
#define QUITTER 4

//Touches
#define HAUT		0
#define BAS			1
#define DROITE		2
#define GAUCHE		3

/** CONSTANTES RELATIVES AUX ÉLÉMENTS **/

//Mode de Collision
#define MODE_COLLISION_CERCLE	0
#define MODE_COLLISION_RECT		1

//Controle de l'individu
#define AUCUN		0
#define HUMAIN		1
#define AI			2
#define ARRET		3
#define AI_IMG_HORSCHAMP 5
//#define RESEAU	-1

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
#define STOP		0
#define MARCHE 		1
#define PAUSE		2
#define COURSE		3
#define ATTAQUE		4
#define BLESSE		5
#define MORT		6
#define ACTIVATION	7

//Identificateurs Directions et Images
#define DIR_ALEATOIRE -1
#define NORD			0
#define SUD				1
#define EST				2
#define OUEST			3
#define N_E				4
#define N_O				5
#define S_E				6
#define S_O				7

//Comportements des individus
#define COMPORTEMENT_ALEATOIRE	10
#define COMPORTEMENT_DEFENSE	11
#define COMPORTEMENT_CHASSE		12
#define COMPORTEMENT_ATTAQUE	13
#define COMPORTEMENT_REGEN		14

//Numéros de classe pour les paysages, types pour les Elements spéciaux
#define CLASSE_PAYSAGE	0
#define TYPE_ACTIONNEUR	1001
#define TYPE_COFFRE		1002
#define TYPE_CADAVRE	1003

//Types des Actionneurs
#define ACTION_TERMINEE		0
#define ACTION_INTERACTION	1
#define ACTION_RECUP		2
#define ACTION_COLL_DIPLOM	3
#define ACTION_COLL_CLASSE	4
#define ACTION_COLL_UNIQUE	5
#define ACTION_LIEU			7
#define ACTION_VILLAGE		8
#define ACTION_DEFENSE		9
#define ACTION_CGMT_CARTE	10
#define ACTION_CHAPITRE		11
#define ACTION_TEXTE		12
#define ACTION_TEXTE_N		13
#define ACTION_TEXTE_S		14
#define ACTION_TEXTE_E		15
#define ACTION_TEXTE_O		16
#define ACTION_TEXTE_PERSO	17
#define ACTION_TEXTE_P_N	18
#define ACTION_TEXTE_P_S	19
#define ACTION_TEXTE_P_E	20
#define ACTION_TEXTE_P_O	21


/** CONSTANTES RELATIVES AUX OBJETS **/

//Clefs d'inventaire
#define CLEF_INVENTAIRE	100
#define CLEF_STUFF		200
#define CLEF_COFFRE		300

//Images des objets
#define OBJET_IMG		0
#define OBJET_MINIATURE	1


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
#define COLL_INTER_ARR	5
#define COLL_ATT		6
#define COLL_ATT_ARR	7
#define COLL_VIS		8

//Action de l'utilisateur & états des individus
#define ACTION_JEU		0
#define ACTION_QUITTER	1
#define ACTION_PAUSE	2
#define ACTION_PERSO	3
#define ACTION_EQUIP	4
#define ACTION_SAUVEG	5
#define ACTION_REPOS	6
#define ACTION_MORT		7
#define ETAT_NORMAL		8
#define ETAT_DESACTIVE	9
#define ETAT_MORT		10
#define ETAT_CONTINUER	11

//Compétences
#define NOMBRE_COMPETENCES	4
#define COMPETENCE_AUCUNE	-1
#define COMPETENCE_CTRL		0
#define COMPETENCE_SHIFT	1
#define COMPETENCE_TAB		2
#define COMPETENCE_SPACE	3

#define TYPE_COMP_ARME		1
#define TYPE_COMP_PRJ		3
#define TYPE_COMP_OBJ		4

#endif
