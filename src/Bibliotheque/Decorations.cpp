
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

#include "../Jeu/Jeu.h"
#include "../Carte/Carte.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "Bibliotheque.h"
#include "Constantes.h"

#include "tools/textManager.h"
#include "tools/timeManager.h"
#include "tools/signals.h"
#include "imageManager/imageManager.h"
#include "imageManager/animation.h"
#include "gui/button.h"
#include "musicManager/musicManager.h"

/** VARIABLES GLOBALES **/

LigneConsole ConsolePerso[10];
LigneConsole ConsoleAmeliorations[10];

gui::Button *menuButton;
gui::Button *restingButton;

void displayMenu();
void manageMenu(Event &event);


/** GESTION DES DÉCORATIONS **/

void Load_Decorations()
{
    imageManager::addContainer("misc");
	imageManager::addImage("misc", "Fond", INSTALL_DIR + "img/Fond.png", Vector2i(0, 0), (float)Options.ScreenW/400.f);
	imageManager::addImage("misc", "Art", INSTALL_DIR + "img/Art.png");
	imageManager::addImage("misc", "FondObjet_75_100", INSTALL_DIR + "img/FondObjet_75_100.png");
	imageManager::addImage("misc", "FondObjet_75_75", INSTALL_DIR + "img/FondObjet_75_75.png");
	imageManager::addImage("misc", "FondObjet_50_50", INSTALL_DIR + "img/FondObjet_50_50.png");
	imageManager::addImage("misc", "FondCoffre", INSTALL_DIR + "img/FondCoffre.png");
	imageManager::addImage("misc", "FondInventaire", INSTALL_DIR + "img/FondInventaire.png");
	imageManager::addImage("misc", "FondMiniaturesCompetences", INSTALL_DIR + "img/FondMiniaturesCompetences.png");
	imageManager::addImage("misc", "Repos", INSTALL_DIR + "img/Repos.png");
	imageManager::addImage("misc", "BoutonPersonnage", INSTALL_DIR + "img/BoutonPersonnage.png", Vector2i(0, 0), 0.38);
	imageManager::addImage("misc", "BoutonEquipement", INSTALL_DIR + "img/BoutonEquipement.png", Vector2i(0, 0), 0.38);
	imageManager::addImage("misc", "BoutonCompetences", INSTALL_DIR + "img/BoutonCompetences.png", Vector2i(0, 0), 0.38);
	imageManager::addImage("misc", "BoutonJournal", INSTALL_DIR + "img/BoutonJournal.png", Vector2i(0, 0), 0.38);
    imageManager::addImage("misc", "gui-menu-button", INSTALL_DIR + "img/gui-menu-button.png", Vector2i(0, 0), 0.38);
    imageManager::addImage("misc", "gui-rest-button", INSTALL_DIR + "img/gui-rest-button.png", Vector2i(0, 0), 0.38);
    imageManager::addImage("misc", "gui-button-background-156x46", INSTALL_DIR + "img/gui-button-background-156x46.png");
    imageManager::addImage("misc", "gui-dialog-background-488x308", INSTALL_DIR + "img/gui-dialog-background-488x308.png");
    imageManager::addImage("misc", "gui-input-field-background-356x46", INSTALL_DIR + "img/gui-input-field-background-356x46.png");
    imageManager::addImage("misc", "gui-top-border", INSTALL_DIR + "img/gui-top-border.png");

    imageManager::addAnimation("playerLifeGauge", INSTALL_DIR + "img/BarreVie.png");
    imageManager::addAnimation("playerLifeGaugeBackground", INSTALL_DIR + "img/BarreVie.png");
    imageManager::addAnimation("playerEnergyGauge", INSTALL_DIR + "img/BarreEnergie.png");
    imageManager::addAnimation("playerEnergyGaugeBackground", INSTALL_DIR + "img/BarreEnergie.png");
    imageManager::addAnimation("playerRecoveryGauge", INSTALL_DIR + "img/BarreRecup.png");
    imageManager::addAnimation("interactorLifeGauge", INSTALL_DIR + "img/BarreVie.png");

    Partie.screenCharacter.button.setTopLeftCoordinates(60, 2);
    Partie.screenCharacter.button.setSize(41, 41);
    Partie.screenCharacter.button.setAllBackground("BoutonPersonnage");
    Partie.screenCharacter.button.setForegroundShader("hover", gui::style::highlightShader);
	Partie.screenCharacter.key = Keyboard::Key::A;
	Partie.screenCharacter.dispFunction = Disp_Personnage;
	Partie.screenCharacter.manageFunction = nullptr;

    Partie.screenEquipment.button.setTopLeftCoordinates(110, 2);
    Partie.screenEquipment.button.setSize(41, 41);
    Partie.screenEquipment.button.setAllBackground("BoutonEquipement");
    Partie.screenEquipment.button.setForegroundShader("hover", gui::style::highlightShader);
	Partie.screenEquipment.key = Keyboard::Key::E;
	Partie.screenEquipment.dispFunction = Disp_Equipement;
	Partie.screenEquipment.manageFunction = Gestion_Coffre;

    Partie.screenSkills.button.setTopLeftCoordinates(160, 2);
    Partie.screenSkills.button.setSize(41, 41);
    Partie.screenSkills.button.setAllBackground("BoutonCompetences");
    Partie.screenSkills.button.setForegroundShader("hover", gui::style::highlightShader);
	Partie.screenSkills.key = Keyboard::Key::K;
	Partie.screenSkills.dispFunction = Disp_Competences;
	Partie.screenSkills.manageFunction = Gestion_Competences;

    Partie.screenJournal.button.setTopLeftCoordinates(210, 2);
    Partie.screenJournal.button.setSize(41, 41);
    Partie.screenJournal.button.setAllBackground("BoutonJournal");
    Partie.screenJournal.button.setForegroundShader("hover", gui::style::highlightShader);
	Partie.screenJournal.key = Keyboard::Key::J;
	Partie.screenJournal.dispFunction = displayJournal;
	Partie.screenJournal.manageFunction = nullptr;

    menuButton = new gui::Button;
    menuButton->setTopLeftCoordinates(10, 2);
    menuButton->setSize(41, 41);
    menuButton->setAllBackground("gui-menu-button");
    menuButton->setForegroundShader("hover", gui::style::highlightShader);
    menuButton->setForegroundShader("disabled", gui::style::disableShader);

    restingButton = new gui::Button;
    restingButton->setTopLeftCoordinates(260, 2);
    restingButton->setSize(41, 41);
    restingButton->setAllBackground("gui-rest-button");
    restingButton->setForegroundShader("hover", gui::style::highlightShader);
    restingButton->setForegroundShader("disabled", gui::style::disableShader);

    tools::textManager::loadFile("devilsai", "lng/devilsai_FR.xml");

	Load_Decorations_Objets();

    musicManager::addSound("Click");

    musicManager::addMusic("Gates_Of_Heaven");
}

