
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

namespace imageManager{

/** Ce fichier implémente le gestionnaire d'images du jeu **/

class Base_Image
{
	private:
		Texture img;

	public:
		Sprite spr;

	public:
		short ExSpriteX = 0, ExSpriteY = 0;
		bool TeinteModifiee = false;

	public:
		void Set_Image(string Fichier, int ExX, int ExY, float Scale=1);
		void Set_ImageParArchive(string Fichier, int ExX, int ExY);
		void ModifierTeinte(float m[3][3]);
		const Vector2u getImageDimension();
		void Disp(float x, float y, bool Centre=false);
};


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

typedef map < ClefIndividu, Base_Image >		ClasseIndividus;
typedef map < ClefPaysage, Base_Image >			ClassePaysages;
typedef map < ClefPaysageMouvant, Base_Image >	ClassePaysagesMouvants;
typedef map < ClefProjectile, Base_Image >		ClasseProjectiles;
typedef map < ClefObjet, Base_Image >			ClasseObjets;
typedef map < ClefDecoration, Base_Image >		ClasseDecorations;
typedef map < ClefCompetence, Base_Image >		ClasseCompetences;

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

		Base_Image img;
		const auto& result = ImgIndividus.insert(ClasseIndividus::value_type(ClefIndividu(Type, Act, Dir, n), img));

		string path = intToString(Act) + "/" + intToString(Dir) + "/" + ((n < 10) ? "0" : "") + intToString(n);

		result.first->second.Set_ImageParArchive(path, 0, 0);
		result.first->second.ModifierTeinte(m);

		MESSAGE("Image Individu " + Type + ", " + intToString(Act) + ", " + intToString(Dir) + ", "
				+ intToString(n) + " ajoutée", IMAGE)
	}
}

void AjouterImagePaysage(string Type, int ExX, int ExY)
{
	if (ImgPaysages.find(ClefPaysage(Type)) != ImgPaysages.end()) return;

	Base_Image img;
	const auto& result = ImgPaysages.insert(ClassePaysages::value_type(ClefPaysage(Type), img));

	string path = INSTALL_DIR + "img/";
	stringstream out;
	out.width(5);
	out.fill('0');
	out << Type;
	path += out.str();

	result.first->second.Set_Image(path, ExX, ExY);

	MESSAGE("Image Paysage " + Type + " ajoutée", IMAGE)
}

void AjouterImagesPaysageMouvant(string Type, short Act, short Num, int ExX, int ExY)
{
	for (short n = 0 ; n < Num ; ++n)
	{
		if (ImgPaysagesMouvants.find(ClefPaysageMouvant(Type, Act, n)) != ImgPaysagesMouvants.end()) continue;

		Base_Image img;
		const auto& result = ImgPaysagesMouvants.insert(ClassePaysagesMouvants::value_type(ClefPaysageMouvant(Type, Act, n), img));

		string path = intToString(Act) + "/" + ((n < 10) ? "0" : "") + intToString(n);

		result.first->second.Set_ImageParArchive(path, 0, 0);

		MESSAGE("Image PayMouvant " + Type + ", " + intToString(n) + " ajoutée", IMAGE)
	}
}

void AjouterImageProjectile(string Type, short Dir)
{
	if (ImgProjectiles.find(ClefProjectile(Type, Dir)) != ImgProjectiles.end()) return;

	Base_Image img;
	const auto& result = ImgProjectiles.insert(ClasseProjectiles::value_type(ClefProjectile(Type, Dir), img));

	string path = INSTALL_DIR + "img/" + Type + "0" + intToString(Dir) + "00";

	result.first->second.Set_Image(path, 0, 0);

	MESSAGE("Image Projectile " + Type + ", " + intToString(Dir) + " ajoutée", IMAGE)
}

void AjouterImagesObjet(unsigned Id)
{
	if (ImgObjets.find(ClefObjet(Id, OBJET_IMG)) != ImgObjets.end()) return;
	if (ImgObjets.find(ClefObjet(Id, OBJET_MINIATURE)) != ImgObjets.end()) return;

	Base_Image img;
	const auto& resultImg = ImgObjets.insert(ClasseObjets::value_type(ClefObjet(Id, OBJET_IMG), img));

	Base_Image mini;
	const auto& resultMini = ImgObjets.insert(ClasseObjets::value_type(ClefObjet(Id, OBJET_MINIATURE), mini));

	string path = INSTALL_DIR + "img/O" + intToString(Id/100) + intToString((Id%100)/10) + intToString(Id%10);

	resultImg.first->second.Set_Image(path + "+", 0, 0);
	resultMini.first->second.Set_Image(path + "-", 0, 0);

	MESSAGE("Images Objet " + intToString(Id) + " ajoutées", IMAGE)
}

