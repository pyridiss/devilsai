
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

#include <sstream>
#include <fstream>
#include <cmath>
#include <string>

#include <lua5.2/lua.hpp>

#include "Bibliotheque.h"
#include "Constantes.h"
#include "../Jeu/Jeu.h"
//#include "../Carte/Carte.h"
//#include "../ElementsCarte/ElementsCarte.h"

#if defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#elif defined(linux) || defined(__linux)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#define PI 3.1415926

bool CreerDossier(string str)
{
#if defined(_WIN32) || defined(__WIN32__)
	if (CreateDirectory(str.c_str(), NULL))
	{
		MESSAGE("Le dossier " + str + " a été créé", FICHIER)
		return true;
	}
	else
	{
		Erreur("Impossible de créer le dossier suivant :", str);
		return false;
	}
#else
	if (mkdir(str.c_str(), S_IRWXU))
	{
		Erreur("Impossible de créer le dossier suivant :", str);
		return false;
	}
	else
	{
		MESSAGE("Le dossier " + str + " a été créé", FICHIER)
		return true;
	}
#endif
	return false;
}

bool SupprimerDossier(string str)
{
#if defined(_WIN32) || defined(__WIN32__)
	if (RemoveDirectory(str.c_str()))
	{
		MESSAGE("Le dossier " + str + " a été supprimé", FICHIER)
		return true;
	}
	else
	{
		Erreur("Impossible de supprimer le dossier suivant :", str);
		return false;
	}
#else
	if (rmdir(str.c_str()))
	{
		Erreur("Impossible de supprimer le dossier suivant :", str);
		return false;
	}
	else
	{
		MESSAGE("Le dossier " + str + " a été supprimé", FICHIER)
		return true;
	}
#endif
	return false;
}

bool SupprimerFichier(string str)
{
#if defined(_WIN32) || defined(__WIN32__)
	if (DeleteFile(str.c_str()))
	{
		MESSAGE("Le fichier " + str + " a été supprimé", FICHIER)
		return true;
	}
	else
	{
		Erreur("Impossible de supprimer le fichier suivant :", str);
		return false;
	}
#else
	if (unlink(str.c_str()))
	{
		Erreur("Impossible de supprimer le fichier suivant :", str);
		return false;
	}
	else
	{
		MESSAGE("Le fichier " + str + " a été supprimé", FICHIER)
		return true;
	}
#endif
	return false;
}

string Get_DossierSauvegardes()
{
#if defined(_WIN32) || defined(__WIN32__)
	return "save/";
#else
	return ".Devilsai/";
#endif
	return "";
}

void getFromLUA(lua_State* L, string fct)
{
	lua_getglobal(L, fct.c_str());

	if (lua_isfunction(L, -1))
	{
		lua_call(L, 0, 1);
		MESSAGE("… OK", LUA)
	}
	else
	{
		lua_pop(L, 1);
		Erreur("La fonction LUA n'est pas disponible :", fct);
		MESSAGE("… FAIL", LUA)
	}
}

bool getBoolFromLUA(lua_State* L, string fct)
{
	MESSAGE("getBoolFromLUA : appel de " + fct + "…", LUA)

	getFromLUA(L, fct);

	if (lua_isboolean(L, -1))
		return lua_toboolean(L, -1);

	Erreur("getBoolFromLUA : la valeur de retour de cette fonction n'est pas booléenne :", fct);
	return false;
}

int getIntFromLUA(lua_State* L, string fct)
{
	return getDoubleFromLUA(L, fct);
}

double getDoubleFromLUA(lua_State* L, string fct)
{
	MESSAGE("getDoubleFromLUA : appel de " + fct + "…", LUA)

	getFromLUA(L, fct);

	if (lua_isnumber(L, -1))
		return lua_tonumber(L, -1);

	Erreur("getDoubleFromLUA : la valeur de retour de cette fonction n'est pas un nombre :", fct);
	return 0;
}

string getStringFromLUA(lua_State* L, string fct)
{
	MESSAGE("getStringFromLUA : appel de " + fct + "…", LUA)

	getFromLUA(L, fct);

	if (lua_isstring(L, -1))
		return lua_tostring(L, -1);

	Erreur("getStringFromLUA : la valeur de retour de cette fonction n'est pas une chaîne de caractères :", fct);
	return "";
}

