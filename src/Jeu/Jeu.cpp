
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

#include "tools/filesystem.h"
#include "tools/timeManager.h"
#include "tools/signals.h"
#include "gui/window.h"
#include "musicManager/musicManager.h"

#include "../Bibliotheque/Bibliotheque.h"
#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/Templates.h"
#include "../Carte/Carte.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "Jeu.h"


void Load_Chapitre(int Id)
{
	Disp_Chargement(255);

	string fileName = INSTALL_DIR + "chapitre/" + intToString(Id) + ".chp";

	ifstream fileStream(fileName.c_str(), ios_base::in);

	if (!fileStream.good()) Erreur("Le fichier suivant n'a pu être chargé :", fileName.c_str());
	if (fileStream.good()) MESSAGE(" Fichier \"" + fileName + "\" ouvert", FICHIER)

	string TypeDonnee = "", bufferString = "";

	while (fileStream.rdstate() == 0)
	{
		fileStream >> TypeDonnee >> bufferString;

		if		(TypeDonnee == "CARTE")	Load_Carte(bufferString, TYPE_CARTE);
		else if	(TypeDonnee == "LISTE")	Load_Carte(bufferString, TYPE_LISTE);
		else if (TypeDonnee == "QUEST")	addQuest(bufferString, "true");

		TypeDonnee = "";
	}

	fileStream.close();
}

/** ÉTABLISSEMENT D'UN NOUVEAU JEU OU CHARGEMENT DU JEU ENREGISTRÉ */

bool RechercheJoueur()
{
	auto carte = Partie.maps.begin();
	bool PersonnageTrouve = false;
	list<Element_Carte*>::iterator tmpj;

	while (carte != Partie.maps.end() && !PersonnageTrouve)
	{
		//Recherche du personnage qui contrôlera le jeu
		tmpj = carte->second.elements.begin();
		while(tmpj != carte->second.elements.end() && !PersonnageTrouve)
		{
			if ((*tmpj)->Get_Controle() == HUMAIN)
			{
				PersonnageTrouve = true;
				Partie.CarteCourante = &(carte->second); //CarteCourante est initialisée à la carte contenant le personnage
			}
			else ++tmpj;
		}

		++carte;
	}

	if (!PersonnageTrouve)
	{
		Erreur("Le jeu demandé ne contient pas de personnage" , "");
		return false;
	}

	Partie.perso = dynamic_cast<Joueur*>((*tmpj));

	if (Partie.perso == NULL)
	{
		Erreur("Le personnage trouvé n'est pas conforme" , "");
		return false;
	}

	Set_PosCarte(Partie.perso->PosX, Partie.perso->PosY, false);
	Set_PosCarte(0, 0, true);

	//Mise en place des éléments dans la liste de collisions :
	for (auto& tmp : Partie.CarteCourante->elements)
	{
		if (tmp->Get_Controle() != HUMAIN) tmp->Set_Controle(AI);
	}

	return true;
}

bool NouvellePartie()
{
	String32 NomPersonnage;
	int ResultatSaisie = SaisieNom(NomPersonnage);

	if (ResultatSaisie == -1) return false;

	if (ResultatSaisie == 1) Load_Chapitre(0);
	if (ResultatSaisie == 2) Load_Chapitre(1);

	MESSAGE(">> Chargement du nouveau jeu terminé <<", FICHIER)

	bool joueur = RechercheJoueur();
	if (!joueur) return false;

	//Nom du joueur et nom du dossier de sauvegarde :
	#ifdef DEBOGAGE
	if (!Arguments.SaveDisabled)
	#endif
	Partie.perso->Nom = NomPersonnage;
	#ifdef DEBOGAGE
	else
	{
		string strNom = "ModeDeTest";
		Utf8::toUtf32(strNom.begin(), strNom.end(), back_inserter(Partie.perso->Nom));
	}
	#endif

	#ifdef DEBOGAGE
	if (!Arguments.SaveDisabled)
	#endif
	{
		stringstream stream; stream << time(NULL);
		Partie.SAVE = stream.str();
	}

	//Mise à jour de la liste de sauvegarde, création du dossier, enregistrement de la nouvelle partie :

	#ifdef DEBOGAGE
	if (!Arguments.SaveDisabled)
	#endif
	{
        tools::filesystem::createDirectory(Get_DossierSauvegardes() + Partie.SAVE);
	}

    musicManager::playMusic(Partie.CarteCourante->ambience);
	MESSAGE(">> Mise en place du nouveau jeu terminée <<", FICHIER)

	return true;
}

