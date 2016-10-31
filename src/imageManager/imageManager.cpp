
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

struct ClefPaysage
{
	string Type;

	ClefPaysage(string t)
	{
		Type = t;
	}
	bool operator<(const ClefPaysage& right) const
	{
		return (Type < right.Type);
	}
};

struct ClefPaysageMouvant
{
	string Type;
	short Act;
	short Num;

	ClefPaysageMouvant(string t, short a, short n)
	{
		Type = t;
		Act = a;
		Num = n;
	}
	bool operator<(const ClefPaysageMouvant& right) const
	{
		if (Type == right.Type) return (Act*100 + Num < right.Act*100 + right.Num);
		else return (Type < right.Type);
	}
};

struct ClefProjectile
{
	string Type;
	short Dir;

	ClefProjectile(string t, short d)
	{
		Type = t;
		Dir = d;
	}
	bool operator<(const ClefProjectile& right) const
	{
		if (Type == right.Type) return (Dir < right.Dir);
		else return (Type < right.Type);
	}
};

struct ClefObjet
{
	unsigned Id;
	short Taille;

	ClefObjet(unsigned i, short t)
	{
		Id = i;
		Taille = t;
	}
	bool operator<(const ClefObjet& right) const
	{
		return (Id*10 + Taille < right.Id*10 + right.Taille);
	}
};

struct ClefDecoration
{
	string Id;

	ClefDecoration(string i)
	{
		Id = i;
	}
	bool operator<(const ClefDecoration& right) const
	{
		return (Id < right.Id);
	}
};

struct ClefCompetence
{
	string Id;

	ClefCompetence(string i)
	{
		Id = i;
	}
	bool operator<(const ClefCompetence& right) const
	{
		return (Id < right.Id);
	}
};

/** DÉFINITION DES CONTENEURS **/

typedef map < ClefIndividu, imageManager::Image >		ClasseIndividus;
typedef map < ClefPaysage, imageManager::Image >			ClassePaysages;
typedef map < ClefPaysageMouvant, imageManager::Image >	ClassePaysagesMouvants;
typedef map < ClefProjectile, imageManager::Image >		ClasseProjectiles;
typedef map < ClefObjet, imageManager::Image >			ClasseObjets;
typedef map < ClefDecoration, imageManager::Image >		ClasseDecorations;
typedef map < ClefCompetence, imageManager::Image >		ClasseCompetences;

ClasseIndividus			ImgIndividus;
ClassePaysages			ImgPaysages;
ClassePaysagesMouvants	ImgPaysagesMouvants;
ClasseProjectiles		ImgProjectiles;
ClasseObjets			ImgObjets;
ClasseDecorations		ImgDecorations;
ClasseCompetences		ImgCompetences;


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

void AjouterImagePaysage(string Type, int ExX, int ExY)
{
	if (ImgPaysages.find(ClefPaysage(Type)) != ImgPaysages.end()) return;

	imageManager::Image img;
	const auto& result = ImgPaysages.insert(ClassePaysages::value_type(ClefPaysage(Type), img));

	string path = INSTALL_DIR + "img/";
	stringstream out;
	out.width(5);
	out.fill('0');
	out << Type;
	path += out.str();

	result.first->second.set(path, Vector2i(ExX, ExY));

	MESSAGE("Image Paysage " + Type + " ajoutée", IMAGE)
}

void AjouterImagesPaysageMouvant(string Type, short Act, short Num, int ExX, int ExY)
{
	for (short n = 0 ; n < Num ; ++n)
	{
		if (ImgPaysagesMouvants.find(ClefPaysageMouvant(Type, Act, n)) != ImgPaysagesMouvants.end()) continue;

		imageManager::Image img;
		const auto& result = ImgPaysagesMouvants.insert(ClassePaysagesMouvants::value_type(ClefPaysageMouvant(Type, Act, n), img));

		string path = intToString(Act) + "/" + ((n < 10) ? "0" : "") + intToString(n);

		result.first->second.setFromArchive(path, Vector2i(0, 0));

		MESSAGE("Image PayMouvant " + Type + ", " + intToString(n) + " ajoutée", IMAGE)
	}
}