void AjouterImageDecoration(string Id, int ExX, int ExY, float Scale)
{
	if (ImgDecorations.find(ClefDecoration(Id)) != ImgDecorations.end()) return;

	Base_Image img;
	const auto& result = ImgDecorations.insert(ClasseDecorations::value_type(ClefDecoration(Id), img));

	result.first->second.Set_Image(INSTALL_DIR + "img/" + Id, ExX, ExY, Scale);

	MESSAGE("Image Decoration " + Id + " ajoutée", IMAGE)
}

void AjouterImageCompetence(string Id)
{
	if (ImgCompetences.find(ClefCompetence(Id)) != ImgCompetences.end()) return;

	Base_Image img;
	const auto& result = ImgCompetences.insert(ClasseCompetences::value_type(ClefCompetence(Id), img));

	result.first->second.Set_Image(INSTALL_DIR + "img/C" + Id, 0, 0);

	MESSAGE("Image Compétence " + Id + " ajoutée", IMAGE)
}

/** FONCTIONS D'AFFICHAGE **/

void Disp_ImageIndividu(string Type, short Act, short Dir, short Num, float x, float y, bool Centre)
{
	ClasseIndividus::iterator i = ImgIndividus.find(ClefIndividu(Type, Act, Dir, Num));

	if (i != ImgIndividus.end())
	{
		i->second.Disp(x, y, Centre);

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
		i->second.Disp(x, y, Centre);

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
		i->second.Disp(x, y, Centre);

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
		i->second.Disp(x, y, Centre);

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
		i->second.Disp(x, y, Centre);

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
		i->second.Disp(x, y, Centre);

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

	i->second.spr.setColor(Couleur);
	i->second.spr.setTextureRect(Clip);
	i->second.spr.setScale(ScaleX, ScaleY);
}

void Disp_ImageDecoration(string Id, float x, float y, bool Centre)
{
	ClasseDecorations::iterator i = ImgDecorations.find(ClefDecoration(Id));

	if (i != ImgDecorations.end())
	{
		i->second.Disp(x, y, Centre);

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
		i->second.Disp(x, y, false);

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

/** GESTION DES Base_Image **/

void Base_Image::Set_Image(string Fichier, int ExX, int ExY, float Scale)
{
	Fichier += ".png";
	ExSpriteX = ExX; ExSpriteY = ExY;

	if (!img.loadFromFile(Fichier))
		Erreur("Le fichier suivant n'a pu être chargé :", Fichier.c_str());

	spr.setTexture(img);
	img.setSmooth(false);

	if (Scale != 1) spr.setScale(Scale, Scale);
}

void Base_Image::Set_ImageParArchive(string Fichier, int ExX, int ExY)
{
	Fichier += ".png";
	ExSpriteX = ExX; ExSpriteY = ExY;

	if (PHYSFS_exists(Fichier.c_str()) == 0)
	{
		const char* error = PHYSFS_getLastError();
		if (error != NULL) Erreur("Erreur provenant de PhysicsFS :", error);
	}
	else
	{
		PHYSFS_file* fichier = PHYSFS_openRead(Fichier.c_str());
		char *Buffer = new char[PHYSFS_fileLength(fichier)];
		PHYSFS_read(fichier, Buffer, 1, PHYSFS_fileLength(fichier));

		if (!img.loadFromMemory(Buffer, PHYSFS_fileLength(fichier)))
		{
			Erreur("Le fichier suivant n'a pu être chargé :", Fichier.c_str());
		}
		spr.setTexture(img);
		img.setSmooth(false);

		delete[] Buffer;
		PHYSFS_close(fichier);
	}
}

void Base_Image::ModifierTeinte(float m[3][3])
{
	if (TeinteModifiee) return;

	Color color;
	float tmpR = 0, tmpG = 0, tmpB = 0;
	unsigned int Width = img.getSize().x; unsigned int Height = img.getSize().y;

	Image tmp = img.copyToImage();

	for (unsigned int x = 0 ; x < Width ; ++x)
		for (unsigned int y = 0 ; y < Height ; ++y)
		{
			Color color = tmp.getPixel(x, y);
			if (color.a != 0)
			{
				tmpR = color.r; tmpG = color.g; tmpB = color.b;
				color.r = Minimum(m[0][0]*tmpR + m[0][1]*tmpG + m[0][2]*tmpB, 255);
				color.g = Minimum(m[1][0]*tmpR + m[1][1]*tmpG + m[1][2]*tmpB, 255);
				color.b = Minimum(m[2][0]*tmpR + m[2][1]*tmpG + m[2][2]*tmpB, 255); 
				tmp.setPixel(x, y, color);
			}
		}

	img.update(tmp);

	TeinteModifiee = true;
}

const Vector2u Base_Image::getImageDimension()
{
	return img.getSize();
}

void Base_Image::Disp(float x, float y, bool Centre)
{
	if (!Centre) spr.setPosition(x + ExSpriteX, y + ExSpriteY);
	else spr.setPosition((int)(x - spr.getLocalBounds().width/2) + ExSpriteX, (int)(y - spr.getLocalBounds().height/2) + ExSpriteY);
	Jeu.App.draw(spr);
}

} //namespace imageManager