void Supprimer_Decorations()
{
    delete menuButton;
	delete restingButton;

	Supprimer_Decorations_Objets();
    musicManager::deleteMusics();
}


/** F.P.S. **/

void Disp_FPS()
{
    float Framerate = tools::timeManager::getFPS();
    Text FPS("FPS : " + intToString(Framerate), gui::style::defaultTextFont(), 10);
    FPS.setPosition(Options.ScreenW - 100, 16);
    Jeu.App.draw(FPS);
}

/** FOND POUR LES MENUS **/

void Disp_FondMenus()
{
	static float Animation = 350;
	static RectangleShape Masque(Vector2f(Options.ScreenW, Options.ScreenH));
	Masque.setFillColor(Color(0,0,0,0));

	imageManager::display(Jeu.App, "misc", "Fond", 0, 0);

	Animation += tools::timeManager::I(0.75);

	if (Animation < 250) Masque.setFillColor(Color((255-Animation)/2,0,0,Animation));
	if (Animation >= 250 && Animation < 350) Masque.setFillColor(Color(0,0,0,250));
	if (Animation >= 350 && Animation < 600) Masque.setFillColor(Color(0,0,(Animation-350)/1.5,600-Animation));
	if (Animation >= 600 && Animation < 700) Masque.setFillColor(Color(0,0,165,0));
	if (Animation >= 700) Animation = 0;

	Jeu.App.draw(Masque);

	Disp_FPS();
}