void setBoolToLUA(lua_State* L, string fct, bool value)
{
	MESSAGE("setBoolToLUA : appel de " + fct + "…", LUA)

	lua_getglobal(L, fct.c_str());

	if (lua_isfunction(L, -1))
	{
		lua_pushboolean(L, value);
		lua_call(L, 1, 0);
		MESSAGE("… OK", LUA)
	}
	else
	{
		lua_pop(L, 1);
		Erreur("La fonction LUA demandée n'est pas disponible :", fct);
		MESSAGE("… FAIL", LUA)
	}
}

void setIntToLUA(lua_State* L, string fct, int value)
{
	setDoubleToLUA(L, fct, value);
}

void setDoubleToLUA(lua_State* L, string fct, double value)
{
	MESSAGE("setDoubleToLUA : appel de " + fct + "…", LUA)

	lua_getglobal(L, fct.c_str());

	if (lua_isfunction(L, -1))
	{
		lua_pushnumber(L, value);
		lua_call(L, 1, 0);
		MESSAGE("… OK", LUA)
	}
	else
	{
		lua_pop(L, 1);
		Erreur("La fonction LUA demandée n'est pas disponible :", fct);
		MESSAGE("… FAIL", LUA)
	}	
}

void setStringToLUA(lua_State* L, string fct, string value)
{
	MESSAGE("setStringToLUA : appel de " + fct + "…", LUA)

	lua_getglobal(L, fct.c_str());

	if (lua_isfunction(L, -1))
	{
		lua_pushstring(L, value.c_str());
		lua_call(L, 1, 0);
		MESSAGE("… OK", LUA)
	}
	else
	{
		lua_pop(L, 1);
		Erreur("La fonction LUA demandée n'est pas disponible :", fct);
		MESSAGE("… FAIL", LUA)
	}
}

void DialogueEtGestionEvenements(string fichier)
{
	Dialog Intro;
	Intro.load(fichier.c_str());

	bool FinDialogue = false;

	while (!FinDialogue)
	{
		Jeu.App.clear();

		Disp_FondMenus();

		FinDialogue = Intro.display();

		Jeu.App.display();

		Event event;

		while (Jeu.App.pollEvent(event))
		{
			if (event.type == Event::KeyReleased)
			{
				switch (event.key.code)
				{
					case Keyboard::Escape :	FinDialogue = true; break;
					default :				break;
				}
			}
			if (event.type == Event::Closed) FinDialogue = true;
		}
	}

}

int SaisieNom(String32 &Texte)
{
    tools::Button mainMenuButton, BoutonDidacticiel, BoutonChapitre1;

    mainMenuButton.setCenterCoordinates(100, Options.ScreenH - 170);
    mainMenuButton.setTextFont(Jeu.DayRoman, 14);
    mainMenuButton.setAllText(getTranslatedMessage(_RETOUR_MENU));

    BoutonDidacticiel.setCenterCoordinates(Options.ScreenW - 200, Options.ScreenH - 200);
    BoutonDidacticiel.setTextFont(Jeu.DayRoman, 14);
    BoutonDidacticiel.setAllText(getTranslatedMessage(_LANCER_DIDACTICIEL));
    BoutonDidacticiel.setDisabled(true);

    BoutonChapitre1.setCenterCoordinates(Options.ScreenW - 200, Options.ScreenH - 170);
    BoutonChapitre1.setTextFont(Jeu.DayRoman, 14);
    BoutonChapitre1.setAllText(getTranslatedMessage(_LANCER_CHAP1));
    BoutonChapitre1.setDisabled(true);

	int Resultat = false;

	Event event;

	while (!Resultat)
	{
		Jeu.App.clear();
		Disp_FondMenus();

		Disp_TitrePage(_SAISIE_NOM);

		while (Jeu.App.pollEvent(event))
		{
            if (Texte.size() == 0)
            {
                BoutonDidacticiel.setDisabled(true);
                BoutonChapitre1.setDisabled(true);
            }
            else
            {
                BoutonDidacticiel.setDisabled(false);
                BoutonChapitre1.setDisabled(false);
            }

			if (event.type == Event::TextEntered)
			{
				switch(event.text.unicode)
				{
					case 8 :	if (Texte.size() > 0) Texte.resize(Texte.size()-1);
								break;	//Touche Retour
					case 13 :	if (Texte.size() > 0) Resultat = 1;
								break;	//Touche Entrée
					case 27 :	Texte.clear();
								break;	//Touche Escape
					case 32 :	break;	//Touche 'Espace'
					default :	if (Texte.size() < 25) Texte += event.text.unicode;
								break;
				}
			}

            if (mainMenuButton.activated(Jeu.App, event.type))
            {
                Texte.clear();
                Resultat = -1;
            }
            if (BoutonDidacticiel.activated(Jeu.App, event.type)) Resultat = 1;
            if (BoutonChapitre1.activated(Jeu.App, event.type)) Resultat = 2;

			if (event.type == Event::KeyPressed)
			{
				switch (event.key.code)
				{
					case Keyboard::Escape : Texte.clear(); Resultat = -1; break;
					default: break;
				}
			}
			if (event.type == Event::Closed || (Keyboard::isKeyPressed(Keyboard::F4) && Keyboard::isKeyPressed(Keyboard::LAlt)))
			{
				Texte.clear();
				Resultat = -1;
			}
		}
		Disp_TexteCentre(Texte, Options.ScreenW/2, Options.ScreenH/2, Color(255,220,220,255), 28, Jeu.DayRoman);
		mainMenuButton.display(Jeu.App);
		BoutonDidacticiel.display(Jeu.App);
		BoutonChapitre1.display(Jeu.App);
		Jeu.App.display();
	}
	return Resultat;
}

