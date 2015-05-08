
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

#include <map>

#include "../Bibliotheque/Bibliotheque.h"
#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/Templates.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "Jeu.h"


struct Sauvegarde
{
	string Dossier = "";
	String32 Nom;
	string Version = "";
	String32 Lieu;
	int Vitalite = 1000;
};


typedef map<string, Sauvegarde> DicoSauvegardes;

DicoSauvegardes Sauvegardes;

void AjouterSauvegarde()
{
	Sauvegarde Save;

	Save.Dossier = Partie.SAVE;
	Save.Nom = Partie.perso->Nom;
	Save.Version = VERSION;
	Save.Lieu = getTranslatedNameOfPlace(Partie.perso->IndiceLieu);
	Save.Vitalite = Partie.perso->get("Vitalite");

	Sauvegardes.insert(DicoSauvegardes::value_type(Partie.SAVE, Save));
	Options.SauvegardeDisponible = true;

	Save_Sauvegardes();
	Save_Options();
}

void SupprimerSauvegarde(DicoSauvegardes::iterator save)
{
	//Ouverture du fichier principal pour déterminer les fichiers présents dans le dossier
	string path = Get_DossierSauvegardes() + save->second.Dossier + "/";
	string strFichierPrincipal = path + "save.sav";

	ifstream FichierPrincipal(strFichierPrincipal.c_str(), ios_base::in);
	if (FichierPrincipal == NULL) Erreur("Le fichier suivant n'a pu être chargé :", strFichierPrincipal.c_str());

	if (FichierPrincipal != NULL) MESSAGE(" Fichier \"" + strFichierPrincipal + "\" ouvert", FICHIER)

	string TypeDonnee;
	string DonneeStr;

	while(FichierPrincipal.rdstate() == 0)
	{
		FichierPrincipal >> TypeDonnee;
		if (TypeDonnee == "CARTE")
		{
			FichierPrincipal >> DonneeStr;
			SupprimerFichier(path + DonneeStr + ".map");
		}
		TypeDonnee = "";
	}

	SupprimerFichier(path + "capture.png");

	//Fermeture du fichier principal pour suppression
	FichierPrincipal.close();
	SupprimerFichier(path + "save.sav");

	//Suppression du dossier
	SupprimerDossier(Get_DossierSauvegardes() + save->second.Dossier);

	//Suppression de la sauvegarde
	Sauvegardes.erase(save);
}

void LectureSauvegardes()
{
	string fichier = Get_DossierSauvegardes() + "sauvegardes.opt";

	ifstream Fichier(fichier.c_str(), ios_base::in);

	string path = "";
	string TypeDonnee;
	int NombreSauvegardes = 0;
	string Buffer, Buffer2;
	String32 Buffer3;

	DicoSauvegardes::iterator i;

	//Ce fichier peut ne pas exister
	if (Fichier != NULL)
	{
		if (Fichier != NULL) MESSAGE(" Fichier \"" + fichier + "\" ouvert", FICHIER)

		Fichier >> TypeDonnee;
		if (TypeDonnee == "NOMBRE_SAUVEGARDES") Fichier >> NombreSauvegardes;
		else Erreur("Fichier de sauvegarde corrompu :", fichier);

		while (Fichier.rdstate() == 0)
		{
			Fichier >> TypeDonnee;
			if (TypeDonnee == "SAUVEGARDE")
			{
				Sauvegarde s;
				string d;
				Fichier >> d;
				Sauvegardes.insert(DicoSauvegardes::value_type(d, s));
				i = Sauvegardes.find(d);
				i->second.Dossier = d;
			}
			if (i == Sauvegardes.end())
			{
				Erreur("Fichier de sauvegarde corrompu :", fichier);
				Fichier.close();
				return;
			}
			if (TypeDonnee == "NOM")		Fichier >> i->second.Nom;
			if (TypeDonnee == "VERSION")	Fichier >> i->second.Version;
			if (TypeDonnee == "LIEU")		Fichier >> i->second.Lieu;
			if (TypeDonnee == "VITALITE")	Fichier >> i->second.Vitalite;
			if (TypeDonnee == "FIN_SAUVEGARDE");

			TypeDonnee = "";
		}

		Fichier.close();
	}
}