void AjouterImageProjectile(string Type, short Dir)
{
	if (ImgProjectiles.find(ClefProjectile(Type, Dir)) != ImgProjectiles.end()) return;

	imageManager::Image img;
	const auto& result = ImgProjectiles.insert(ClasseProjectiles::value_type(ClefProjectile(Type, Dir), img));

	string path = INSTALL_DIR + "img/" + Type + "0" + intToString(Dir) + "00";

	result.first->second.set(path, Vector2i(0, 0));

	MESSAGE("Image Projectile " + Type + ", " + intToString(Dir) + " ajoutée", IMAGE)
}

void AjouterImagesObjet(unsigned Id)
{
	if (ImgObjets.find(ClefObjet(Id, OBJET_IMG)) != ImgObjets.end()) return;
	if (ImgObjets.find(ClefObjet(Id, OBJET_MINIATURE)) != ImgObjets.end()) return;

	imageManager::Image img;
	const auto& resultImg = ImgObjets.insert(ClasseObjets::value_type(ClefObjet(Id, OBJET_IMG), img));

	imageManager::Image mini;
	const auto& resultMini = ImgObjets.insert(ClasseObjets::value_type(ClefObjet(Id, OBJET_MINIATURE), mini));

	string path = INSTALL_DIR + "img/O" + intToString(Id/100) + intToString((Id%100)/10) + intToString(Id%10);

	resultImg.first->second.set(path + "+", Vector2i(0, 0));
	resultMini.first->second.set(path + "-", Vector2i(0, 0));

	MESSAGE("Images Objet " + intToString(Id) + " ajoutées", IMAGE)
}

void AjouterImageDecoration(string Id, int ExX, int ExY, float Scale)
{
	if (ImgDecorations.find(ClefDecoration(Id)) != ImgDecorations.end()) return;

	imageManager::Image img;
	const auto& result = ImgDecorations.insert(ClasseDecorations::value_type(ClefDecoration(Id), img));

	result.first->second.set(INSTALL_DIR + "img/" + Id, Vector2i(ExX, ExY), Scale);

	MESSAGE("Image Decoration " + Id + " ajoutée", IMAGE)
}