/** MENU SUPÉRIEUR **/

void Gestion_Menu(Event &event)
{
    if (menuButton->activated(Jeu.App, event))
        tools::signals::addSignal("ask-menu");

    if (Partie.perso->LieuVillage != "village") restingButton->setDisabled(true);
    else restingButton->setDisabled(false);

    if (restingButton->activated(Jeu.App, event))
        tools::signals::addSignal("rest");

    if (Partie.screenCharacter.button.activated(Jeu.App, event))	Partie.changeCurrentUserScreen(&Partie.screenCharacter);
    if (Partie.screenEquipment.button.activated(Jeu.App, event))	Partie.changeCurrentUserScreen(&Partie.screenEquipment);
    if (Partie.screenSkills.button.activated(Jeu.App, event))		Partie.changeCurrentUserScreen(&Partie.screenSkills);
    if (Partie.screenJournal.button.activated(Jeu.App, event))		Partie.changeCurrentUserScreen(&Partie.screenJournal);

    if (event.type == Event::KeyReleased)
    {
        if (event.key.code == Partie.screenCharacter.key) Partie.changeCurrentUserScreen(&Partie.screenCharacter);
        if (event.key.code == Partie.screenEquipment.key) Partie.changeCurrentUserScreen(&Partie.screenEquipment);
        if (event.key.code == Partie.screenSkills.key) Partie.changeCurrentUserScreen(&Partie.screenSkills);
        if (event.key.code == Partie.screenJournal.key) Partie.changeCurrentUserScreen(&Partie.screenJournal);
    }
}

void Disp_Menu()
{
	for (unsigned short a = 0 ; a < Options.ScreenW/220 + 1 ; ++a)
    {
		imageManager::display(Jeu.App, "misc", "gui-top-border", 220*a, 0);
    }

    menuButton->display(Jeu.App);
    restingButton->display(Jeu.App);

	//Erreur éventuelle
	if (Jeu.ErreurDetectee)
	{
        Disp_Texte(_ERREUR, 350, 16, Color(255,0,0,255), 10.);
	}

    Partie.screenCharacter.button.display(Jeu.App);
    Partie.screenEquipment.button.display(Jeu.App);
    Partie.screenSkills.button.display(Jeu.App);
    Partie.screenJournal.button.display(Jeu.App);

    if (Partie.journal.newEntryAdded)
        Partie.screenJournal.button.setForegroundShader("normal", gui::style::warnShader);
    else
        Partie.screenJournal.button.setForegroundShader("normal", nullptr);
}

/** CONSOLES DU PERSONNAGE **/