bool PartieSauvegardee()
{
	Partie.SAVE = ChoixSauvegarde();

	if (Partie.SAVE == "ANNULER")
	{
		Partie.SAVE = "";
		return false;
	}
	Disp_Chargement(255);

	if (!Load_Partie(Partie.SAVE))
	{
		DialogueEtGestionEvenements("sauvegarde_incompatible.lng");
		return NouvellePartie();
	}

	MESSAGE(">> Chargement de la sauvegarde terminée <<", FICHIER)

	bool joueur = RechercheJoueur();
	if (!joueur) return false;

    musicManager::playMusic(Partie.CarteCourante->ambience);

	MESSAGE(">> Mise en place du jeu sauvegardé terminée <<", FICHIER)

	return true;
}

/** FONCTIONS DE GESTION DE LA PARTIE **/

void EcranJeu(bool SauvegardePrealable)
{
	float Chargement = 255;
	float ChangementLieu = 255;
	float SauvegardeEffectuee = 255;

    bool managementActivated = true;
    bool playerResting = false;
    bool playerDead = false;
    bool askExit = false;
    bool askIngameMenu = false;

	String32 NomLieu;

	if (Partie.CarteCourante == NULL)
	{
		Erreur("La fonction EcranJeu() a été appelée avec CarteCourante == NULL" , "");
		return;
	}

    gui::Window confirmExitGameWindow;
    confirmExitGameWindow.loadFromFile("gui/confirm-exit-game.xml", Jeu.App);

    gui::Window ingameMenuWindow;
    ingameMenuWindow.loadFromFile("gui/ingame-menu.xml", Jeu.App);

    //Clear signals before starting the main loop
    string s = tools::signals::getNextSignal();
    while (s != "")
    {
        tools::signals::removeSignal(s);
        s = tools::signals::getNextSignal();
    }

	while (true)
	{
		Jeu.App.clear();

        //1. Management, Events & Signals

        if (managementActivated)
            Partie.CarteCourante->GestionElements();

        Event event;
        while (Jeu.App.pollEvent(event))
        {
            Gestion_Menu(event);

            if (Partie.currentUserScreen != nullptr && Partie.currentUserScreen->manageFunction != nullptr)
                Partie.currentUserScreen->manageFunction(event);

            if (event.type == Event::KeyReleased)
            {
                switch (event.key.code)
                {
                    case Keyboard::M : tools::signals::addSignal("ask-menu"); break;
                    case Keyboard::S : tools::signals::addSignal("savegame"); break;
                    case Keyboard::R : tools::signals::addSignal("rest"); break;
                    case Keyboard::Q : tools::signals::addSignal("ask-exit"); break;
                    default : break;
                }
            }

            if (event.type == Event::Closed || (Keyboard::isKeyPressed(Keyboard::F4) && Keyboard::isKeyPressed(Keyboard::LAlt)))
                return;

            if (playerDead && Keyboard::isKeyPressed(Keyboard::Return))
                return;
        }

        string signal = tools::signals::getNextSignal();
        while (signal != "")
        {
            if (signal == "savegame")
            {
                Partie.currentUserScreen = nullptr;
                Disp_Menu();
                Disp_JaugesVie();
                Disp_Consoles();
                SauvegardeEffectuee = 254;
                Save_Partie();
            }

            if (signal == "rest")
            {
                managementActivated = false;
                playerResting = true;
                Partie.perso->Repos();
            }

            if (signal == "player-dead")
            {
                playerDead = true;
            }

            if (signal == "ask-menu")
            {
                ingameMenuWindow.startWindow(Jeu.App);
                askIngameMenu = true;
            }

            if (signal == "ask-exit")
            {
                confirmExitGameWindow.startWindow(Jeu.App);
                askExit = true;
            }

            if (signal == "exit")
            {
                return;
            }

            tools::signals::removeSignal(signal);
            signal = tools::signals::getNextSignal();
        }

        //2. Display

        Partie.CarteCourante->displayBackground();
        Partie.CarteCourante->display();

        if (!Partie.ModeCinematiques)
        {
            Disp_Menu();
            Disp_JaugesVie();
            Disp_Consoles();
        }

        if (Partie.currentUserScreen != NULL)
            Partie.currentUserScreen->dispFunction();
        else
            Disp_MiniaturesCompetences();

        Disp_FonduNoir(0);
        Chargement = Disp_Chargement(Chargement);

        if (playerResting)
        {
            bool animationFinished = Disp_Repos();
            if (animationFinished)
            {
                playerResting = false;
                managementActivated = true;
            }
        }

        if (playerDead)
        {
            Disp_Mort();
        }

        if (askIngameMenu)
        {
            ingameMenuWindow.manage(Jeu.App);
            askIngameMenu = false;
        }

        if (askExit)
        {
            confirmExitGameWindow.manage(Jeu.App);
            askExit = false;
        }

        Disp_FPS();

		//2. GESTION DES MISSIONS

		manageQuests();

		//4. CHANGEMENTS DE LIEU

		if (Partie.perso->IndiceLieu != Partie.perso->SauvegardeIndiceLieu && Chargement <= 0)
		{
			ChangementLieu = 254;
			NomLieu = getTranslatedNameOfPlace(Partie.perso->IndiceLieu);
			Partie.perso->SauvegardeIndiceLieu = Partie.perso->IndiceLieu;
		}
		if (ChangementLieu != 255)
		{
			ChangementLieu -= tools::timeManager::I((256-ChangementLieu)/24);
			if (ChangementLieu <= 0) ChangementLieu = 255;
			else
			{
				Disp_TexteCentre(NomLieu, Options.ScreenW/2 + 1, 121, Color(0, 0, 0, ChangementLieu), 30., Jeu.DayRoman);
				Disp_TexteCentre(NomLieu, Options.ScreenW/2, 120, Color(225, 0, 0, ChangementLieu), 30., Jeu.DayRoman);

			}
		}

		//5. SAUVEGARDE EFFECTUÉE

		if (SauvegardeEffectuee != 255)
		{
			SauvegardeEffectuee -= tools::timeManager::I((256-SauvegardeEffectuee)/24);
			if (SauvegardeEffectuee <= 0) SauvegardeEffectuee = 255;
			else
			{
				Disp_TexteCentre(_SAUVEGARDE, Options.ScreenW/2 + 1, 161, Color(0, 0, 0, SauvegardeEffectuee), 30., Jeu.DayRoman);
				Disp_TexteCentre(_SAUVEGARDE, Options.ScreenW/2, 160, Color(225, 0, 0, SauvegardeEffectuee), 30., Jeu.DayRoman);

			}
		}

		//6. SAUVEGARDE PRÉALABLE

		if (SauvegardePrealable && Chargement <= 0)
		{
			#ifdef DEBOGAGE
			if (!Arguments.SaveDisabled)
			#endif
			{
				AjouterSauvegarde();
				Save_Partie();
			}
			SauvegardePrealable = false;
		}

		//7. CHARGEMENT DU JEU APRÈS LE DIDACTICIEL

		if (Partie.loadFirstChapter)
		{
			String32 NomPersonnage = Partie.perso->Nom;
			Clean_Partie();
			Load_Chapitre(1);
			bool joueur = RechercheJoueur();
			if (!joueur)
			{
				Erreur("Le passage au chapitre 1 a échoué", "");
				return;
			}
			Partie.perso->Nom = NomPersonnage;
			Partie.loadFirstChapter = false;
		}

        tools::timeManager::frameDone();
		Jeu.App.display();
        musicManager::manageRunningMusics();
	}
}