void PasDeSauvegarde()
{
	Bouton BoutonMenu;
	BoutonMenu.Creer(100, Options.ScreenH - 170, getTranslatedMessage(_RETOUR_MENU), 14., Jeu.DayRoman);

	Event event;

	while (true)
	{
		while (Jeu.App.pollEvent(event))
		{
			if (event.type == Event::MouseButtonPressed || event.type == Event::MouseButtonReleased)
			{
				if (BoutonMenu.TestActivation(event.type))
				{
					return;
				}
			}
			if (event.type == Event::KeyPressed)
			{
				switch (event.key.code)
				{
					case Keyboard::Escape : return;
					default: break;
				}
			}
			if (event.type == Event::Closed || (Keyboard::isKeyPressed(Keyboard::F4) && Keyboard::isKeyPressed(Keyboard::LAlt)))
				return;
		}

		//Affichage :
		Jeu.App.clear();
		Disp_FondMenus();

		Disp_TitrePage(_MENUPRINCIPAL_CHARGER);
		Disp_TexteCentre(_PAS_DE_SAUVEG, Options.ScreenW/2, Options.ScreenH/2, Color(50,128,128,255), 28, Jeu.DayRoman);

		BoutonMenu.Disp();

		Jeu.App.display();
	}
}

int NombreSauvegardesDisponibles()
{
	return Sauvegardes.size();
}

void SauvegardeSuivante(DicoSauvegardes::iterator &i)
{
	++i;
	if (i == Sauvegardes.end()) i = Sauvegardes.begin();
}

void SauvegardePrecedente(DicoSauvegardes::iterator &i)
{
	if (i == Sauvegardes.begin()) i = Sauvegardes.end();
	--i;
}

string ChoixSauvegarde()
{
	if (!Options.SauvegardeDisponible)
	{
		PasDeSauvegarde();
		return "ANNULER";
	}

	DicoSauvegardes::iterator save = Sauvegardes.begin();

	string Vitalite;
	bool SauvegardeCompatible = false;
	Texture TexCapture;
	Sprite Capture;

	Bouton BoutonLancer, BoutonPrecedent, BoutonSuivant, BoutonMenu, BoutonSupprimer;

	BoutonLancer.Creer(Options.ScreenW/2, 188, getTranslatedMessage(_LANCER_PARTIE), 24., Jeu.DayRoman);

	BoutonPrecedent.Creer(Options.ScreenW/2 - 180, 190, getTranslatedMessage(_PRECEDENT), 14., Jeu.DayRoman);
	BoutonSuivant.Creer(Options.ScreenW/2 + 180, 190, getTranslatedMessage(_SUIVANT), 14., Jeu.DayRoman);

	BoutonMenu.Creer(100, Options.ScreenH - 170, getTranslatedMessage(_RETOUR_MENU), 14., Jeu.DayRoman);

	BoutonSupprimer.Creer(Options.ScreenW/2 - 220, 370, getTranslatedMessage(_SUPPRIMER_SAUVEGARDE), 16., Jeu.DayRoman);

	string path = "";

	string PATH = Get_DossierSauvegardes();

	bool ChangementSauvegarde = true;
	Event event;

	//2. Écran de choix de la sauvegarde
	while (path == "")
	{
		while (Jeu.App.pollEvent(event))
		{
			if (event.type == Event::MouseButtonPressed || event.type == Event::MouseButtonReleased)
			{
				if (NombreSauvegardesDisponibles() > 1 && BoutonPrecedent.TestActivation(event.type))
				{
					SauvegardePrecedente(save);
					ChangementSauvegarde = true;
				}
				if (NombreSauvegardesDisponibles() > 1 && BoutonSuivant.TestActivation(event.type))
				{
					SauvegardeSuivante(save);
					ChangementSauvegarde = true;
				}
				if (SauvegardeCompatible && BoutonLancer.TestActivation(event.type))
				{
					path = save->second.Dossier;
				}
				if (BoutonSupprimer.TestActivation(event.type))
				{
					SupprimerSauvegarde(save);
					if (NombreSauvegardesDisponibles())
					{
						save = Sauvegardes.begin();
						ChangementSauvegarde = true;
					}
					else
					{
						Options.SauvegardeDisponible = false;
						PasDeSauvegarde();
						return "ANNULER";
					}
				}
				if (BoutonMenu.TestActivation(event.type))
				{
					path = "ANNULER";
				}
			}
			if (event.type == Event::KeyPressed)
			{
				switch (event.key.code)
				{
					case Keyboard::Escape : path = "ANNULER"; break;
					default: break;
				}
			}
			if (event.type == Event::Closed || (Keyboard::isKeyPressed(Keyboard::F4) && Keyboard::isKeyPressed(Keyboard::LAlt)))
				path = "ANNULER";
		}

		//Chargement des infos de la sauvegarde :
		if (ChangementSauvegarde)
		{
			Vitalite = intToString(save->second.Vitalite);
			if (save->second.Version == VERSION) SauvegardeCompatible = true;
			else SauvegardeCompatible = false;
			//Anciennes versions de sauvegarde toujours compatibles :
			//if (save->second.Version == "0.2.svn66") SauvegardeCompatible = true;

			TexCapture.loadFromFile(PATH + save->second.Dossier + "/capture.png");
			Capture.setTexture(TexCapture);
			Capture.setPosition(Options.ScreenW/2 - 50, 240);
			Capture.setScale(0.5, 0.5);

			ChangementSauvegarde = false;
		}

		//Affichage :
		Jeu.App.clear();
		Disp_FondMenus();

		Disp_TitrePage(_MENUPRINCIPAL_CHARGER);

		if (SauvegardeCompatible) BoutonLancer.Disp();
		BoutonSupprimer.Disp();
		BoutonMenu.Disp();
		if (NombreSauvegardesDisponibles() > 1)
		{
			BoutonPrecedent.Disp();
			BoutonSuivant.Disp();
		}

		Jeu.App.draw(Capture);

		if (!SauvegardeCompatible) Disp_TexteCentre(_VERSION_INCOMPATIBLE, Options.ScreenW/2, 210, Color(255, 70, 70, 255), 10.);

		Disp_Texte(save->second.Nom, Options.ScreenW/2 - 300, 240, Color(255, 220, 220, 255), 35., Jeu.DayRoman);
		Disp_Texte(getTranslatedMessage(_SAUVEGARDE_VERSION) + save->second.Version, Options.ScreenW/2 - 280, 280, Color(200, 255, 200, 255), 14.);
		Disp_Texte(save->second.Lieu, Options.ScreenW/2 - 280, 300, Color(255, 220, 220, 255), 14.);
		Disp_Texte(_PERSO_VITALITE, Options.ScreenW/2 - 280, 320, Color(255, 255, 255, 255), 14.);
		Disp_Texte(Vitalite, Options.ScreenW/2 - 220, 320, Color(255, 64, 64, 255), 14.);

		Jeu.App.display();
	}

	return path;
}