void Disp_JaugesVie()
{
	static int PersoEnePrec = Partie.perso->get("Energie");
	static int AncienInteraction = 0;

	static Element_Carte *Elem; static Individu* Ind;

    static imageManager::Animation* playerLifeGauge = imageManager::getAnimation("playerLifeGauge");
    static imageManager::Animation* playerLifeGaugeBackground = imageManager::getAnimation("playerLifeGaugeBackground");
    static imageManager::Animation* playerEnergyGauge = imageManager::getAnimation("playerEnergyGauge");
    static imageManager::Animation* playerEnergyGaugeBackground = imageManager::getAnimation("playerEnergyGaugeBackground");
    static imageManager::Animation* playerRecoveryGauge = imageManager::getAnimation("playerRecoveryGauge");

	Disp_TexteCentre(Partie.perso->Nom, 92, 60, Color(128, 255, 128, 255), 12.f);

	//1. Jauges de vitalité, d'énergie, de récupération

    playerLifeGauge->setSmoothRectangle(0, 0, Partie.perso->get("Vitalite") / 10, 7);
    playerEnergyGauge->setSmoothRectangle(0, 0, Partie.perso->get("Energie") / 10, 7);

    if (Partie.perso->get("Vitalite") < 50) playerLifeGaugeBackground->setFlickering(0.5);
    else if (Partie.perso->get("Vitalite") < 100) playerLifeGaugeBackground->setFlickering(0.25);
    else
    {
        playerLifeGaugeBackground->setFlickering(0);
        playerLifeGaugeBackground->setColor(Color(0, 0, 0, 255));
    }

    if (Partie.perso->get("Energie") < 50) playerEnergyGaugeBackground->setFlickering(0.5);
    else if (Partie.perso->get("Energie") < 100) playerEnergyGaugeBackground->setFlickering(0.25);
    else
    {
        playerEnergyGaugeBackground->setFlickering(0);
        playerEnergyGaugeBackground->setColor(Color(0, 0, 0, 255));
    }

    playerLifeGaugeBackground->display(Jeu.App, 42, 70, false);
    playerLifeGauge->display(Jeu.App, 42, 70, false);
    playerEnergyGaugeBackground->display(Jeu.App, 42, 79, false);
    playerEnergyGauge->display(Jeu.App, 42, 79, false);

	int Recup = Partie.perso->get("Recuperation");

	if (Recup > 0)
	{
        playerRecoveryGauge->setRectangle(51, 0, (Recup-(Recup/98)*Recup%98)/2, 7);
        playerRecoveryGauge->display(Jeu.App, 93, 88, false);
	}
	if (Recup < 0)
	{
        playerRecoveryGauge->setRectangle(51+Recup/2, 0, - Recup/2, 7);
        playerRecoveryGauge->display(Jeu.App, 92+Recup/2, 88, false);
	}

	//2. État général, fatigue si nécessaire, effet d'une potion
	if		(Partie.perso->get("Vitalite") == 0)				Disp_TexteCentre(_MORT, 92, 105, Color(168, 168, 168, 255), 11.f);
	else if	(Partie.perso->get("Vitalite") + Recup * 10 >= 900)	Disp_TexteCentre(_SANTE1, 92, 105, Color(128, 255, 128, 255), 11.f);
	else if (Partie.perso->get("Vitalite") + Recup * 10 >= 650)	Disp_TexteCentre(_SANTE2, 92, 105, Color(255, 220, 30, 255), 11.f);
	else if (Partie.perso->get("Vitalite") + Recup * 10 >= 300)	Disp_TexteCentre(_SANTE3, 92, 105, Color(255, 190, 10, 255), 11.f);
	else if (Partie.perso->get("Vitalite") + Recup * 10 >= 100)	Disp_TexteCentre(_SANTE4, 92, 105, Color(255, 80, 10, 255), 11.f);
	else 														Disp_TexteCentre(_SANTE5, 92, 105, Color(255, 0, 0, 255), 11.f);

	if (Partie.perso->get("Energie") < 140)
	{
		if (PersoEnePrec >= 140) Disp_Information(_FATIGUE, true);
		Disp_TexteCentre(_SANTE_FATIGUE, 92, 120, Color(255, 255, 128, 255), 11.f);
	}
	PersoEnePrec = Partie.perso->get("Energie");

	//Effets dûs aux objets temporaires
	int y = 70;
	for (mapObjects::iterator i = Partie.perso->Get_Caracs()->objects.objects.begin() ; i != Partie.perso->Get_Caracs()->objects.objects.end() ; ++i)
	{
		if (getStringFromLUA(i->second, "getIdEmplacement") == i->first)
		{
			if (getStringFromLUA(i->second, "getCategorieObjet") == "temporaire")
			{
				String32 name = getTranslatedNameOfObject(getIntFromLUA(i->second, "getInternalNumber"));
				Disp_Texte(getFormatedTranslatedMessage(_SOUS_EFFET, name), 160, y, Color(255, 255, 128, 255), 11.f);
				y += 12;
			}
		}
	}

	Disp_Information(_TITRE, false);

	//3. Individu en interaction

	static unsigned char TransInd = 0;
	static int sIVit = 0, sIRec = 0;
	static String32 NomInd;
	static Color CouleurNom = Color(255, 255, 255, 255);
	static RectangleShape MasqueInter(Vector2f(100, 7));
	MasqueInter.setPosition(Options.ScreenW - 114, Options.ScreenH/2 - 25);

    static imageManager::Animation* interactorLifeGauge = imageManager::getAnimation("interactorLifeGauge");

	if (TransInd != 0)
	{
		Disp_TexteCentre(NomInd, Options.ScreenW - 64, Options.ScreenH/2 - 35, CouleurNom, 12.f);

		MasqueInter.setFillColor(Color(0, 0, 0, (TransInd > 80) ? TransInd-80 : 0));
		Jeu.App.draw(MasqueInter);

        interactorLifeGauge->setSmoothRectangle(0, 0, sIVit/10, 7);
        interactorLifeGauge->setColor(Color(255, 255, 255, TransInd));
        interactorLifeGauge->display(Jeu.App, Options.ScreenW - 114, Options.ScreenH/2 - 25, false);

		if		(sIVit == 0)				Disp_TexteCentre(_MORT, Options.ScreenW - 64, Options.ScreenH/2 - 10, Color(168, 168, 168, TransInd), 11.f);
		else if	(sIVit/10 + sIRec >= 90)	Disp_TexteCentre(_SANTE1, Options.ScreenW - 64, Options.ScreenH/2 - 10, Color(128, 255, 128, TransInd), 11.f);
		else if (sIVit/10 + sIRec >= 65)	Disp_TexteCentre(_SANTE2, Options.ScreenW - 64, Options.ScreenH/2 - 10, Color(255, 220, 30, TransInd), 11.f);
		else if (sIVit/10 + sIRec >= 30)	Disp_TexteCentre(_SANTE3, Options.ScreenW - 64, Options.ScreenH/2 - 10, Color(255, 190, 10, TransInd), 11.f);
		else if (sIVit/10 + sIRec >= 10)	Disp_TexteCentre(_SANTE4, Options.ScreenW - 64, Options.ScreenH/2 - 10, Color(255, 80, 10, TransInd), 11.f);
		else 								Disp_TexteCentre(_SANTE5, Options.ScreenW - 64, Options.ScreenH/2 - 10, Color(255, 0, 0, TransInd), 11.f);
	}

	if (Partie.perso->ElementInteraction != -1 && Partie.perso->ElementInteraction != AncienInteraction)
	{
		Elem = Get_Element(Partie.perso->ElementInteraction);
		if (Elem == NULL) return;

		if (Elem->Diplomatie != 0)
		{
			TransInd = 255;
			Ind = dynamic_cast<Individu*>(Elem);

			if (Ind == NULL)
			{
				Erreur("Disp_JaugesVie() tente d'afficher les statistiques d'un élément non individu", "");
				return;
			}

			NomInd = Ind->Get_Nom();
			if (Elem->Diplomatie == DIPLOM_ALLIE) CouleurNom = Color(128, 255, 128, 255);
			if (Elem->Diplomatie == DIPLOM_ENNEMI) CouleurNom = Color(255, 255, 255, 255);
            interactorLifeGauge->setRectangle(0, 0, Ind->get("Vitalite")/10, 7);
		}
		else Ind = NULL;
		AncienInteraction = Partie.perso->ElementInteraction;
	}
	if (Ind != NULL && Partie.perso->ElementInteraction != -1)
	{
		sIVit = Ind->get("Vitalite");
		sIRec = Ind->get("Recuperation");
	}

	if ((Ind == NULL || Partie.perso->ElementInteraction == -1) && TransInd > 0)
	{
		Elem = NULL; Ind = NULL;
		TransInd -= 5;
		if (CouleurNom.r == 128) CouleurNom = Color(128, 255, 128, TransInd);
		if (CouleurNom.r == 255) CouleurNom = Color(255, 255, 255, TransInd);
		AncienInteraction = 0;
	}
}