/* Retourne l'incrément à donner à une variable dépendante du temps */
float I(float i)
{
	return Options.VitesseJeu * Jeu.FrameTime.asMilliseconds() * i;
}

float Minimum(float a, float b)
{
	return (a < b) ? a : b;
}

float Maximum(float a, float b)
{
	return (a < b) ? b : a;
}

float Abs(float a)
{
	return (a >= 0) ? a : -a;
}

int Abs(int a)
{
	return (a >= 0) ? a : -a;
}
long int Abs(long int a)
{
	return (a >= 0) ? a : -a;
}

double ToSegment(double x, int min, int max)
{
	return min + 2*(max-min)/PI * atan(x*PI/(2*(max-min)));
}

int NouveauId()
{
	static int id = -1;
	return ++id;
}

#ifdef DEBOGAGE
void Verbose(const string &Message, const short &Type)
{
	if(!Arguments.TabVerbose[0]) return;

	switch (Type)
	{
		case FICHIER :		if (!Arguments.TabVerbose[FICHIER]) return;
							cout << "VERB.FICH : " << Message << endl;
							break;
		case IMAGE :		if (!Arguments.TabVerbose[IMAGE]) return;
							cout << "VERB.IMG  : " << Message << endl;
							break;
		case LISTE :		if (!Arguments.TabVerbose[LISTE]) return;
							cout << "VERB.LIST : " << Message << endl;
							break;
		case LUA :			if (!Arguments.TabVerbose[LUA]) return;
							cout << "VERB.LUA  : " << Message << endl;
							break;
		case MUSIC :		if (!Arguments.TabVerbose[MUSIC]) return;
							cout << "VERB.MUSIC  : " << Message << endl;
							break;
	}
}
#endif

void Erreur(const string &Message1, const string &Message2)
{
	Jeu.ErreurDetectee = true;

	ofstream FichierErreurs("Devilsai_Errors", ios_base::out | ios_base::app);

	time_t rawtime;
	struct tm * timeinfo;

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	FichierErreurs << "On " << asctime (timeinfo) << " >> " << Message1 << " " << Message2 << endl << endl;

	FichierErreurs.close();
}

void Erreur(const string &Message1, const float &Num)
{
	Erreur(Message1, intToString(Num));
}

float DirToCoeff_X(short Dir, float Devn)
{
	switch (Dir)
	{
		case NORD :		return Devn/100.;
		case SUD :		return Devn/100.;
		case EST :		return 1-0.5*(Devn*Devn)/10000.;
		case OUEST :	return -1+0.5*(Devn*Devn)/10000.;
		case N_E :		return 0.7;
		case N_O :		return -0.7;
		case S_E :		return 0.7;
		case S_O :		return -0.7;
	}
	return 0;
}

float DirToCoeff_Y(short Dir, float Devn)
{
	switch (Dir)
	{
		case NORD :		return -1+0.5*(Devn*Devn)/10000.;
		case SUD :		return 1-0.5*(Devn*Devn)/10000.;
		case EST :		return Devn/100.;
		case OUEST :	return Devn/100.;
		case N_E :		return -0.7;
		case N_O :		return -0.7;
		case S_E :		return 0.7;
		case S_O :		return 0.7;
	}
	return 0;
}

