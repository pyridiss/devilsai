
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
class Individu;
struct Paragraph;

//Type de base pour les string contenant des textes à afficher
typedef basic_string<Uint32> String32;
typedef list<String32> ListString32;
typedef list<Paragraph> ListParagraph;


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


/** DÉCLARATION DES FONCTIONS **/

/* Fonctions définies dans le fichier Decorations.cpp */

void Load_Decorations();
void Supprimer_Decorations();
void Disp_FPS();
void Disp_FondMenus();
void Disp_Menu();
void Disp_JaugesVie();
void Ajouter_LignePerso(String32, Color);
void Ajouter_LigneAmelioration(String32, Color);
void Disp_Consoles();
void SupprimerLignesConsoles();
void Disp_Information(const String32&, bool);
void Disp_FonduNoir(int);
bool Disp_Repos();

/* Fonctions définies dans le fichier Langues.cpp */

void loadAvailableLanguages();
void deleteLanguagesList();
String32 getTranslatedNameOfElement(string);
String32 getTranslatedNameOfPlace(int);
String32 getTranslatedNameOfObject(int);
String32 getTranslatedNameOfObject(string);
String32 getTranslatedNameOfSkill(string);
String32 getTranslatedDescriptionOfObject(int);
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
double ToSegment(double, int, int);
void Verbose(const string&, const short&);
void Erreur(const string&, const string&);
void Erreur(const string&, const float&);
void Disp_Texte(const String32&, int, int, Color couleur=Color(255,255,255,255), float Size=16.f, const Font &CharFont=gui::style::defaultTextFont());
void Disp_TexteCentre(const String32&, int, int, Color couleur=Color(255,255,255,255), float Size=16.f, const Font &CharFont=gui::style::defaultTextFont());
void Disp_Texte(const string&, int, int, Color couleur=Color(255,255,255,255), float Size=16.f, const Font &CharFont=gui::style::defaultTextFont());
void Disp_TitrePage(const String32&);
string intToString(double, int size = -1);

#endif
