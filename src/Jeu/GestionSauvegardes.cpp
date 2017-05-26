
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
#include <sstream>

#include "tools/signals.h"
#include "tools/filesystem.h"
#include "tools/timeManager.h"

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

    stringstream stream; stream << time(NULL);
    Partie.SAVE = stream.str();
    tools::filesystem::createDirectory(tools::filesystem::getSaveDirectoryPath() + Partie.SAVE);

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
	string path = tools::filesystem::getSaveDirectoryPath() + save->second.Dossier + "/";
	string strFichierPrincipal = path + "save.sav";

	ifstream FichierPrincipal(strFichierPrincipal.c_str(), ios_base::in);
	if (!FichierPrincipal.good()) Erreur("Le fichier suivant n'a pu être chargé :", strFichierPrincipal.c_str());

	if (FichierPrincipal.good()) MESSAGE(" Fichier \"" + strFichierPrincipal + "\" ouvert", FICHIER)

	string TypeDonnee;
	string DonneeStr;

	while(FichierPrincipal.rdstate() == 0)
	{
		FichierPrincipal >> TypeDonnee;
		if (TypeDonnee == "CARTE")
		{
			FichierPrincipal >> DonneeStr;
            tools::filesystem::removeFile(path + DonneeStr + ".map");
		}
		TypeDonnee = "";
	}

    tools::filesystem::removeFile(path + "capture.png");

	//Fermeture du fichier principal pour suppression
	FichierPrincipal.close();
    tools::filesystem::removeFile(path + "save.sav");

	//Suppression du dossier
    tools::filesystem::removeDirectory(tools::filesystem::getSaveDirectoryPath() + save->second.Dossier);

	//Suppression de la sauvegarde
	Sauvegardes.erase(save);
}