void Clean_Partie()
{
	Partie.perso = NULL;
	Partie.CarteCourante = NULL;
	Partie.CoffreOuvert = NULL;
	Partie.selectedObject = nullptr;
	Partie.selectedSkill = nullptr;
	Partie.listDialogs.clear();
	Partie.currentUserScreen = nullptr;
	Partie.journal.entries.clear();

	Set_PosCarte(0, 0, false);
	Set_PosCarte(0, 0, true);

	deleteQuests();

	SupprimerLignesConsoles();

	SupprimerListe_Carte();

	deleteCommonClasses();
	deleteLandsClasses();
	deleteMovingLandsClasses();
}

/** GESTION DU MENU PRINCIPAL : ÉVÉNEMENTS ET ANIMATION  **/

int MenuPrincipal()
{
    gui::Button newGameButton, loadGameButton, optionsButton, exitButton;

    newGameButton.setCenterCoordinates(Options.ScreenW/2, 238);
    newGameButton.setTextFont(Jeu.DayRoman, 28);
    newGameButton.setAllText(getTranslatedMessage(_MENUPRINCIPAL_NOUVEAU));

    loadGameButton.setCenterCoordinates(Options.ScreenW/2, 298);
    loadGameButton.setTextFont(Jeu.DayRoman, 28);
    loadGameButton.setAllText(getTranslatedMessage(_MENUPRINCIPAL_CHARGER));

    optionsButton.setCenterCoordinates(Options.ScreenW/2, 358);
    optionsButton.setTextFont(Jeu.DayRoman, 28);
    optionsButton.setAllText(getTranslatedMessage(_MENUPRINCIPAL_OPTIONS));

    exitButton.setCenterCoordinates(Options.ScreenW/2, 418);
    exitButton.setTextFont(Jeu.DayRoman, 28);
    exitButton.setAllText(getTranslatedMessage(_MENUPRINCIPAL_QUITTER));

	while (true)
	{
		/* Gestion des evenements */
		Event event;
		while (Jeu.App.pollEvent(event))
		{
            if (newGameButton.activated(Jeu.App, event.type)) return NOUVEAU;
            if (loadGameButton.activated(Jeu.App, event.type)) return CHARGER;
            if (optionsButton.activated(Jeu.App, event.type)) return OPTIONS;
            if (exitButton.activated(Jeu.App, event.type)) return QUITTER;

			if (event.type == Event::KeyPressed)
			{
				switch (event.key.code)
				{
					case Keyboard::Escape :	return QUITTER;
					default: break;
				}
			}
			if (event.type == Event::Closed) break;
			if (Keyboard::isKeyPressed(Keyboard::F4) && Keyboard::isKeyPressed(Keyboard::LAlt)) return QUITTER;
		}
		/* Fin gestion des evenements */

		/* Affichage */

		Jeu.App.clear();
		Disp_FondMenus();

		Disp_TitrePage(_TITRE);

		static float Temps = 0;
		if (!(rand()%200) || Temps > 0)
		{
			static int X = 0, Y = 0;
			static int Taille = 0;
			if (Temps <= 0)
			{
				X = 100 + rand()%(Options.ScreenW - 200), Y = 100 + rand()%(Options.ScreenH - 200);
				Taille = 120 + rand()%80;
				Temps = 3;
			}
			else
			{
				Temps -= tools::timeManager::I(1);
				X += rand()%6-3;
				Y += rand()%6-3;
			}

			Disp_TexteCentre(_TITRE, X-4, Y-4, Color(0,0,0,96), Taille, Jeu.DayRoman);
			Disp_TexteCentre(_TITRE, X+4, Y+4, Color(0,0,0,96), Taille, Jeu.DayRoman);
			Disp_TexteCentre(_TITRE, X, Y, Color(50,192,192,128), Taille, Jeu.DayRoman);
		}

        newGameButton.display(Jeu.App);
        loadGameButton.display(Jeu.App);
        optionsButton.display(Jeu.App);
        exitButton.display(Jeu.App);

		//Numéro de version de Devilsai
		string version = string("Devilsai : version ") + VERSION + "   (" +  __DATE__ + ")";
		Disp_Texte(version, Options.ScreenW - 250, Options.ScreenH - 60, Color(175,255,255,255), 10.);

        tools::timeManager::frameDone();
		Jeu.App.display();
        musicManager::manageRunningMusics();
		/* Fin animation */
	}

	return QUITTER;
}