void Ajouter_LignePerso(String32 ligne, Color couleur)
{
	if (!Options.AffichageDegats) return;

	for(int a = 0 ; a < 10 ; ++a)
	{
		if (ConsolePerso[a].Affichage) --ConsolePerso[a].NumLigne;
	}

	short NouvelleLigne = -1;

	for (int b = 0 ; b < 10 ; ++b)
	{
		if (!ConsolePerso[b].Affichage) NouvelleLigne = b;
	}

	if (NouvelleLigne == -1)
	{
		int TempsMax = 0; int Remplacement = 0;
		for (int c = 0 ; c < 10 ; ++c)
		{
			if (ConsolePerso[c].Temps > TempsMax)
			{
				Remplacement = c;
				TempsMax = ConsolePerso[c].Temps;
			}
		}
		NouvelleLigne = Remplacement;
	}

	ConsolePerso[NouvelleLigne].Ligne = ligne;
	ConsolePerso[NouvelleLigne].Couleur = couleur;
	ConsolePerso[NouvelleLigne].Temps = 0;
	ConsolePerso[NouvelleLigne].NumLigne = 10;
	ConsolePerso[NouvelleLigne].Affichage = true;
}

void Ajouter_LigneAmelioration(String32 ligne, Color couleur)
{
	for(int a = 0 ; a < 10 ; ++a)
	{
		if (ConsoleAmeliorations[a].Affichage) --ConsoleAmeliorations[a].NumLigne;
	}

	short NouvelleLigne = -1;

	for (int b = 0 ; b < 10 ; ++b)
	{
		if (!ConsoleAmeliorations[b].Affichage) NouvelleLigne = b;
	}

	if (NouvelleLigne == -1)
	{
		int TempsMax = 0; int Remplacement = 0;
		for (int c = 0 ; c < 10 ; ++c)
		{
			if (ConsoleAmeliorations[c].Temps > TempsMax)
			{
				Remplacement = c;
				TempsMax = ConsoleAmeliorations[c].Temps;
			}
		}
		NouvelleLigne = Remplacement;
	}

	ConsoleAmeliorations[NouvelleLigne].Ligne = ligne;
	ConsoleAmeliorations[NouvelleLigne].Couleur = couleur;
	ConsoleAmeliorations[NouvelleLigne].Temps = 0;
	ConsoleAmeliorations[NouvelleLigne].NumLigne = 10;
	ConsoleAmeliorations[NouvelleLigne].Affichage = true;
}

