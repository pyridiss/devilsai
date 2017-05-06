
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

#include <physfs.h>

#include "Jeu/Jeu.h"
#include "Bibliotheque/Bibliotheque.h"
#include "Bibliotheque/Constantes.h"

#include "imageManager/imageManager.h"
#include "imageManager/image.h"
#include "imageManager/animation.h"

namespace imageManager{

/** CLASSES POUR LES CLEFS DES CONTENEURS **/

struct ClefIndividu
{
	string Type;
	short Act;
	short Dir;
	short Num;

	ClefIndividu(string t, short a, short d, short n)
	{
		Type = t;
		Act = a;
		Dir = d;
		Num = n;
	}
	bool operator<(const ClefIndividu& right) const
	{
		if (Type == right.Type) return (Act*1000 + Dir*100 + Num < right.Act*1000 + right.Dir*100 + right.Num);
		else return (Type < right.Type);
	}
};


/** DÉFINITION DES CONTENEURS **/

typedef map < ClefIndividu, imageManager::Image >		ClasseIndividus;

ClasseIndividus			ImgIndividus;


typedef map < string, imageManager::Image > Container;
typedef map < string, Container > Database;
typedef map < string, imageManager::Animation > AnimationDatabase;

Database images;
AnimationDatabase animations;

void addContainer(string container)
{
    Database::iterator c = images.find(container);

    if (c == images.end())
    {
        Container ctnr;
        images.insert(Database::value_type(container, ctnr));
    }
}

void addImage(string container, string key, string file, Vector2i of, float scale)
{
    Database::iterator c = images.find(container);

    if (c == images.end())
    {
        Erreur("Contained has not been created yet:", container);
        return;
    }

    Container::iterator i = (*c).second.find(key);

    if (i == (*c).second.end())
    {
        imageManager::Image img;
        const auto& result = (*c).second.insert(Container::value_type(key, img));
        result.first->second.set(file, of, scale);
    }
}

void addArchiveFile(string path)
{
    if (PHYSFS_addToSearchPath(path.c_str(), 1) == 0)
    {
        Erreur("Unable to load file:", path);
    }
    else MESSAGE("File \"" + path + "\" open", FICHIER)
}

void removeArchiveFile(string path)
{
    PHYSFS_removeFromSearchPath(path.c_str());
}

imageManager::Image* getImage(string container, string key)
{
    Database::iterator c = images.find(container);

    if (c == images.end())
    {
        Erreur("Container not found:", container);
        return nullptr;
    }

    Container::iterator i = (*c).second.find(key);

    if (i == (*c).second.end())
    {
        Erreur("Image not found:", container + "::" + key);
        return nullptr;
    }

    return &(i->second);
}

void display(RenderWindow& app, string container, string key, float x, float y, bool atCenter)
{
    Database::iterator c = images.find(container);

    if (c == images.end())
    {
        Erreur("Image container has not been added yet:", container);
        return;
    }

    Container::iterator i = (*c).second.find(key);

    (*i).second.display(app, x, y, atCenter);
}

void addAnimation(string name, string file)
{
    AnimationDatabase::iterator a = animations.find(name);

    if (a == animations.end())
    {
        imageManager::Animation ani;
        const auto& result = animations.insert(AnimationDatabase::value_type(name, ani));
        result.first->second.image.set(file, Vector2i(0, 0));
    }
}

imageManager::Animation* getAnimation(string name)
{
    AnimationDatabase::iterator a = animations.find(name);

    if (a == animations.end())
    {
        Erreur("Animation not found:", name);
        return nullptr;
    }

    return &(a->second);
}

/** FONCTIONS D'AJOUT **/

void AjouterImagesIndividu(string Type, short Act, short Dir, short Num, float m[3][3])
{
	for (short n = 0 ; n < Num ; ++n)
	{
		if (ImgIndividus.find(ClefIndividu(Type, Act, Dir, n)) != ImgIndividus.end()) continue;

		imageManager::Image img;
		const auto& result = ImgIndividus.insert(ClasseIndividus::value_type(ClefIndividu(Type, Act, Dir, n), img));

		string path = intToString(Act) + "/" + intToString(Dir) + "/" + ((n < 10) ? "0" : "") + intToString(n);

		result.first->second.setFromArchive(path, Vector2i(0, 0));
//TODO		result.first->second.ModifierTeinte(m);

		MESSAGE("Image Individu " + Type + ", " + intToString(Act) + ", " + intToString(Dir) + ", "
				+ intToString(n) + " ajoutée", IMAGE)
	}
}


/** FONCTIONS D'AFFICHAGE **/

void Disp_ImageIndividu(string Type, short Act, short Dir, short Num, float x, float y, bool Centre)
{
	ClasseIndividus::iterator i = ImgIndividus.find(ClefIndividu(Type, Act, Dir, Num));

	if (i != ImgIndividus.end())
	{
		i->second.display(Jeu.App, x, y, Centre);

		MESSAGE("Image Individu demandée : [" + Type + ", " + intToString(Act) + ", " + intToString(Dir) + ", "
				+ intToString(Num) + "] … OK", IMAGE)
	}
	else
	{
		string StrId = Type + ", " + intToString(Act) + ", " + intToString(Dir) + ", " + intToString(Num);
		Erreur("L'image Individu suivante a été demandée sans avoir été chargée :", StrId);
		MESSAGE("Image Individu demandée : [" + StrId + "] … INEXISTANTE", IMAGE)
	}
}

void Disp_ImageCadavre(string Type, short Dir, short Num, float x, float y, bool Centre)
{
	ClasseIndividus::iterator i = ImgIndividus.find(ClefIndividu(Type, MORT, Dir, Num));

	if (i != ImgIndividus.end())
	{
		i->second.display(Jeu.App, x, y, Centre);

		MESSAGE("Image Cadavre demandée : [" + Type + "] … OK", IMAGE)
	}
	else
	{
		Erreur("Le cadavre de l'individu suivant n'a pu être affiché :", Type);
		MESSAGE("Image Cadavre demandée : [" + Type + "] … INEXISTANTE", IMAGE)
	}
}

void Set_ImageDecoration(string Id, Color Couleur, IntRect Clip, float ScaleX, float ScaleY)
{
	ClasseDecorations::iterator i = ImgDecorations.find(ClefDecoration(Id));

	i->second.sprite.setColor(Couleur);
	i->second.sprite.setTextureRect(Clip);
	i->second.sprite.setScale(ScaleX, ScaleY);
}


void Supprimer_Images()
{
	ImgIndividus.clear();
	ImgPaysages.clear();

	MESSAGE("Toutes les images ont été suppprimées", IMAGE)
}

} //namespace imageManager
