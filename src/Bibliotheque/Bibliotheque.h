
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

#ifndef header_bibliotheque
#define header_bibliotheque

#include <iostream>
#include <string>
#include <fstream>
#include <list>

#include "gui/style.h"
#include "gui/button.h"

using namespace std;
using namespace sf;

class lua_State;
class Element_Carte;
struct Paragraph;

//Type de base pour les string contenant des textes à afficher
typedef basic_string<Uint32> String32;
typedef list<String32> ListString32;
typedef list<Paragraph> ListParagraph;

enum enumPhrases{
	_TITRE = 100,
	_ERREUR,
	_CHOIX_SAUVEGARDE = 104,
	_LANCER_PARTIE,
	_SUPPRIMER_SAUVEGARDE,
	_SAUVEGARDE_VERSION,
	_VERSION_INCOMPATIBLE,
	_SAUVEGARDE = 110,
	_RETOUR_MENU = 114,
	_MENUPRINCIPAL_CHARGER = 201,
	_MENUPRINCIPAL_OPTIONS,
	_PAS_DE_SAUVEG = 208,
	_ACTIVE = 300,
	_DESACTIVE,
	_SUIVANT,
	_PRECEDENT,
	_LANGUE,
	_PLEINECRAN,
	_AFFICHAGE_DEGATS,
	_RESOLUTION,
	_DEMANDE_REDEMARRAGE,
	_RESOLUTION_IMCOMPATIBLE,
	_SANTE1 = 401,
	_SANTE2,
	_SANTE3,
	_SANTE4,
	_SANTE5,
	_MORT,
	_SANTE_FATIGUE,
	_MONSTRES_RESTANTS,
	_CADAVRE,
	_ENERGIE_INSUFFISANTE,
	_SOUS_EFFET,
	_ESQUIVE = 500,
	_CRITIQUE,
	_BLESSURE,
	_FATIGUE,
	_DEGATS_INFLIGES,
	_DEGATS_RECUS,
	_ECHEC,
	_AMELIORATION_FORCE = 600,
	_AMELIORATION_PUISS,
	_AMELIORATION_AGILITE,
	_AMELIORATION_INTELLI,
	_AMELIORATION_CONSTIT,
	_AMELIORATION_ESQUIVE,
	_AMELIORATION_CHARISM,
	_AMELIORATION_RECUP,
	_AMELIORATION_VIT_COURSE,
	_AMELIORATION_VIT_ATTAQUE,
	_AMELIORATION_VIT_BLESSE,
	_PERSO_NIVEAU = 700,
	_PERSO_VITALITE,
	_PERSO_ENERGIE,
	_PERSO_RECUP,
	_PERSO_EXP,
	_PERSO_DEGATS,
	_PERSO_FORCE,
	_PERSO_PUISS,
	_PERSO_AGILITE,
	_PERSO_INTELLI,
	_PERSO_CONSTIT,
	_PERSO_ESQUIVE,
	_PERSO_CHARISM,
	_PERSO_RECUPMOY,
	_EQUIP_VIDE = 800,
	_EQUIP_ARME_VIDE,
	_EQUIP_BOUCLIER_VIDE,
	_EQUIP_CASQUE_VIDE,
	_EQUIP_ARMURE_VIDE,
	_EQUIP_BOTTES_VIDE,
	_EQUIP_BAGUE_VIDE,
	_INVENTAIRE,
	_EQUIP_FORCE,
	_EQUIP_PUISS,
	_EQUIP_AGILITE,
	_EQUIP_INTELLI,
	_EQUIP_CONSTIT,
	_EQUIP_ESQUIVE,
	_EQUIP_CHARISM,
	_EQUIP_RECUP,
	_EQUIP_VIT_COURSE,
	_EQUIP_VIT_ATTAQUE,
	_EQUIP_VIT_BLESSE,
	_EQUIP_MULT_FORCE,
	_EQUIP_MULT_PUISS,
	_EQUIP_MULT_AGILITE,
	_EQUIP_MULT_INTELLI,
	_EQUIP_MULT_CONSTIT,
	_EQUIP_MULT_ESQUIVE,
	_EQUIP_MULT_CHARISM,
	_EQUIP_MULT_RECUP,
	_EQUIP_DUREE,
	_COMPETENCES = 900
};


/** CLASSES **/

