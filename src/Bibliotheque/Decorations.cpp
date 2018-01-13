
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

#include "gamedata.h"

#include "Jeu/options.h"

/** VARIABLES GLOBALES **/

LigneConsole ConsolePerso[10];
LigneConsole ConsoleAmeliorations[10];


/** GESTION DES DÉCORATIONS **/

void Load_Decorations()
{
    imageManager::addContainer("misc");
    imageManager::addContainer("gui");
    imageManager::addImage("gui", "Fond", "img/Fond.png", Vector2i(0, 0), (float)Options.ScreenW/400.f);
    imageManager::addImage("gui", "FondObjet_75_100", "img/FondObjet_75_100.png");
    imageManager::addImage("gui", "FondObjet_75_75", "img/FondObjet_75_75.png");
    imageManager::addImage("gui", "FondObjet_50_50", "img/FondObjet_50_50.png");
    imageManager::addImage("misc", "FondCoffre", "img/FondCoffre.png");
    imageManager::addImage("misc", "FondInventaire", "img/FondInventaire.png");
    imageManager::addImage("misc", "Repos", "img/Repos.png");
    imageManager::addImage("misc", "gui-top-border", "img/gui-top-border.png");

    imageManager::addAnimation("playerLifeGauge", "img/BarreVie.png");
    imageManager::addAnimation("playerLifeGaugeBackground", "img/BarreVie.png");
    imageManager::addAnimation("playerEnergyGauge", "img/BarreEnergie.png");
    imageManager::addAnimation("playerEnergyGaugeBackground", "img/BarreEnergie.png");
    imageManager::addAnimation("playerRecoveryGauge", "img/BarreRecup.png");

    textManager::loadFile("gui", "lng/gui_FR.xml");
    textManager::loadFile("devilsai", "lng/devilsai_FR.xml");
    textManager::loadFile("species", "lng/species_FR.xml");
    textManager::loadFile("places", "lng/places_FR.xml");
    textManager::loadFile("objects", "lng/objects_FR.xml");


    musicManager::addSound("Click");

    musicManager::addMusic("Gates_Of_Heaven");
}

void Supprimer_Decorations()
{
    musicManager::deleteMusics();
}


/** F.P.S. **/

void Disp_FPS(RenderTarget& target)
{
    float Framerate = tools::timeManager::getFPS();
    Text FPS("FPS : " + intToString(Framerate), gui::style::defaultTextFont(), 10);
    FPS.setPosition(target.getSize().x - 100, 16);
    target.draw(FPS);
}


/** MENU SUPÉRIEUR **/

void Disp_Menu(RenderTarget& target)
{
	for (unsigned short a = 0 ; a < target.getSize().x/220 + 1 ; ++a)
    {
		imageManager::display(target, "misc", "gui-top-border", 220*a, 0);
    }

	//Erreur éventuelle
    if (options::option<bool>(tools::math::sdbm_hash("error-occured")))
	{
        Disp_Texte(textManager::getText("devilsai", "ERREUR"), 350, 16, Color(255,0,0,255), 10.);
	}
}

/** CONSOLES DU PERSONNAGE **/

