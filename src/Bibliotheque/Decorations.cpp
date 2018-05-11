
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

#include "../ElementsCarte/ElementsCarte.h"
#include "Bibliotheque.h"

#include "textManager/textManager.h"
#include "tools/math.h"
#include "tools/timeManager.h"
#include "imageManager/imageManager.h"
#include "imageManager/animation.h"
#include "musicManager/musicManager.h"
#include "textManager/richText.h"

#include "devilsai-resources/Carte.h"

#include "devilsai-gui/console.h"

#include "Jeu/options.h"


/** GESTION DES DÉCORATIONS **/

void Load_Decorations()
{
    imageManager::addContainer(tools::hash("misc"));
    imageManager::addContainer(tools::hash("gui"));
    imageManager::addImage(tools::hash("misc"), "Repos", "img/Repos.png");

    imageManager::addAnimation("playerLifeGauge", "img/BarreVie.png");
    imageManager::addAnimation("playerLifeGaugeBackground", "img/BarreVie.png");
    imageManager::addAnimation("playerEnergyGauge", "img/BarreEnergie.png");
    imageManager::addAnimation("playerEnergyGaugeBackground", "img/BarreEnergie.png");
    imageManager::addAnimation("playerRecoveryGauge", "img/BarreRecup.png");

    textManager::loadFile("gui", "lng/gui.xml", options::option<string>(tools::hash("language")));
    textManager::loadFile("devilsai", "lng/devilsai.xml", options::option<string>(tools::hash("language")));
    textManager::loadFile("species", "lng/species.xml", options::option<string>(tools::hash("language")));
    textManager::loadFile("places", "lng/places.xml", options::option<string>(tools::hash("language")));
    textManager::loadFile("objects", "lng/objects.xml", options::option<string>(tools::hash("language")));


    musicManager::addSound("Click");

    musicManager::addMusic("Gates_Of_Heaven");
}

void Supprimer_Decorations()
{
    musicManager::deleteMusics();
}


/** CONSOLES DU PERSONNAGE **/

void Joueur::Disp_JaugesVie(RenderTarget& target)
{
    static int PersoEnePrec = currentHealthStatus(Energy);

    static imageManager::Animation* playerLifeGauge = imageManager::getAnimation("playerLifeGauge");
    static imageManager::Animation* playerLifeGaugeBackground = imageManager::getAnimation("playerLifeGaugeBackground");
    static imageManager::Animation* playerEnergyGauge = imageManager::getAnimation("playerEnergyGauge");
    static imageManager::Animation* playerEnergyGaugeBackground = imageManager::getAnimation("playerEnergyGaugeBackground");
    static imageManager::Animation* playerRecoveryGauge = imageManager::getAnimation("playerRecoveryGauge");

    textManager::PlainText playerStateText;

    playerStateText += "@c[128,255,128]";
    playerStateText += displayedName();
    playerStateText += " @d@n[30]"; //Make place for the gauges.

	//1. Jauges de vitalité, d'énergie, de récupération

    playerLifeGauge->setSmoothRectangle(0, 0, currentHealthStatus(Life) / 10, 7);
    playerEnergyGauge->setSmoothRectangle(0, 0, currentHealthStatus(Energy) / 10, 7);

    if (currentHealthStatus(Life) < 50) playerLifeGaugeBackground->setFlickering(0.5);
    else if (currentHealthStatus(Life) < 100) playerLifeGaugeBackground->setFlickering(0.25);
    else
    {
        playerLifeGaugeBackground->setFlickering(0);
        playerLifeGaugeBackground->setColor(Color(0, 0, 0, 255));
    }

    if (currentHealthStatus(Energy) < 50) playerEnergyGaugeBackground->setFlickering(0.5);
    else if (currentHealthStatus(Energy) < 100) playerEnergyGaugeBackground->setFlickering(0.25);
    else
    {
        playerEnergyGaugeBackground->setFlickering(0);
        playerEnergyGaugeBackground->setColor(Color(0, 0, 0, 255));
    }

    playerLifeGaugeBackground->display(target, 42, 70, false);
    playerLifeGauge->display(target, 42, 70, false);
    playerEnergyGaugeBackground->display(target, 42, 79, false);
    playerEnergyGauge->display(target, 42, 79, false);

    int Recup = currentHealthStatus(Healing);

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
    int l = currentHealthStatus(Life) + Recup * 10;

    if (currentHealthStatus(Life) == 0)
        playerStateText += textManager::getText("devilsai", "player-health-dead");
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

    if (currentHealthStatus(Energy) < 140)
	{
        if (PersoEnePrec >= 140) addConsoleEntry(textManager::getText("devilsai", "FATIGUE"));
        playerStateText += " @n";
        playerStateText += textManager::getText("devilsai", "player-health-tired");
	}
    PersoEnePrec = currentHealthStatus(Energy);

	//Effets dûs aux objets temporaires
    for (auto& i : inventory.objects)
    {
        if (i.active() && i.temporary())
        {
            playerStateText += " @n";
            playerStateText += textManager::getText("devilsai", "SOUS_EFFET");
            playerStateText.addParameter(textManager::getText("objects", i.name()));
        }
    }

    textManager::RichText playerState;
    playerState.setSize(160, 0);
    playerState.setDefaultProperties("liberation", 11, Color(255, 255, 255));
    playerState.addFlags(textManager::HAlignCenter | textManager::OriginXCenter);
    playerState.create(playerStateText);
    playerState.displayFullText(target, 92, 55);
}

/** SITUATIONS PARTICULIÈRES **/

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

    imageManager::display(target, tools::hash("misc"), "Repos", target.getSize().x/2, target.getSize().y/2, true);
    target.draw(Degrade, 4, sf::Quads);
	return false;
}
