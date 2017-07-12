
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

#include <map>

#include "tools/filesystem.h"

#include "../Bibliotheque/Bibliotheque.h"
#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/Templates.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "Jeu.h"


struct Sauvegarde
{
	string Dossier = "";
	String32 Nom;
	string Version = "";
};


typedef map<string, Sauvegarde> DicoSauvegardes;

DicoSauvegardes Sauvegardes;

void SupprimerSauvegarde(DicoSauvegardes::iterator save)
{
	//Ouverture du fichier principal pour déterminer les fichiers présents dans le dossier
	string path = tools::filesystem::getSaveDirectoryPath() + save->second.Dossier + "/";
	string strFichierPrincipal = path + "save.sav";

	ifstream FichierPrincipal(strFichierPrincipal.c_str(), ios_base::in);
	if (!FichierPrincipal.good()) Erreur("Le fichier suivant n'a pu être chargé :", strFichierPrincipal.c_str());

	if (FichierPrincipal.good()) MESSAGE(" Fichier \"" + strFichierPrincipal + "\" ouvert", FICHIER)

	string TypeDonnee;
	string DonneeStr;

	while(FichierPrincipal.rdstate() == 0)
	{
		FichierPrincipal >> TypeDonnee;
		if (TypeDonnee == "CARTE")
		{
			FichierPrincipal >> DonneeStr;
            tools::filesystem::removeFile(path + DonneeStr + ".map");
		}
		TypeDonnee = "";
	}

	//Fermeture du fichier principal pour suppression
	FichierPrincipal.close();
    tools::filesystem::removeFile(path + "save.sav");

	//Suppression du dossier
    tools::filesystem::removeDirectory(tools::filesystem::getSaveDirectoryPath() + save->second.Dossier);

	//Suppression de la sauvegarde
	Sauvegardes.erase(save);
}

void LectureSauvegardes()
{
	string fichier = tools::filesystem::getSaveDirectoryPath() + "sauvegardes.opt";

	ifstream Fichier(fichier.c_str(), ios_base::in);

	string TypeDonnee;
	int NombreSauvegardes = 0;

	DicoSauvegardes::iterator i;

	//Ce fichier peut ne pas exister
	if (Fichier.good())
	{
		if (Fichier.good()) MESSAGE(" Fichier \"" + fichier + "\" ouvert", FICHIER)

		Fichier >> TypeDonnee;
		if (TypeDonnee == "NOMBRE_SAUVEGARDES") Fichier >> NombreSauvegardes;
		else Erreur("Fichier de sauvegarde corrompu :", fichier);

		while (Fichier.rdstate() == 0)
		{
			Fichier >> TypeDonnee;
			if (TypeDonnee == "SAUVEGARDE")
			{
				Sauvegarde s;
				string d;
				Fichier >> d;
				Sauvegardes.insert(DicoSauvegardes::value_type(d, s));
				i = Sauvegardes.find(d);
				i->second.Dossier = d;
			}
			if (i == Sauvegardes.end())
			{
				Erreur("Fichier de sauvegarde corrompu :", fichier);
				Fichier.close();
				return;
			}
			if (TypeDonnee == "NOM")		Fichier >> i->second.Nom;
			if (TypeDonnee == "VERSION")	Fichier >> i->second.Version;
			if (TypeDonnee == "FIN_SAUVEGARDE");

			TypeDonnee = "";
		}

		Fichier.close();
	}
}
