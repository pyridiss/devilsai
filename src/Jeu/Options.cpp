
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

#include <fstream>

#include "../Bibliotheque/Bibliotheque.h"
#include "../Bibliotheque/Constantes.h"
#include "Jeu.h"


void ChangerResolution()
{
	list < pair<unsigned,unsigned> > ResolutionsDisponibles;
	ResolutionsDisponibles.push_back(pair<unsigned,unsigned>(800, 600));
	ResolutionsDisponibles.push_back(pair<unsigned,unsigned>(1024, 768));
	ResolutionsDisponibles.push_back(pair<unsigned,unsigned>(1280, 800));

	bool Changement = false;

	for (list< pair<unsigned,unsigned> >::iterator i = ResolutionsDisponibles.begin() ; i != ResolutionsDisponibles.end() ; ++i)
	{
		if (i->first == Options.ScreenW_Save && i->second == Options.ScreenH_Save)
		{
			++i; if (i == ResolutionsDisponibles.end()) i = ResolutionsDisponibles.begin();
			Options.ScreenW_Save = i->first; Options.ScreenH_Save = i->second;
			Changement = true;
		}
	}

	if (!Changement)
	{
		Options.ScreenW_Save = ResolutionsDisponibles.begin()->first;
		Options.ScreenH_Save = ResolutionsDisponibles.begin()->second;
	}
}

void Load_Options()
{
	bool ChargerDefaut = false;

	string fichier = Get_DossierSauvegardes() + "options.opt";

	ifstream Fichier(fichier, ios_base::in);
	if (Fichier == NULL)
	{
		ChargerDefaut = true;
		CreerDossier(Get_DossierSauvegardes());
	}

	if (Fichier != NULL) MESSAGE(" Fichier \"" + fichier + "\" ouvert", FICHIER)

	string Test;
	if (!ChargerDefaut)
	{
		Fichier >> Test;
		if (Test != "FICHIER_OPTIONS")
		{
			ChargerDefaut = true;
			Fichier.close();
		}
	}

	if (ChargerDefaut)
	{
		string fichierDef = Partie.DATA + "defaut.opt";
		Fichier.open(fichierDef, ios_base::in);

		if (Fichier == NULL) Erreur("Le fichier suivant n'a pu être chargé :", fichierDef);
		if (Fichier != NULL) MESSAGE(" Fichier \"" + fichierDef + "\" ouvert", FICHIER)
	}
	string TypeDonnee = "";

	while (Fichier.rdstate() == 0)
	{
		Fichier >> TypeDonnee;

		if (TypeDonnee == "LANGUE")			Fichier >> Options.Langue;
		if (TypeDonnee == "PLEINECRAN")		Fichier >> Options.PleinEcran;
		if (TypeDonnee == "SAUVEGARDE")		Fichier >> Options.SauvegardeDisponible;
		if (TypeDonnee == "DEGATS")			Fichier >> Options.AffichageDegats;
		if (TypeDonnee == "RESOLUTION")		Fichier >> Options.ScreenW >> Options.ScreenH;

		TypeDonnee = "";
	}

	Options.PleinEcran_Save = Options.PleinEcran;
	Options.ScreenW_Save = Options.ScreenW;
	Options.ScreenH_Save = Options.ScreenH;

	Fichier.close();
}

void Save_Options()
{
	string fileName = Get_DossierSauvegardes() + "options.opt";

	ofstream fileStream(fileName, ios_base::out);

	if (fileStream == NULL) Erreur("Le fichier suivant n'a pu être ouvert en écriture :", fileName);
	if (fileStream != NULL) MESSAGE(" Fichier \"" + fileName + "\" ouvert", FICHIER)

	fileStream << "FICHIER_OPTIONS" << endl
			   << "LANGUE " << Options.Langue << endl
			   << "PLEINECRAN " << Options.PleinEcran_Save << endl
			   << "SAUVEGARDE " << Options.SauvegardeDisponible << endl
			   << "DEGATS " << Options.AffichageDegats << endl
			   << "RESOLUTION " << Options.ScreenW_Save << " " << Options.ScreenH_Save << endl;

	fileStream.close();
}

