
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
#include "musicManager/musicManager.h"
#include "textManager/richText.h"

#include "devilsai-resources/Carte.h"

#include "devilsai-gui/console.h"

#include "Jeu/options.h"


/** GESTION DES DÉCORATIONS **/

void Load_Decorations()
{
    imageManager::addContainer(tools::hash("misc"));
    imageManager::addImage(tools::hash("misc"), "Repos", "img/Repos.png");

    textManager::loadFile("gui", "lng/gui.xml", options::option<string>(tools::hash("language")));
    textManager::loadFile("devilsai", "lng/devilsai.xml", options::option<string>(tools::hash("language")));
    textManager::loadFile("species", "lng/species.xml", options::option<string>(tools::hash("language")));
    textManager::loadFile("places", "lng/places.xml", options::option<string>(tools::hash("language")));
    textManager::loadFile("objects", "lng/objects.xml", options::option<string>(tools::hash("language")));


    musicManager::addSound("Click");

    musicManager::addMusic("Gates_Of_Heaven");
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