void LectureSauvegardes()
{
	string fichier = tools::filesystem::getSaveDirectoryPath() + "sauvegardes.opt";

	ifstream Fichier(fichier.c_str(), ios_base::in);

	string path = "";
	string TypeDonnee;
	int NombreSauvegardes = 0;
	string Buffer, Buffer2;
	String32 Buffer3;

	DicoSauvegardes::iterator i;

	//Ce fichier peut ne pas exister
	if (Fichier.good())
	{
		if (Fichier.good()) MESSAGE(" Fichier \"" + fichier + "\" ouvert", FICHIER)

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
    gui::Button mainMenuButton;
    mainMenuButton.setCenterCoordinates(100, Options.ScreenH - 170);
    mainMenuButton.setTextFont(Jeu.DayRoman, 14);
    mainMenuButton.setAllText(getTranslatedMessage(_RETOUR_MENU));

	Event event;

	while (true)
	{
		while (Jeu.App.pollEvent(event))
		{
            if (mainMenuButton.activated(Jeu.App, event))
            {
                tools::signals::addSignal("main-menu");
                return;
            }

			if (event.type == Event::KeyPressed)
			{
				switch (event.key.code)
				{
					case Keyboard::Escape :
                        tools::signals::addSignal("main-menu");
                        return;
					default: break;
				}
			}
			if (event.type == Event::Closed || (Keyboard::isKeyPressed(Keyboard::F4) && Keyboard::isKeyPressed(Keyboard::LAlt)))
            {
                tools::signals::addSignal("exit");
				return;
            }
		}

		//Affichage :
		Jeu.App.clear();
		Disp_FondMenus();

		Disp_TitrePage(_MENUPRINCIPAL_CHARGER);
		Disp_TexteCentre(_PAS_DE_SAUVEG, Options.ScreenW/2, Options.ScreenH/2, Color(50,128,128,255), 28, Jeu.DayRoman);

        mainMenuButton.display(Jeu.App);

        tools::timeManager::frameDone();
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

	gui::Button startGameButton, previousButton, nextButton, mainMenuButton, deleteGameButton;

    startGameButton.setCenterCoordinates(Options.ScreenW/2, 188);
    startGameButton.setTextFont(Jeu.DayRoman, 24);
    startGameButton.setAllText(getTranslatedMessage(_LANCER_PARTIE));

    previousButton.setCenterCoordinates(Options.ScreenW/2 - 180, 190);
    previousButton.setTextFont(Jeu.DayRoman, 14);
    previousButton.setAllText(getTranslatedMessage(_PRECEDENT));

    nextButton.setCenterCoordinates(Options.ScreenW/2 + 180, 190);
    nextButton.setTextFont(Jeu.DayRoman, 14);
    nextButton.setAllText(getTranslatedMessage(_SUIVANT));

    mainMenuButton.setCenterCoordinates(100, Options.ScreenH - 170);
    mainMenuButton.setTextFont(Jeu.DayRoman, 14);
    mainMenuButton.setAllText(getTranslatedMessage(_RETOUR_MENU));

    deleteGameButton.setCenterCoordinates(Options.ScreenW/2 - 220, 370);
    deleteGameButton.setTextFont(Jeu.DayRoman, 14);
    deleteGameButton.setAllText(getTranslatedMessage(_SUPPRIMER_SAUVEGARDE));

    if (NombreSauvegardesDisponibles() <= 1)
    {
        previousButton.setDisabled(true);
        nextButton.setDisabled(true);
    }

	string path = "";

	string PATH = tools::filesystem::getSaveDirectoryPath();

	bool ChangementSauvegarde = true;
	Event event;

	//2. Écran de choix de la sauvegarde
	while (path == "")
	{
		while (Jeu.App.pollEvent(event))
		{
            if (SauvegardeCompatible)
                startGameButton.setDisabled(false);
            else
                startGameButton.setDisabled(true);

            if (previousButton.activated(Jeu.App, event))
            {
                SauvegardePrecedente(save);
                ChangementSauvegarde = true;
            }
            if (nextButton.activated(Jeu.App, event))
            {
                SauvegardeSuivante(save);
                ChangementSauvegarde = true;
            }
            if (startGameButton.activated(Jeu.App, event))
            {
                path = save->second.Dossier;
            }
            if (deleteGameButton.activated(Jeu.App, event))
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
            if (mainMenuButton.activated(Jeu.App, event))
            {
                path = "ANNULER";
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

        startGameButton.display(Jeu.App);
        deleteGameButton.display(Jeu.App);
        mainMenuButton.display(Jeu.App);
        previousButton.display(Jeu.App);
        nextButton.display(Jeu.App);

		Jeu.App.draw(Capture);

		if (!SauvegardeCompatible) Disp_TexteCentre(_VERSION_INCOMPATIBLE, Options.ScreenW/2, 210, Color(255, 70, 70, 255), 10.);

		Disp_Texte(save->second.Nom, Options.ScreenW/2 - 300, 240, Color(255, 220, 220, 255), 35., Jeu.DayRoman);
		Disp_Texte(getTranslatedMessage(_SAUVEGARDE_VERSION) + save->second.Version, Options.ScreenW/2 - 280, 280, Color(200, 255, 200, 255), 14.);
		Disp_Texte(save->second.Lieu, Options.ScreenW/2 - 280, 300, Color(255, 220, 220, 255), 14.);
		Disp_Texte(_PERSO_VITALITE, Options.ScreenW/2 - 280, 320, Color(255, 255, 255, 255), 14.);
		Disp_Texte(Vitalite, Options.ScreenW/2 - 220, 320, Color(255, 64, 64, 255), 14.);

        tools::timeManager::frameDone();
		Jeu.App.display();
	}

	return path;
}

void MaJ_Sauvegarde()
{
	DicoSauvegardes::iterator i = Sauvegardes.find(Partie.SAVE);
	if (i == Sauvegardes.end())
	{
        AjouterSauvegarde();
        MaJ_Sauvegarde();
		return;
	}
	i->second.Version = VERSION;
	i->second.Lieu.clear();
	i->second.Lieu = getTranslatedNameOfPlace(Partie.perso->IndiceLieu);
	i->second.Vitalite = Partie.perso->get("Vitalite");
}

void Save_Sauvegardes()
{
	string fileName = tools::filesystem::getSaveDirectoryPath() + "sauvegardes.opt";

	string bufferString;

	ofstream fileStream(fileName.c_str(), ios_base::out);

	if (!fileStream.good()) Erreur("Le fichier suivant n'a pu être ouvert en écriture :", fileName.c_str());
	if (fileStream.good()) MESSAGE(" Fichier \"" + fileName + "\" ouvert", FICHIER)

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
