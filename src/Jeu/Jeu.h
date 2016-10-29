
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

#ifndef header_jeu
#define header_jeu

#define MAX_VALUE 2147483647

#include <string>
#include <list>

#include <SFML/System.hpp>

#include "../Bibliotheque/Bibliotheque.h"

using namespace std;
using namespace sf;

class Carte;
class Joueur;
class Coffre;
class Individu_Unique;
class Classe_Commune;
class Classe_Paysage;
class Classe_Paysage_Mouvant;

typedef map < enumPhrases, basic_string<Uint32> > Dictionnaire;
typedef map < int, int > MapDonneesInt;
typedef map < int, float > MapDonneesFloat;
typedef map < int, string > MapDonneesString;


/**
 * Contient les principales données concernant la partie en cours :
 * cartes, joueur, classes des éléments, éléments de collisions…
*/
struct Classe_Partie
{
	string SAVE = "";

	bool ModeCinematiques   = false;

	Carte *CarteCourante    = NULL;
	float PosCarteX = 0, PosCarteY = 0;

	Joueur *perso   = NULL;

	Journal journal;

	UserScreen screenCharacter;
	UserScreen screenEquipment;
	UserScreen screenSkills;
	UserScreen screenJournal;
	UserScreen *currentUserScreen = nullptr;

	Coffre *CoffreOuvert        = NULL;
	lua_State* selectedObject   = nullptr;
	lua_State* selectedSkill    = nullptr;

	map<string, Carte> maps;

	map<string, Classe_Commune> commonClasses;
	map<string, Classe_Paysage> landsClasses;
	map<string, Classe_Paysage_Mouvant> movingLandsClasses;

	bool loadFirstChapter   = false;

	list<Dialog> listDialogs;
	map<string, lua_State*> quests;

	public:
		void changeCurrentUserScreen(UserScreen* _new);
};

/**
 * Contient les données du programme : fenêtre d'affichage, polices, phrases, arguments et options. 
*/
struct Classe_Jeu
{
	RenderWindow App;
	Font Cardinal;
	Font DayRoman;
	Dictionnaire Dico;
	Time FrameTime;

	int intNotFound = MAX_VALUE;
	float floatNotFound = MAX_VALUE;

	struct Classe_Arguments
	{
		bool TabVerbose[6]      = {false};
		bool Masks              = false;
		bool LimitToFpsDisabled = false;
		bool SaveDisabled       = false;
	};

	struct Classe_Options
	{
		bool PleinEcran             = false;
		bool PleinEcran_Save        = false;
		bool SauvegardeDisponible	= false;
		bool AffichageDegats        = false;
		float VitesseJeu            = 60.f/1000.f;
		unsigned ScreenW            = 800;
		unsigned ScreenW_Save       = 800;
		unsigned ScreenH            = 600;
		unsigned ScreenH_Save       = 600;
		string Langue               = "en=";
		String32 CharForm;
		String32 CharEOL;
		String32 CharSpace;

		Classe_Options();
	};

	bool ErreurDetectee = false;

	//Utilisé par certains éléments pour déterminer leur comportement
	Individu_Unique *Ind_AI;

	Classe_Jeu();
	~Classe_Jeu();
};

//Objets globaux Jeu et Partie :
extern Classe_Jeu Jeu;
extern Classe_Partie Partie;
extern Classe_Jeu::Classe_Arguments Arguments;
extern Classe_Jeu::Classe_Options Options;


//Fonctions définies dans les fichiers dépendants de Jeu
void Load_Chapitre(int);

bool RechercheJoueur();
bool NouvellePartie();
void LectureSauvegardes();
bool PartieSauvegardee();
void EcranJeu(bool);
void Clean_Partie();

int MenuPrincipal();

void Save_Partie();
bool Load_Partie(string);

void AjouterSauvegarde();
string ChoixSauvegarde();
void MaJ_Sauvegarde();
void Save_Sauvegardes();

void Load_Options();
void EcranOptions();
void Save_Options();

void addQuest(string, string);
void manageQuests();
void saveQuests(ofstream&);
void loadQuestFromSavedGame(string, string);
void deleteQuests();

#endif
