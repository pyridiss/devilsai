
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

#include "../Bibliotheque/Bibliotheque.h"
#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/Templates.h"
#include "../Carte/Carte.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "Jeu.h"


void Load_Chapitre(int Id)
{
	Disp_Chargement(255);

	string fileName = Partie.DATA + "chapitre/" + intToString(Id) + ".chp";

	ifstream fileStream(fileName.c_str(), ios_base::in);

	if (fileStream == NULL) Erreur("Le fichier suivant n'a pu être chargé :", fileName.c_str());
	if (fileStream != NULL) MESSAGE(" Fichier \"" + fileName + "\" ouvert", FICHIER)

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
		if ((tmp->RayonCollision || tmp->RayX || tmp->RayY) && tmp->AjouterDansListeCollision) addCollider(tmp);
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
		CreerDossier(Get_DossierSauvegardes() + Partie.SAVE);
	}

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

	MESSAGE(">> Mise en place du jeu sauvegardé terminée <<", FICHIER)

	return true;
}

/** FONCTIONS DE GESTION DE LA PARTIE **/

void EcranJeu(bool SauvegardePrealable)
{
	int Action = ACTION_JEU;

	float Chargement = 255;
	float ChangementLieu = 255;
	float SauvegardeEffectuee = 255;

	String32 NomLieu;

	if (Partie.CarteCourante == NULL)
	{
		Erreur("La fonction EcranJeu() a été appelée avec CarteCourante == NULL" , "");
		return;
	}

	while (true)
	{
		Jeu.App.clear();

		//1. GESTION DE LA CARTE ET DES ÉLÉMENTS

		Partie.CarteCourante->Disp_FondCarte();

		Action = Partie.CarteCourante->GestionElements();

		if (Action == ACTION_SAUVEG)
		{
			Partie.currentUserScreen = nullptr;
			Disp_Menu();
			Disp_JaugesVie();
			Disp_Consoles();
			SauvegardeEffectuee = 254;
			Save_Partie();
			continue;
		}

		if (Action == ACTION_JEU)
		{
			if (Partie.currentUserScreen != NULL)
				Partie.currentUserScreen->dispFunction();
			else
				Disp_MiniaturesCompetences();

			if (!Partie.ModeCinematiques)
			{
				Disp_Menu();
				Disp_JaugesVie();
				Disp_Consoles();
			}

			Disp_FonduNoir(0);
			Chargement = Disp_Chargement(Chargement);

			Disp_FPS();
		}
		while (Action == ACTION_PAUSE)
		{
			Event event;

			//Réaffichage de la carte telle quelle
			Jeu.App.clear();
			Partie.CarteCourante->Disp_FondCarte();
			for (auto& tmp : Partie.CarteCourante->elements)
				tmp->Disp(Partie.PosCarteX, Partie.PosCarteY);

			while (Jeu.App.pollEvent(event))
			{
				Action = Gestion_Menu(event);

				if (Action == ACTION_JEU) Action = ACTION_PAUSE;
				else if (Action == ACTION_PAUSE) Action = ACTION_JEU;

				if (event.type == Event::KeyReleased)
				{
					switch (event.key.code)
					{
						case Keyboard::P :	Action = ACTION_JEU; break;
						default :			break;
					}
				}
				if (event.type == Event::Closed || (Keyboard::isKeyPressed(Keyboard::F4) && Keyboard::isKeyPressed(Keyboard::LAlt)))
					Action = ACTION_QUITTER;
			}

			Disp_Menu();
			Disp_JaugesVie();
			Disp_Pause();

			Disp_FPS();

			Jeu.App.display();
		}
		while (Action == ACTION_REPOS)
		{
			Partie.perso->Repos();

			Event event;

			if (Disp_Repos()) Action = ACTION_JEU;

			while (Jeu.App.pollEvent(event))
			{
				if (event.type == Event::KeyReleased)
				{
					switch (event.key.code)
					{
						case Keyboard::Escape :	Action = ACTION_JEU; break;
						default :				break;
					}
				}
				if (event.type == Event::Closed || (Keyboard::isKeyPressed(Keyboard::F4) && Keyboard::isKeyPressed(Keyboard::LAlt)))
					Action = ACTION_QUITTER;
			}

			Disp_FPS();

			Jeu.App.display();
		}
		while (Action == ACTION_MORT)
		{
			Event event;

			Jeu.App.clear();

			Partie.CarteCourante->Disp_FondCarte();
			Partie.CarteCourante->GestionElements();
			Disp_Menu();
			if (Action == ACTION_MORT) Disp_Mort();

			while (Jeu.App.pollEvent(event))
			{
				if (event.type == Event::KeyReleased || event.type == Event::KeyPressed)
				{
					switch (event.key.code)
					{
						case Keyboard::Return :	Action = ACTION_QUITTER; break;
						default:				break;
					}
				}
				if (event.type == Event::Closed || (Keyboard::isKeyPressed(Keyboard::F4) && Keyboard::isKeyPressed(Keyboard::LAlt)))
					Action = ACTION_QUITTER;
			}

			Jeu.App.display();
		}
		if (Action == ACTION_QUITTER)
			break;

		Set_PosCarte(0, 0, true);

		//2. GESTION DES MISSIONS

		manageQuests();

		//3. OPTIMISATION DE LA LISTE DE COLLISION
		TriCollision();

		//4. CHANGEMENTS DE LIEU

		if (Partie.perso->IndiceLieu != Partie.perso->SauvegardeIndiceLieu && Chargement <= 0)
		{
			ChangementLieu = 254;
			NomLieu = getTranslatedNameOfPlace(Partie.perso->IndiceLieu);
			Partie.perso->SauvegardeIndiceLieu = Partie.perso->IndiceLieu;
		}
		if (ChangementLieu != 255)
		{
			ChangementLieu -= I((256-ChangementLieu)/24);
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
			SauvegardeEffectuee -= I((256-SauvegardeEffectuee)/24);
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

		Jeu.App.display();
		manageRunningMusics();
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

	removeColliders();
	SupprimerLignesConsoles();

	SupprimerListe_Carte();

	deleteCommonClasses();
	deleteLandsClasses();
	deleteMovingLandsClasses();
}

/** GESTION DU MENU PRINCIPAL : ÉVÉNEMENTS ET ANIMATION  **/

int MenuPrincipal()
{
	Bouton B_NouveauJeu, B_Charger, B_Options, B_Quitter;
	B_NouveauJeu.Creer(Options.ScreenW/2, 238, getTranslatedMessage(_MENUPRINCIPAL_NOUVEAU), 28., Jeu.DayRoman);
	B_Charger.Creer(Options.ScreenW/2, 298, getTranslatedMessage(_MENUPRINCIPAL_CHARGER), 28., Jeu.DayRoman);
	B_Options.Creer(Options.ScreenW/2, 358, getTranslatedMessage(_MENUPRINCIPAL_OPTIONS), 28., Jeu.DayRoman);
	B_Quitter.Creer(Options.ScreenW/2, 418, getTranslatedMessage(_MENUPRINCIPAL_QUITTER), 28., Jeu.DayRoman);

	while (true)
	{
		/* Gestion des evenements */
		Event event;
		while (Jeu.App.pollEvent(event))
		{
			if (event.type == Event::MouseButtonPressed || event.type == Event::MouseButtonReleased)
			{
				if (B_NouveauJeu.TestActivation(event.type)) return NOUVEAU;
				if (B_Charger.TestActivation(event.type)) return CHARGER;
				if (B_Options.TestActivation(event.type)) return OPTIONS;
				if (B_Quitter.TestActivation(event.type)) return QUITTER;
			}
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
				Temps -= I(1);
				X += rand()%6-3;
				Y += rand()%6-3;
			}

			Disp_TexteCentre(_TITRE, X-4, Y-4, Color(0,0,0,96), Taille, Jeu.DayRoman);
			Disp_TexteCentre(_TITRE, X+4, Y+4, Color(0,0,0,96), Taille, Jeu.DayRoman);
			Disp_TexteCentre(_TITRE, X, Y, Color(50,192,192,128), Taille, Jeu.DayRoman);
		}

		B_NouveauJeu.Disp();
		B_Charger.Disp();
		B_Options.Disp();
		B_Quitter.Disp();

		//Numéro de version de Devilsai
		string version = string("Devilsai : version ") + VERSION + "   (" +  __DATE__ + ")";
		Disp_Texte(version, Options.ScreenW - 250, Options.ScreenH - 60, Color(175,255,255,255), 10.);

		Jeu.App.display();
		manageRunningMusics();
		/* Fin animation */
	}

	return QUITTER;
}
