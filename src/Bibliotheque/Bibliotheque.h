
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

#include <string>
#include <list>

#include "gui/style.h"
#include "gui/button.h"

using namespace std;
using namespace sf;

class lua_State;
class Element_Carte;
class Individu;


/** CLASSES **/

struct LigneConsole
{
	String32 Ligne;
	Color Couleur;
	float Temps		= 0;
	short NumLigne	= 0;
	bool Affichage	= false;
};


/** DÉCLARATION DES FONCTIONS **/

/* Fonctions définies dans le fichier Decorations.cpp */

void Load_Decorations();
void Supprimer_Decorations();
void Disp_JaugesVie(RenderTarget& target);
void Ajouter_LignePerso(String32, Color);
void Ajouter_LigneAmelioration(String32, Color);
void Disp_Consoles(RenderTarget& target);
void SupprimerLignesConsoles();
void Disp_Information(const String32&, bool);
void Disp_FonduNoir(int, RenderTarget& target);
bool Disp_Repos(RenderTarget& target);

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
string intToString(double, int size = -1);

#endif