void MaJ_Sauvegarde()
{
	DicoSauvegardes::iterator i = Sauvegardes.find(Partie.SAVE);
	if (i == Sauvegardes.end())
	{
		Erreur("MaJ_Sauvegarde() appelé sans partie ajoutée auparavant", "");
		return;
	}
	i->second.Version = VERSION;
	i->second.Lieu.clear();
	i->second.Lieu = getTranslatedNameOfPlace(Partie.perso->IndiceLieu);
	i->second.Vitalite = Partie.perso->get("Vitalite");
}

void Save_Sauvegardes()
{
	string fileName = Get_DossierSauvegardes() + "sauvegardes.opt";

	string bufferString;

	ofstream fileStream(fileName.c_str(), ios_base::out);

	if (fileStream == NULL) Erreur("Le fichier suivant n'a pu être ouvert en écriture :", fileName.c_str());
	if (fileStream != NULL) MESSAGE(" Fichier \"" + fileName + "\" ouvert", FICHIER)

	fileStream << "NOMBRE_SAUVEGARDES " << Sauvegardes.size() << endl;

	for (auto& savedGame : Sauvegardes)
	{
		fileStream << "SAUVEGARDE " << savedGame.second.Dossier << endl;
		sf::Utf32::toUtf8(savedGame.second.Nom.begin(), savedGame.second.Nom.end(), back_inserter(bufferString));
		fileStream << "NOM " << bufferString << endl;
		fileStream << "VERSION " << savedGame.second.Version << endl;
		bufferString = "";
		sf::Utf32::toUtf8(savedGame.second.Lieu.begin(), savedGame.second.Lieu.end(), back_inserter(bufferString));
		fileStream << "LIEU " << bufferString << endl;
		fileStream << "VITALITE " << savedGame.second.Vitalite << endl;
		fileStream << "FIN_SAUVEGARDE" << endl;
		bufferString = "";
	}

	fileStream.close();
}