void Disp_JaugesVie(RenderTarget& target)
{
    static int PersoEnePrec = gamedata::player()->currentHealthStatus(Energy);

    static imageManager::Animation* playerLifeGauge = imageManager::getAnimation("playerLifeGauge");
    static imageManager::Animation* playerLifeGaugeBackground = imageManager::getAnimation("playerLifeGaugeBackground");
    static imageManager::Animation* playerEnergyGauge = imageManager::getAnimation("playerEnergyGauge");
    static imageManager::Animation* playerEnergyGaugeBackground = imageManager::getAnimation("playerEnergyGaugeBackground");
    static imageManager::Animation* playerRecoveryGauge = imageManager::getAnimation("playerRecoveryGauge");

    textManager::PlainText playerStateText;

    playerStateText += "@c[128,255,128]";
    playerStateText += *gamedata::player()->displayedName();
    playerStateText += " @d@n[30]"; //Make place for the gauges.

	//1. Jauges de vitalité, d'énergie, de récupération

    playerLifeGauge->setSmoothRectangle(0, 0, gamedata::player()->currentHealthStatus(Life) / 10, 7);
    playerEnergyGauge->setSmoothRectangle(0, 0, gamedata::player()->currentHealthStatus(Energy) / 10, 7);

    if (gamedata::player()->currentHealthStatus(Life) < 50) playerLifeGaugeBackground->setFlickering(0.5);
    else if (gamedata::player()->currentHealthStatus(Life) < 100) playerLifeGaugeBackground->setFlickering(0.25);
    else
    {
        playerLifeGaugeBackground->setFlickering(0);
        playerLifeGaugeBackground->setColor(Color(0, 0, 0, 255));
    }

    if (gamedata::player()->currentHealthStatus(Energy) < 50) playerEnergyGaugeBackground->setFlickering(0.5);
    else if (gamedata::player()->currentHealthStatus(Energy) < 100) playerEnergyGaugeBackground->setFlickering(0.25);
    else
    {
        playerEnergyGaugeBackground->setFlickering(0);
        playerEnergyGaugeBackground->setColor(Color(0, 0, 0, 255));
    }

    playerLifeGaugeBackground->display(target, 42, 70, false);
    playerLifeGauge->display(target, 42, 70, false);
    playerEnergyGaugeBackground->display(target, 42, 79, false);
    playerEnergyGauge->display(target, 42, 79, false);

	int Recup = gamedata::player()->currentHealthStatus(Healing);

	if (Recup > 0)
	{
        playerRecoveryGauge->setRectangle(51, 0, (Recup-(Recup/98)*Recup%98)/2, 7);
        playerRecoveryGauge->display(target, 93, 88, false);
	}
	if (Recup < 0)
	{
        playerRecoveryGauge->setRectangle(51+Recup/2, 0, - Recup/2, 7);
        playerRecoveryGauge->display(target, 92+Recup/2, 88, false);
	}

	//2. État général, fatigue si nécessaire, effet d'une potion
    int l = gamedata::player()->currentHealthStatus(Life) + Recup * 10;

    if (gamedata::player()->currentHealthStatus(Life) == 0)
        playerStateText += textManager::getText("devilsai", "player-health-6");
    else if (l >= 900)
        playerStateText += textManager::getText("devilsai", "player-health-1");
    else if (l >= 650)
        playerStateText += textManager::getText("devilsai", "player-health-2");
    else if (l >= 300)
        playerStateText += textManager::getText("devilsai", "player-health-3");
    else if (l >= 100)
        playerStateText += textManager::getText("devilsai", "player-health-4");
    else
        playerStateText += textManager::getText("devilsai", "player-health-5");

	if (gamedata::player()->currentHealthStatus(Energy) < 140)
	{
        if (PersoEnePrec >= 140) Disp_Information(textManager::getText("devilsai", "FATIGUE"), true);
        playerStateText += " @n";
        playerStateText += textManager::getText("devilsai", "player-health-tired");
	}
	PersoEnePrec = gamedata::player()->currentHealthStatus(Energy);

	//Effets dûs aux objets temporaires
	for (mapObjects::iterator i = gamedata::player()->inventory.objects.begin() ; i != gamedata::player()->inventory.objects.end() ; ++i)
	{
		if (getStringFromLUA(i->second, "getIdEmplacement") == i->first)
		{
			if (getStringFromLUA(i->second, "getCategorieObjet") == "temporaire")
			{
                playerStateText += " @n";
                playerStateText += textManager::getText("devilsai", "SOUS_EFFET");
                playerStateText.addParameter(textManager::getText("objects", getStringFromLUA(i->second, "getInternalNumber")));
			}
		}
	}

    textManager::RichText playerState;
    playerState.setSize(160, 0);
    playerState.setDefaultProperties("liberation", 11, Color(255, 255, 255));
    playerState.addFlags(textManager::HAlignCenter | textManager::OriginXCenter);
    playerState.setSource(&playerStateText);
    playerState.displayFullText(target, 92, 55);

    Disp_Information(textManager::getText("devilsai", "FATIGUE"), false);
}

void Ajouter_LignePerso(String32 ligne, Color couleur)
{
    if (!options::option<bool>(tools::math::sdbm_hash("show-console"))) return;

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

void Disp_Information(const String32& info, bool reactiver)
{
	static float Temps = 0;
	static String32 Information;
	if (reactiver)
	{
		Temps = 250;
		Information = info;
		return;
	}
	if (Temps > 0)
	{
		Disp_TexteCentre(Information, Options.ScreenW/2, Options.ScreenH/2 - 100, Color(255,255,255,Temps), 20., gui::style::fontFromString("dayroman"));
		Temps -= tools::timeManager::I((256-Temps)/24);
	}
}

void Disp_FonduNoir(int Commande, RenderTarget& target)
{
	static float Avancement = 0;

	if (Commande > 0 && Avancement == 0)
	{
		Avancement = 1;
	}
	if (Commande < 0) Avancement = 0;
	if (Avancement > 0 && Avancement <= 255)
	{
        static RectangleShape Fond(Vector2f(target.getSize().x, target.getSize().y));

		Fond.setFillColor(Color(0, 0, 0, Avancement));
        target.draw(Fond);
		Avancement += tools::timeManager::I(1);
		if (Avancement > 255) Avancement = 255;
	}
}

bool Disp_Repos(RenderTarget& target)
{
	static float Animation = 0;
	static float T1 = 255, T2 = 255, T3 = 255, T4 = 255;

    target.clear(Color::Black);

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
		sf::Vertex(sf::Vector2f(0, target.getSize().y), Color(0, 0, 0, T2)),
		sf::Vertex(sf::Vector2f(target.getSize().x, target.getSize().y), Color(0, 0, 0, T3)),
		sf::Vertex(sf::Vector2f(target.getSize().x, 0), Color(0, 0, 0, T4))
	};

    imageManager::display(target, "misc", "Repos", target.getSize().x/2, target.getSize().y/2, true);
    target.draw(Degrade, 4, sf::Quads);
	return false;
}