void Disp_Consoles()
{
	for (int a = 0 ; a < 10 ; ++a)
	{
		if (ConsolePerso[a].Affichage)
		{
			ConsolePerso[a].Temps += tools::timeManager::I(1);
			if (ConsolePerso[a].Temps < 300)
				Disp_Texte(ConsolePerso[a].Ligne, Options.ScreenW - 115, 90+15*ConsolePerso[a].NumLigne, ConsolePerso[a].Couleur, 13.f);

			if (ConsolePerso[a].Temps == 300) ConsolePerso[a].Affichage = false;

			//Animation de dégradé :
			if (ConsolePerso[a].Temps > 175 && (int)ConsolePerso[a].Temps % 5 == 0)
				ConsolePerso[a].Couleur = Color(ConsolePerso[a].Couleur.r, ConsolePerso[a].Couleur.g, ConsolePerso[a].Couleur.b, 600-ConsolePerso[a].Temps*2);
		}
		if (ConsoleAmeliorations[a].Affichage)
		{
			ConsoleAmeliorations[a].Temps += tools::timeManager::I(1);
			if (ConsoleAmeliorations[a].Temps < 345)
				Disp_Texte(ConsoleAmeliorations[a].Ligne, 210, 400-16*ConsoleAmeliorations[a].NumLigne, ConsoleAmeliorations[a].Couleur, 13.f);

			if (ConsoleAmeliorations[a].Temps == 300) ConsoleAmeliorations[a].Affichage = false;
		}
	}
}