void EcranOptions()
{
	bool AfficherDemandeRedemarrage = false;

	Bouton BoutonMenu, Langue, PleinEcran, Degats, Resolution;

	BoutonMenu.Creer(100, Options.ScreenH - 170, Get_Phrase(_RETOUR_MENU), 14., Jeu.DayRoman);

	bool RetourMenu = false;
	while (!RetourMenu)
	{
		Langue.Creer(3.f/4.f * Options.ScreenW , 230, Get_NomLangue(), 16., Jeu.DayRoman);
		PleinEcran.Creer(3.f/4.f * Options.ScreenW, 270, Get_Phrase((Options.PleinEcran_Save) ? _ACTIVE : _DESACTIVE), 16., Jeu.DayRoman);
		Degats.Creer(3.f/4.f * Options.ScreenW, 310, Get_Phrase((Options.AffichageDegats) ? _ACTIVE : _DESACTIVE), 16., Jeu.DayRoman);
		string reso = intToString(Options.ScreenW_Save) + " x " + intToString(Options.ScreenH_Save);
		String32 Reso;
		Utf8::toUtf32(reso.begin(), reso.end(), back_inserter(Reso));
		Resolution.Creer(3.f/4.f * Options.ScreenW, 350, Reso, 16., Jeu.DayRoman);

		Event event;
		while (Jeu.App.pollEvent(event))
		{
			if (event.type == Event::MouseButtonPressed || event.type == Event::MouseButtonReleased)
			{
				if (Langue.TestActivation(event.type))
				{
					ChangerLangue();
					AfficherDemandeRedemarrage = true;
				}
				if (PleinEcran.TestActivation(event.type))
				{
					Options.PleinEcran_Save = !Options.PleinEcran_Save;
					AfficherDemandeRedemarrage = true;
				}
				if (Degats.TestActivation(event.type)) Options.AffichageDegats = !Options.AffichageDegats;
				if (BoutonMenu.TestActivation(event.type)) RetourMenu = true;
				if (Resolution.TestActivation(event.type))
				{
					ChangerResolution();
					AfficherDemandeRedemarrage = true;
				}
			}

			if (event.type == Event::Closed || (Keyboard::isKeyPressed(Keyboard::F4) && Keyboard::isKeyPressed(Keyboard::LAlt)))
				RetourMenu = true;
		}

		/* Affichage */

		Disp_FondMenus();

		Disp_TitrePage(_MENUPRINCIPAL_OPTIONS);

		Disp_Texte(_LANGUE, 1.f/4.f * Options.ScreenW, 220, Color(200,255,255,255), 16., Jeu.DayRoman);
		Disp_Texte(_PLEINECRAN, 1.f/4.f * Options.ScreenW, 260, Color(200,255,255,255), 16., Jeu.DayRoman);
		Disp_Texte(_AFFICHAGE_DEGATS, 1.f/4.f * Options.ScreenW, 300, Color(200,255,255,255), 16., Jeu.DayRoman);
		Disp_Texte(_RESOLUTION, 1.f/4.f * Options.ScreenW, 340, Color(200,255,255,255), 16., Jeu.DayRoman);

		Langue.Disp();
		PleinEcran.Disp();
		Degats.Disp();
		BoutonMenu.Disp();
		Resolution.Disp();

		if (AfficherDemandeRedemarrage)
			Disp_TexteCentre(_DEMANDE_REDEMARRAGE, Options.ScreenW/2, Options.ScreenH - 210, Color(255,196,196,255), 12.);

		//Licence :
		Disp_Texte("Devilsai - A game written using the SFML library", 20, Options.ScreenH - 140, Color(255,255,255,128), 13);
		Disp_Texte("Copyright (C) 2009-2014  Quentin Henriet", 20, Options.ScreenH - 125, Color(255,255,255,128), 13);

		Disp_Texte("This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by", 20, Options.ScreenH - 95, Color(255,255,255,128), 11);
		Disp_Texte("the Free Software Foundation, either version 3 of the License, or (at your option) any later version.", 20, Options.ScreenH - 80, Color(255,255,255,128), 11);

		Disp_Texte("This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of", 20, Options.ScreenH - 55, Color(255,255,255,128), 11);
		Disp_Texte("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.", 20, Options.ScreenH - 40, Color(255,255,255,128), 11);

		Disp_Texte("You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.", 20, Options.ScreenH - 20, Color(255,255,255,128), 11);

		Jeu.App.display();
		/* Fin animation */
	}

	Save_Options();

	return;
}