void AjouterImageCompetence(string Id)
{
	if (ImgCompetences.find(ClefCompetence(Id)) != ImgCompetences.end()) return;

	imageManager::Image img;
	const auto& result = ImgCompetences.insert(ClasseCompetences::value_type(ClefCompetence(Id), img));

	result.first->second.set(INSTALL_DIR + "img/C" + Id, Vector2i(0, 0));

	MESSAGE("Image Compétence " + Id + " ajoutée", IMAGE)
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

Vector2u getImagePaysageDimension(string Type)
{
	ClassePaysages::iterator i = ImgPaysages.find(ClefPaysage(Type));

	if (i != ImgPaysages.end())
		return i->second.getImageDimension();

	return Vector2u(0, 0);
}

void Disp_ImagePaysage(string Type, float x, float y, bool Centre)
{
	ClassePaysages::iterator i = ImgPaysages.find(ClefPaysage(Type));

	if (i != ImgPaysages.end())
	{
		i->second.display(Jeu.App, x, y, Centre);

		MESSAGE("Image Paysage demandée : " + Type + " … OK", IMAGE)
	}
	else
	{
		Erreur("L'image Paysage suivante a été demandée sans avoir été chargée :", Type);
		MESSAGE("Image Paysage demandée : " + Type + " … INEXISTANTE", IMAGE)
	}
}

void Disp_ImagePaysageMouvant(string Type, short Act, short Num, float x, float y, bool Centre)
{
	ClassePaysagesMouvants::iterator i = ImgPaysagesMouvants.find(ClefPaysageMouvant(Type, Act, Num));

	if (i != ImgPaysagesMouvants.end())
	{
		i->second.display(Jeu.App, x, y, Centre);

		MESSAGE("Image PayMouvant demandée : [" + Type + ", " + intToString(Act) + ", " + intToString(Num) + "] … OK", IMAGE)
	}
	else
	{
		string StrId = Type + ", " + intToString(Act) + ", " + intToString(Num);
		Erreur("L'image PayMouvant suivante a été demandée sans avoir été chargée :", StrId);
		MESSAGE("Image PayMouvant demandée : [" + StrId + "] … OK", IMAGE)
	}
}

void Disp_ImageProjectile(string Type, short Dir, float x, float y, bool Centre)
{
	ClasseProjectiles::iterator i = ImgProjectiles.find(ClefProjectile(Type, Dir));

	if (i != ImgProjectiles.end())
	{
		i->second.display(Jeu.App, x, y, Centre);

		MESSAGE("Image Projectile demandée : [" + Type + ", " + intToString(Dir) + "] … OK", IMAGE)
	}
	else
	{
		string StrId = Type + ", " + intToString(Dir);
		Erreur("L'image Projectile suivante a été demandée sans avoir été chargée :", StrId);
		MESSAGE("Image Projectile demandée : [" + StrId + "] … OK", IMAGE)
	}
}

void Disp_ImageObjet(unsigned Id, short Taille, float x, float y, bool Centre)
{
	ClasseObjets::iterator i = ImgObjets.find(ClefObjet(Id, Taille));

	if (i != ImgObjets.end())
	{
		i->second.display(Jeu.App, x, y, Centre);

		MESSAGE("Image Objet demandée : [" + intToString(Id) + ", " + intToString(Taille) + "] … OK", IMAGE)
	}
	else
	{
		string StrId = intToString(Id) + ", " + intToString(Taille);
		Erreur("L'image Objet suivante a été demandée sans avoir été chargée :", StrId);
		MESSAGE("Image Objet demandée : [" + StrId + "] … INEXISTANTE", IMAGE)
	}
}

void Set_ImageDecoration(string Id, Color Couleur, IntRect Clip, float ScaleX, float ScaleY)
{
	ClasseDecorations::iterator i = ImgDecorations.find(ClefDecoration(Id));

	i->second.sprite.setColor(Couleur);
	i->second.sprite.setTextureRect(Clip);
	i->second.sprite.setScale(ScaleX, ScaleY);
}

void Disp_ImageDecoration(string Id, float x, float y, bool Centre)
{
	ClasseDecorations::iterator i = ImgDecorations.find(ClefDecoration(Id));

	if (i != ImgDecorations.end())
	{
		i->second.display(Jeu.App, x, y, Centre);

		MESSAGE("Image Décoration demandée : " + Id + " … OK", IMAGE)
	}
	else
	{
		Erreur("L'image Décoration suivante a été demandée sans avoir été chargée :", Id);
		MESSAGE("Image Décoration demandée : " + Id + " … INEXISTANTE", IMAGE)
	}
}

void Disp_ImageCompetence(string Id, float x, float y)
{
	ClasseCompetences::iterator i = ImgCompetences.find(ClefCompetence(Id));

	if (i != ImgCompetences.end())
	{
		i->second.display(Jeu.App, x, y, false);

		MESSAGE("Image Compétence demandée : " + Id + " … OK", IMAGE)
	}
	else
	{
		Erreur("L'image Compétence suivante a été demandée sans avoir été chargée :", Id);
		MESSAGE("Image Compétence demandée : " + Id + " … INEXISTANTE", IMAGE)
	}
}

void Supprimer_Images()
{
	ImgIndividus.clear();
	ImgPaysages.clear();
	ImgPaysagesMouvants.clear();
	ImgProjectiles.clear();
	ImgObjets.clear();
	ImgDecorations.clear();
	ImgCompetences.clear();

	MESSAGE("Toutes les images ont été suppprimées", IMAGE)
}

} //namespace imageManager