void SupprimerLignesConsoles()
{
	for (int a = 0 ; a < 10 ; ++a)
	{
		ConsolePerso[a].Affichage = false;
		ConsoleAmeliorations[a].Affichage = false;
	}
}

/** SITUATIONS PARTICULIÈRES **/

void Disp_Information(enumPhrases info, bool reactiver)
{
	static float Temps = 0;
	static enumPhrases Information;
	if (reactiver)
	{
		Temps = 250;
		Information = info;
		return;
	}
	if (Temps > 0)
	{
		Disp_TexteCentre(Information, Options.ScreenW/2, Options.ScreenH/2 - 100, Color(255,255,255,Temps), 20., Jeu.DayRoman);
		Temps -= tools::timeManager::I((256-Temps)/24);
	}
}

void Disp_Mort()
{
	static RectangleShape Fond(Vector2f(Options.ScreenW, Options.ScreenH));
	//Peut-être moduler la couleur…
	Fond.setFillColor(Color(200, 0, 0, 128));
	Jeu.App.draw(Fond);
	Disp_TexteCentre(_AFFICHAGE_MORT, Options.ScreenW/2, 280, Color(255,255,255,255), 40., Jeu.DayRoman);
	Disp_TexteCentre(_RETOUR_MENU_ENTREE, Options.ScreenW/2, 320, Color(255,255,255,255), 18.);
}

void Disp_FonduNoir(int Commande)
{
	static float Avancement = 0;

	if (Commande > 0 && Avancement == 0)
	{
		Avancement = 1;
	}
	if (Commande < 0) Avancement = 0;
	if (Avancement > 0 && Avancement <= 255)
	{
		static RectangleShape Fond(Vector2f(Options.ScreenW, Options.ScreenH));

		Fond.setFillColor(Color(0, 0, 0, Avancement));
		Jeu.App.draw(Fond);
		Avancement += tools::timeManager::I(1);
		if (Avancement > 255) Avancement = 255;
	}
}

bool Disp_Repos()
{
	static float Animation = 0;
	static float T1 = 255, T2 = 255, T3 = 255, T4 = 255;

	Jeu.App.clear(Color::Black);

	Animation += tools::timeManager::I(1);

	if (Animation < 255)
	{
		if (T1 > 5) T1 -= tools::timeManager::I(1);
		if (T2 > 5) T2 -= tools::timeManager::I(0.75);
		if (T3 > 5) T3 -= tools::timeManager::I(0.5);
	}
	else if (Animation < 510)
	{
		if (T1 < 250) T1 += tools::timeManager::I(0.25);
		if (T2 < 250) T2 += tools::timeManager::I(0.5);
		if (T3 > 5) T3 -= tools::timeManager::I(0.25);
		if (T4 > 5) T4 -= tools::timeManager::I(1);
	}
	else
	{
		if (T1 < 250) T1 += tools::timeManager::I(1);
		if (T2 < 250) T2 += tools::timeManager::I(1);
		if (T3 < 250) T3 += tools::timeManager::I(1);
		if (T4 < 250) T4 += tools::timeManager::I(1);
	}
	if (Animation > 800)
	{
		Animation = 0;
		T1 = 255; T2 = 255; T3 = 255; T4 = 255;
		return true;
	}

	sf::Vertex Degrade[] =
	{
		sf::Vertex(sf::Vector2f(0, 0), Color(0, 0, 0, T1)),
		sf::Vertex(sf::Vector2f(0, Options.ScreenH), Color(0, 0, 0, T2)),
		sf::Vertex(sf::Vector2f(Options.ScreenW, Options.ScreenH), Color(0, 0, 0, T3)),
		sf::Vertex(sf::Vector2f(Options.ScreenW, 0), Color(0, 0, 0, T4))
	};

	imageManager::display(Jeu.App, "misc", "Repos", Options.ScreenW/2, Options.ScreenH/2, true);
 	Jeu.App.draw(Degrade, 4, sf::Quads);
	return false;
}
