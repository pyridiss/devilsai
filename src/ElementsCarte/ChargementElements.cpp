
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

#include "tools/filesystem.h"
#include "tools/textManager.h"
#include "tools/math.h"

#include "imageManager/imageManager.h"

#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/Bibliotheque.h"
#include "../Carte/Carte.h"
#include "ElementsCarte.h"

#include "gamedata.h"

void Load_IndividuUnique(string Type, Individu_Unique *ind)
{
	string fichier = tools::filesystem::dataDirectory() + "individu/" + Type + ".one";

	ind->Nom = tools::textManager::getText("species", Type);

	Activite *act = NULL;

	ifstream Fichier(fichier, ios_base::in);

	if (!Fichier.good()) Erreur("Le fichier suivant n'a pu être chargé :", fichier);
	if (Fichier.good()) MESSAGE(" Fichier \"" + fichier + "\" ouvert", FICHIER)

    string Act;
	short Num;
    double angle;
	int h = 0, s = 0, l = 0;

	string TypeDonnee;

	while (Fichier.rdstate() == 0)
	{
		Fichier >> TypeDonnee;

        if (TypeDonnee == "addImageArchiveFile")
        {
            Fichier >> ind->imagePrefix;
            imageManager::addArchiveFile(ind->imagePrefix);
        }
        if (TypeDonnee == "changeHSL")
            Fichier >> h >> s >> l;
        if (TypeDonnee == "corpseImageKey")
            Fichier >> ind->corpseImageKey;

		if (TypeDonnee == "EQUIPEMENT")
		{
			string numero = "", IdEmplacement = "";
			Fichier >> IdEmplacement >> numero;

			ind->inventory.addObject(numero, IdEmplacement);
		}
		if (TypeDonnee == "INVENTAIRE")
		{
			string TypeDonnee2;
			bool FinInventaire = false;
			int NombreObjets = 1;
			int NumeroObjet = 0;
			while (!FinInventaire)
			{
				Fichier >> TypeDonnee2;

				//Cette partie est une copie du chargement des coffres, fichier Carte.cpp
				if (TypeDonnee2 == "NOMBRE_OBJETS") Fichier >> NombreObjets;
				if (TypeDonnee2 == "NOMBRE_OBJETS_ALEATOIRE")
				{
					int min = 0, max = 0;
					Fichier >> min >> max;
					NombreObjets = min + rand()%(max-min+1);
				}
				if (TypeDonnee2 == "OBJET")
				{
					ifstream::pos_type PosObjet = Fichier.tellg();
					string TypeDonnee3;
					bool FinObjet = false;
					string IdObjet = "";
					int Qualite = 0;
					while (!FinObjet)
					{
						Fichier >> TypeDonnee3;

						if (TypeDonnee3 == "TYPE")
						{
							Fichier >> IdObjet >> Qualite;
						}
						if (TypeDonnee3 == "TYPE_ALEATOIRE")
						{
							if ((IdObjet == "" && Qualite == 0) || rand()%2)
								Fichier >> IdObjet >> Qualite;
						}
						if (TypeDonnee3 == "FIN_OBJET")
						{
							--NombreObjets;
							ind->inventory.addObject(IdObjet, intToString(CLEF_INVENTAIRE + NumeroObjet), Qualite);
							++NumeroObjet;
							if (NombreObjets <= 0) FinObjet = true;
							else Fichier.seekg(PosObjet);
						}
					}
				}
				if (TypeDonnee2 == "FIN_INVENTAIRE") FinInventaire = true;
				TypeDonnee2 = "";
			}
		}
		if (TypeDonnee == "DIPLOM")		Fichier >> ind->Diplomatie;
		if (TypeDonnee == "CMP_VIS")
        {
            int a;
            Fichier >> a;
            ind->viewField.circle(tools::math::Vector2d(0, 0), a);
        }
		if (TypeDonnee == "RAY_COL")
        {
            int a;
            Fichier >> a;
            ind->size.circle(tools::math::Vector2d(0, 0), a);
        }
		if (TypeDonnee == "RAY_ATT" || TypeDonnee == "RAY_INTER")
        {
            int a;
            Fichier >> a;
            ind->interactionField.circle(tools::math::Vector2d(0, 0), a);
        }
		if (TypeDonnee == "ACT_DEFAUT")
		{
			Fichier >> ind->ActDefaut;
			ind->Set_Activite(ind->ActDefaut);
		}
		if (TypeDonnee == "ACT")
		{
			Fichier >> Act;
			ind->Ajouter_Activite(Act);
			act = ind->Get_Activite(Act);
		}
		if (TypeDonnee == "blo")	Fichier >> act->priority;

		if (TypeDonnee == "VIT")	Fichier >> act->speed;
		if (TypeDonnee == "MAJ")	Fichier >> act->step;
		if (TypeDonnee == "angle")
		{
			Fichier >> angle;
		}

        if (TypeDonnee == "numberOfImages")
		{
			Fichier >> Num;
			act->numberOfImages = Num;
		}
        if (TypeDonnee == "pathToImages")
        {
            string pathPattern;
            Fichier >> pathPattern;
            for (int i = 0 ; i < Num ; ++i)
            {
                string path = pathPattern;
                string number = intToString(i, 2);
                path.replace(path.find_first_of('%'), 2, number);
                string key = Type + ":" + path;
                imageManager::addImage("individuals", key, path);
                if (h + s + l != 0)
                    imageManager::changeHSL("individuals", key, h, s, l);

                act->addImage(angle * M_PI / 180.0, i, key);
            }
        }

		if (TypeDonnee == "FIN_ACT")
		{
			Act = ""; Num = 0;
            angle = 0;
			act = NULL;
		}

		if (TypeDonnee == "FORCE")		Fichier >> ind->Caracs["strength"];
		if (TypeDonnee == "PUISS")		Fichier >> ind->Caracs["power"];
		if (TypeDonnee == "AGILITE")	Fichier >> ind->Caracs["agility"];
		if (TypeDonnee == "INTELLI")	Fichier >> ind->Caracs["intellect"];
		if (TypeDonnee == "CONSTIT")	Fichier >> ind->Caracs["constitution"];
		if (TypeDonnee == "ESQUIVE")	Fichier >> ind->Caracs["dodge"];
		if (TypeDonnee == "CHARISM")	Fichier >> ind->Caracs["charisma"];
		if (TypeDonnee == "RECUP")
		{
			Fichier >> ind->Caracs["recovery"];
			ind->Set_Recuperation(ind->Caracs["recovery"]);
		}
		if (TypeDonnee == "EXP")		Fichier >> ind->Experience;
		if (TypeDonnee == "RECUP_FIXE")		ind->RecuperationFixe = true;
		if (TypeDonnee == "ENERGIE_MAX")	ind->EnergieMax = true;

		TypeDonnee = "";
	}

	imageManager::removeArchiveFile(ind->imagePrefix);

	Fichier.close();
}
