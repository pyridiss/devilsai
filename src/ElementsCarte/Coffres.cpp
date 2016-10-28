
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

#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/Templates.h"
#include "../Carte/Carte.h"
#include "ElementsCarte.h"


#include <physfs.h>

/** FONCTIONS DE LA CLASSE Coffre **/

int Coffre::Gestion()
{
	return ETAT_NORMAL;
}

int Coffre::Collision(Individu* elem, int TypeCollision)
{
	if (TypeCollision == COLL_VIS) return COMPORTEMENT_ALEATOIRE;
	return COLL_INTER;
}

void Coffre::Disp(float RefX, float RefY)
{
	return;
}

void Coffre::Disp_Masks(float RefX, float RefY)
{
	return;
}

/** FONCTIONS DE LA CLASSE Cadavre **/

Cadavre::Cadavre() : Coffre()
{
	TypeClassement = CLASSEMENT_CADAVRE;
}

int Cadavre::Gestion()
{
	if (objects.objects.empty()) Vide = true;
	if (Vide && Duree > 5) Duree = 5;

	if (Vide && !objects.objects.empty()) //Un objet a été reposé alors que le cadavre venait d'être vidé…
	{
		Vide = false;
		Duree += 100;
	}

	if (Duree <= 0)
		return ETAT_MORT;
	else
	{
		Duree -= I(1/60.);
		return ETAT_NORMAL;
	}
}

void Cadavre::Set_Individu()
{
	Nom = getFormatedTranslatedMessage(_CADAVRE, getTranslatedNameOfElement(Ind_Id));

	ifstream Fichier(FichierIndividu, ios_base::in);
	if (!Fichier.good()) Erreur("Le fichier suivant n'a pu être chargé :", FichierIndividu);

	float Matrice[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
	string TypeDonnee;
	string Archive = INSTALL_DIR + "img/";

	while (Fichier.rdstate() == 0)
	{
		Fichier >> TypeDonnee;

		if (TypeDonnee == "ARCHIVE_IMG")
		{
			string buffer;	Fichier >> buffer;	Archive += buffer;
			if (PHYSFS_addToSearchPath(Archive.c_str(), 1) == 0)
				Erreur("Le fichier suivant n'a pu être chargé :", Archive);
		}
		if (TypeDonnee == "MATRICE_TEINTE")
		{
			Fichier >> Matrice[0][0] >> Matrice[0][1] >> Matrice[0][2]
					>> Matrice[1][0] >> Matrice[1][1] >> Matrice[1][2]
					>> Matrice[2][0] >> Matrice[2][1] >> Matrice[2][2];
		}
		TypeDonnee = "";
	}

	AjouterImagesIndividu(Ind_Id, MORT, Ind_Dir, Ind_Num, Matrice);

	if (Archive != INSTALL_DIR + "img/") PHYSFS_removeFromSearchPath(Archive.c_str());
	Fichier.close();
}

void Cadavre::Set_Individu(string type, short dir, short num, bool Unique)
{
	Ind_Id = type;
	Ind_Dir = dir;
	Ind_Num = num;

	FichierIndividu = INSTALL_DIR + "individu/" + type;
	if (Unique) FichierIndividu += ".one";
	else FichierIndividu += ".com";

	Set_Individu();
}

void Cadavre::Disp(float RefX, float RefY)
{
	if (Controle == AI_IMG_HORSCHAMP) return;

	Disp_ImageCadavre(Ind_Id, Ind_Dir, Ind_Num - 1, Options.ScreenW/2 - (RefX - PosX), Options.ScreenH/2 + 12 - (RefY - PosY), true);
}