struct LigneConsole
{
	String32 Ligne;
	Color Couleur;
	float Temps		= 0;
	short NumLigne	= 0;
	bool Affichage	= false;
};

struct Language
{
	string shortName;
	String32 name;
};

struct Paragraph
{
	public:
		String32 name;
		String32 characters;
		ListString32 lines;
		IntRect rectangle;
		bool centered = false;

	public:
		Paragraph();
};

class JournalEntry : public Paragraph
{
	public:
		string reference = "";
		bool done = false;

	public:
		JournalEntry();
};

class Journal
{
	public:
		list<JournalEntry> entries;
		bool newEntryAdded = false;

	public:
		void addEntry(string _ref);
		void setDone(string _ref);
};

class Dialog
{
	private:
		ListParagraph::iterator paragraphNumber;
		float duration = 0;
		ListParagraph paragraphs;

	public:
		string id = "";

	public:
		void load(string str);
		bool display();
		void setPosition(int x, int y, bool c);

	private:
		void unload();
};

class UserScreen
{
	public:
		gui::Button button;
		Keyboard::Key key;

	public:
		void (*dispFunction)(void);
		void (*manageFunction)(Event&);
};


/** DÉCLARATION DES FONCTIONS **/

/* Fonctions définies dans le fichier Decorations.cpp */

void Load_Decorations();
void Supprimer_Decorations();
void Disp_FPS();
void Disp_FondMenus();
void Gestion_Menu(Event &event);
void Disp_Menu();
void Disp_JaugesVie();
void Ajouter_LignePerso(String32, Color);
void Ajouter_LigneAmelioration(String32, Color);
void Disp_Consoles();
void SupprimerLignesConsoles();
void Disp_Information(enumPhrases, bool);
void Disp_FonduNoir(int);
bool Disp_Repos();

/* Fonctions définies dans le fichier Langues.cpp */

void loadAvailableLanguages();
void deleteLanguagesList();
void loadDevilsaiMessages();
String32 getTranslatedNameOfElement(string);
String32 getTranslatedNameOfPlace(int);
String32 getTranslatedNameOfObject(int);
String32 getTranslatedNameOfObject(string);
String32 getTranslatedNameOfSkill(string);
String32 getTranslatedDescriptionOfObject(int);
String32& getTranslatedMessage(enumPhrases);
String32 getFormatedTranslatedMessage(enumPhrases, String32);
void changeLanguage();
String32& getNameOfLanguage();
void cutParagraph(Paragraph*);
void displayJournal();

/* Fonctions définies dans le fichier Utilitaires.cpp */

bool getBoolFromLUA(lua_State*, string);
int getIntFromLUA(lua_State*, string);
double getDoubleFromLUA(lua_State*, string);
string getStringFromLUA(lua_State*, string);
void setBoolToLUA(lua_State*, string, bool);
void setIntToLUA(lua_State*, string, int);
void setDoubleToLUA(lua_State*, string, double);
void setStringToLUA(lua_State*, string, string);
void DialogueEtGestionEvenements(string);
double ToSegment(double, int, int);
int NouveauId();
void Verbose(const string&, const short&);
void Erreur(const string&, const string&);
void Erreur(const string&, const float&);
float DirToCoeff_X(short, float Devn=0);
float DirToCoeff_Y(short, float Devn=0);
bool TestAngle(float&, float&, short, float&, float&, const short&);
void Disp_Texte(const String32&, int, int, Color couleur=Color(255,255,255,255), float Size=16.f, const Font &CharFont=gui::style::defaultTextFont());
void Disp_Texte(enumPhrases, int, int, Color couleur=Color(255,255,255,255), float Size=16.f, const Font &CharFont=gui::style::defaultTextFont());
void Disp_TexteCentre(enumPhrases, int, int, Color couleur=Color(255,255,255,255), float Size=16.f, const Font &CharFont=gui::style::defaultTextFont());
void Disp_TexteCentre(const String32&, int, int, Color couleur=Color(255,255,255,255), float Size=16.f, const Font &CharFont=gui::style::defaultTextFont());
void Disp_Texte(const string&, int, int, Color couleur=Color(255,255,255,255), float Size=16.f, const Font &CharFont=gui::style::defaultTextFont());
void Disp_TitrePage(enumPhrases);
string intToString(double, int size = -1);

#endif