bool TestAngle(float &Ox, float &Oy, short dir, float &Ax, float &Ay, const short &NbDir)
{
	bool Touche = false;

	if (NbDir == 8)
	{
		switch (dir)
		{
			case NORD :		if (2*Abs((long int)(Ax-Ox)) <= Oy-Ay) Touche = true; break;
			case SUD :		if (2*Abs((long int)(Ax-Ox)) <= Ay-Oy) Touche = true; break;
			case EST :		if (2*Abs((long int)(Ay-Oy)) <= Ax-Ox) Touche = true; break;
			case OUEST :	if (2*Abs((long int)(Ay-Oy)) <= Ox-Ax) Touche = true; break;
			case N_E :		if ((Ax-Ox) > 0 && (Oy-Ay) > 0 && 2*Abs((long int)(Ax-Ox)) >= Oy-Ay && 2*Abs((long int)(Ay-Oy)) >= Ax-Ox) Touche = true; break;
			case N_O :		if ((Ox-Ax) > 0 && (Oy-Ay) > 0 && 2*Abs((long int)(Ax-Ox)) >= Oy-Ay && 2*Abs((long int)(Ay-Oy)) >= Ox-Ax) Touche = true; break;
			case S_E :		if ((Ax-Ox) > 0 && (Ay-Oy) > 0 && 2*Abs((long int)(Ax-Ox)) >= Ay-Oy && 2*Abs((long int)(Ay-Oy)) >= Ax-Ox) Touche = true; break;
			case S_O :		if ((Ox-Ax) > 0 && (Ay-Oy) > 0 && 2*Abs((long int)(Ax-Ox)) >= Ay-Oy && 2*Abs((long int)(Ay-Oy)) >= Ox-Ax) Touche = true; break;
		}
	}
	if (NbDir == 4)
	{
		switch (dir)
		{
			case NORD :		if (Abs((long int)(Ax-Ox)) <= Oy-Ay) Touche = true; break;
			case SUD :		if (Abs((long int)(Ax-Ox)) <= Ay-Oy) Touche = true; break;
			case EST :		if (Abs((long int)(Ay-Oy)) <= Ax-Ox) Touche = true; break;
			case OUEST :	if (Abs((long int)(Ay-Oy)) <= Ox-Ax) Touche = true; break;
		}
	}

	if (NbDir == 1) Touche = true;

	return Touche;
}

void Disp_Texte(const String32 &str, int x, int y, Color couleur, float Size, const Font &CharFont)
{
	Text Texte(str, CharFont, Size);
	Texte.setPosition(x, y);
	Texte.setColor(couleur);

	Jeu.App.draw(Texte);
}

void Disp_Texte(const string &str, int x, int y, Color couleur, float Size, const Font &CharFont)
{
	Text Texte(str, CharFont, Size);
	Texte.setPosition(x, y);
	Texte.setColor(couleur);

	Jeu.App.draw(Texte);
}

void Disp_Texte(enumPhrases NumPhrase, int x, int y, Color couleur, float Size, const Font &CharFont)
{
	Text Texte(getTranslatedMessage(NumPhrase), CharFont, Size);
	Texte.setPosition(x, y);
	Texte.setColor(couleur);

	Jeu.App.draw(Texte);
}

void Disp_TexteCentre(enumPhrases NumPhrase, int x, int y, Color couleur, float Size, const Font &CharFont)
{
	Text Texte(getTranslatedMessage(NumPhrase), CharFont, Size);
	Texte.setPosition((int)(x - Texte.getGlobalBounds().width/2), (int)(y - Texte.getGlobalBounds().height/2));
	Texte.setColor(couleur);

	Jeu.App.draw(Texte);
}

void Disp_TexteCentre(const String32 &str, int x, int y, Color couleur, float Size, const Font &CharFont)
{
	Text Texte(str, CharFont, Size);
	Texte.setPosition((int)(x - Texte.getGlobalBounds().width/2), (int)(y - Texte.getGlobalBounds().height/2));
	Texte.setColor(couleur);

	Jeu.App.draw(Texte);
}

void Disp_TitrePage(enumPhrases NumPhrase)
{
	Disp_TexteCentre(NumPhrase, Options.ScreenW/2 - 2, 118, Color(0,0,0,255), 45, Jeu.DayRoman);
	Disp_TexteCentre(NumPhrase, Options.ScreenW/2 + 2, 122, Color(0,0,0,255), 45, Jeu.DayRoman);
	Disp_TexteCentre(NumPhrase, Options.ScreenW/2, 120, Color(50,192,192,255), 45, Jeu.DayRoman);
}

string intToString(double number)
{
	stringstream out;
	out.precision(8);
	out << number;
	return out.str();
}
