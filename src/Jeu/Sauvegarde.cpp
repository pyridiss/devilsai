
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

#include <typeinfo>

#include <tinyxml2.h>

#include "tools/filesystem.h"
#include "musicManager/musicManager.h"

#include "../Bibliotheque/Bibliotheque.h"
#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/Templates.h"
#include "Jeu.h"
#include "../Carte/Carte.h"
#include "../ElementsCarte/ElementsCarte.h"

#include "gamedata.h"
#include "options.h"

using namespace tinyxml2;

void Save_Partie()
{
	/* 2. FICHIERS DE CARTE */

	//Création d'éléments pour récupérer le type et le comparer aux Elements de la liste
	Individu_Unique	ExempleUnique;			const type_info &TypeUnique = typeid(ExempleUnique);
	Joueur			ExempleJoueur;			const type_info &TypeJoueur = typeid(ExempleJoueur);
	Actionneur		ExempleActionneur;		const type_info &TypeActionneur = typeid(ExempleActionneur);
	Coffre			ExempleCoffre;			const type_info &TypeCoffre = typeid(ExempleCoffre);
	Cadavre			ExempleCadavre;			const type_info &TypeCadavre = typeid(ExempleCadavre);

	for (auto& carte : Partie.maps)
	{
		string fileName = PATH + carte.second.Id + ".map";

		ofstream fileStream(fileName, ios_base::in|ios_base::trunc);

		if (!fileStream.good()) Erreur("Erreur de sauvegarde : Le fichier suivant n'a pu être ouvert :", fileName);

		//Parcours de la liste des Elements :
		for (auto& tmp : carte.second.elements)
		{
			if (tmp->Liste == "NO_SAVE") continue;

			const type_info &TypeTmp = typeid(*tmp);

			if (TypeTmp == TypeUnique)		fileStream << "IND_UNIQUE ";
			if (TypeTmp == TypeJoueur)		fileStream << "JOUEUR ";
			if (TypeTmp == TypeActionneur)	fileStream << "ACTIONNEUR ";
			if (TypeTmp == TypeCoffre)		fileStream << "COFFRE ";
			if (TypeTmp == TypeCadavre)		fileStream << "CADAVRE ";

			fileStream << tmp->Type << " "
					   << tmp->Liste << " " << tmp->PosX << " " << tmp->PosY << " "
					   << tmp->Id << " "
					   << tmp->collisionType << " " << tmp->TypeClassement << " ";

			if (TypeTmp == TypeUnique || TypeTmp == TypeJoueur)
			{
				Individu_Unique* ind = dynamic_cast<Individu_Unique*>(tmp);
				fileStream << *(ind->Get_Stats()) << " " << *(ind->Get_Caracs()) << " ";

				if (TypeTmp == TypeJoueur)
				{
					Joueur* ind2 = dynamic_cast<Joueur*>(tmp);
					fileStream << ind2->Experience << " " << ind2->IndiceLieu << " " << ind2->LieuVillage << " "
							   << ind2->DureeEveil << " ";
					fileStream << ind2->BufForce << " " << ind2->BufPuissance << " " << ind2->BufAgilite << " " << ind2->BufIntelligence << " "
							   << ind2->BufConstitution << " " << ind2->BufEsquive << " " << ind2->BufCharisme << " ";

					//Nom du joueur :
					string nom = "";
					sf::Utf32::toUtf8(Partie.perso->Nom.begin(), Partie.perso->Nom.end(), back_inserter(nom));
					fileStream << nom << " ";
				}

				//Équipement & Inventaire :
 				ind->Get_Caracs()->objects.saveObjects(fileStream);

				fileStream << endl;
			}

			if (TypeTmp == TypeActionneur)
			{
				Actionneur *act = dynamic_cast<Actionneur*>(tmp);
				fileStream << act->RayonCollision << " " << act->RayX << " " << act->RayY << " ";
				if (act->Type == ACTION_CGMT_CARTE) fileStream << act->Type << " " << act->DonneeString;
				else fileStream << act->Type << " " << act->DonneeInt;
				fileStream << endl;
			}

			if (TypeTmp == TypeCoffre)
			{
				fileStream << tmp->RayonCollision << " " << tmp->RayX << " " << tmp->RayY << " ";

				Coffre *cof = dynamic_cast<Coffre*>(tmp);
				fileStream << cof->NumeroNom << " ";
				cof->objects.saveObjects(fileStream);

				fileStream << endl;
			}
			if (TypeTmp == TypeCadavre)
			{
				Cadavre *cad = dynamic_cast<Cadavre*>(tmp);
                fileStream << cad->Ind_Id << " " << cad->Duree << " " << cad->imageKey << " ";
				cad->objects.saveObjects(fileStream);

				fileStream << endl;
			}
		}

		fileStream.close();
	}


	MESSAGE(">> Sauvegarde du jeu terminée <<", FICHIER)
    options::updateCurrentSavedGamePack();

    string path = tools::filesystem::getSaveDirectoryPath() + options::getCurrentSavedGameDirectory() + "/";

    //'files' will store the filenames to create index.xml
    list<string> files;


    //gamedata.xml
    XMLDocument gamedata_xml;

    XMLElement* gamedata_xml_root = gamedata_xml.NewElement("gamedata");
    gamedata_xml.InsertFirstChild(gamedata_xml_root);

    XMLHandle gamedata_xml_handle(gamedata_xml_root);
    gamedata::saveToXML(gamedata_xml, gamedata_xml_handle);

    gamedata_xml.SaveFile((path + "gamedata.xml").c_str());
    files.push_back("gamedata.xml");


    //worlds xml files
    const auto& worlds = gamedata::worlds();

    for (const auto& w : worlds)
    {
        XMLDocument world_xml;

        XMLElement* world_xml_root = world_xml.NewElement("world");
        world_xml.InsertFirstChild(world_xml_root);

        XMLHandle world_xml_handle(world_xml_root);
        w.second->saveToXML(world_xml, world_xml_handle);

        world_xml.SaveFile((path + w.second->Id + ".xml").c_str());
        files.push_back(w.second->Id + ".xml");
    }


    //index.xml
    XMLDocument index_xml;

    XMLElement* index_xml_root = index_xml.NewElement("files");
    index_xml.InsertFirstChild(index_xml_root);

    for (auto& f : files)
    {
        XMLElement* fileName = index_xml.NewElement("file");
        fileName->SetAttribute("path", f.c_str());
        index_xml_root->InsertEndChild(fileName);
    }

    index_xml.SaveFile((path + "index.xml").c_str());
}

bool Load_Partie(string path)
{
	/* 1. CHARGEMENT DU FICHIER PRINCIPAL */

	string PATH = tools::filesystem::getSaveDirectoryPath() + path + "/";

	string mainFileName = PATH + "save.sav";

	ifstream mainFileStream(mainFileName, ios_base::in);

	if (!mainFileStream.good()) Erreur("Le fichier suivant n'a pu être chargé :", mainFileName);
	if (mainFileStream.good()) MESSAGE(" Fichier \"" + mainFileName + "\" ouvert", FICHIER)

	string TypeDonnee;
	int DonneeInt; string DonneeString1, DonneeString2;

	while(mainFileStream.rdstate() == 0)
	{
		mainFileStream >> TypeDonnee;
		if (TypeDonnee == "CARTE")
		{
			mainFileStream >> DonneeString1;
			Ajouter_Carte(DonneeString1);
		}
		if (TypeDonnee == "CARTE_COURANTE")
		{
			mainFileStream >> DonneeString1;
			for (auto& tmp : Partie.maps)
			{
				if (tmp.second.Id == DonneeString1)
				{
					Partie.CarteCourante = &(tmp.second);
					break;
				}
			}
			if (Partie.CarteCourante == NULL)
			{
				Erreur("La sauvegarde ne fournit pas de CarteCourante valide", "");
				mainFileStream.close();
				return false;
			}
		}
		if (TypeDonnee == "ID_SUIVANT")
		{
			mainFileStream >> DonneeInt;
			if (NouveauId() < DonneeInt) while (NouveauId() != DonneeInt) {}
		}
		if (TypeDonnee == "JOURNAL")
		{
			mainFileStream >> DonneeString1 >> DonneeInt;
			Partie.journal.addEntry(DonneeString1);
			if (DonneeInt) Partie.journal.setDone(DonneeString1);
		}

		//Quests cannot be loaded here because we need to load maps first

		TypeDonnee = "";
	}

	mainFileStream.clear();
	mainFileStream.seekg(0);


	/* 2. CHARGEMENT DES FICHIERS CARTE */

	string StrType, StrListe;
	float FloatX, FloatY;

	for (auto& carte : Partie.maps)
	{
		MESSAGE("=== Chargement de la carte " + carte.second.Id + " ===", FICHIER)

		string fileName = PATH + carte.second.Id + ".map";

		ifstream fileStream(fileName, ios_base::in);

		if (!fileStream.good()) Erreur("Le fichier suivant n'a pu être chargé :", fileName);

		while (fileStream.rdstate() == 0)
		{
			fileStream >> TypeDonnee;

			if (TypeDonnee == "backgroundImage")
			{
				fileStream >> StrType; carte.second.setBackgroundImage(StrType);
			}

			if (TypeDonnee == "AMBIENCE")
			{
				fileStream >> StrType;
                musicManager::addMusic(StrType);
				carte.second.ambience = StrType;
			}

			if (TypeDonnee == "IND_UNIQUE")
			{
				fileStream >> StrType >> StrListe >> FloatX >> FloatY;
				Individu_Unique* ind = carte.second.AjouterElement_Unique(StrType, StrListe, FloatX, FloatY);
				fileStream >> ind->Id >> ind->collisionType >> ind->TypeClassement;
				fileStream >> *(ind->Get_Stats()) >> *(ind->Get_Caracs());

				//Objects:
				ind->Get_Caracs()->objects.deleteObjects();
				fileStream >> ind->Get_Caracs()->objects;
			}

			if (TypeDonnee == "IND_COMMUN")
			{
				fileStream >> StrType >> StrListe >> FloatX >> FloatY;
				Load_ClasseCommune(StrType);
				Individu_Commun* ind = carte.second.AjouterElement_Commun(StrType, StrListe, FloatX, FloatY);
				fileStream >> ind->Id >> ind->collisionType >> ind->TypeClassement;
				fileStream >> *(ind->Get_Stats());
			}

			if (TypeDonnee == "JOUEUR")
			{
				fileStream >> StrType >> StrListe >> FloatX >> FloatY;
				Joueur* ind = carte.second.AjouterJoueur(StrType, StrListe, FloatX, FloatY);
				fileStream >> ind->Id >> ind->collisionType >> ind->TypeClassement;
				fileStream >> *(ind->Get_Stats()) >> *(ind->Get_Caracs())
						   >> ind->Experience >> ind->IndiceLieu >> ind->LieuVillage
						   >> ind->DureeEveil;

				fileStream >> ind->BufForce >> ind->BufPuissance >> ind->BufAgilite >> ind->BufIntelligence
						   >> ind->BufConstitution >> ind->BufEsquive >> ind->BufCharisme;

				//Nom du joueur : (on ne peut pas utiliser l'opérateur >> qui utilise en fait getline())
				string nom = "";
				fileStream >> nom;
				ind->Nom.clear();
				sf::Utf8::toUtf32(nom.begin(), nom.end(), back_inserter(ind->Nom));

				//Objects:
				ind->Get_Caracs()->objects.deleteObjects();
				fileStream >> ind->Get_Caracs()->objects;

				//Mise à jour de PosCarte
				Lag_PosFondCartes(-ind->PosX, -ind->PosY);
			}

			if (TypeDonnee == "PAYSAGE")
			{
				fileStream >> StrType >> StrListe >> FloatX >> FloatY;
				Load_ClassePaysage(StrType);
				Paysage* ind = carte.second.AjouterPaysage(StrType, StrListe, FloatX, FloatY);
				fileStream >> ind->Id >> ind->collisionType >> ind->TypeClassement;
			}

			if (TypeDonnee == "PAYSAGE-REPEAT")
			{
				fileStream >> StrType >> StrListe >> FloatX >> FloatY;
				Load_ClassePaysage(StrType);
				Paysage* ind = carte.second.AjouterPaysage(StrType, StrListe, FloatX, FloatY);
				fileStream >> ind->Id >> ind->collisionType >> ind->TypeClassement >> ind->repeatX >> ind->repeatY;
				ind->calculateCollisionRadius();
				carte.second.setMaxRadius(ind->RayX);
				carte.second.setMaxRadius(ind->RayY);
			}

			if (TypeDonnee == "ACTIONNEUR")
			{
				fileStream >> StrType >> StrListe >> FloatX >> FloatY;
				Actionneur* ind = carte.second.AjouterActionneur(StrListe, FloatX, FloatY);
				fileStream >> ind->Id >> ind->collisionType >> ind->TypeClassement;
				fileStream >> ind->RayonCollision >> ind->RayX >> ind->RayY;
				fileStream >> ind->Type;
				if (ind->Type == ACTION_CGMT_CARTE) fileStream >> ind->DonneeString;
				else fileStream >> ind->DonneeInt;
			}

			if (TypeDonnee == "COFFRE")
			{
				fileStream >> StrType >> StrListe >> FloatX >> FloatY;
				Coffre* ind = carte.second.AjouterCoffre(StrListe, FloatX, FloatY);
				fileStream >> ind->Id >> ind->collisionType >> ind->TypeClassement;
				fileStream >> ind->RayonCollision >> ind->RayX >> ind->RayY >> ind->NumeroNom;
				ind->Nom = getTranslatedNameOfElement(ind->NumeroNom);

				fileStream >> ind->objects;
			}

			if (TypeDonnee == "CADAVRE")
			{
				fileStream >> StrType >> StrListe >> FloatX >> FloatY;
				Cadavre* ind = carte.second.AjouterCadavre(StrListe, FloatX, FloatY);
				fileStream >> ind->Id >> ind->collisionType >> ind->TypeClassement;
                fileStream >> ind->Ind_Id >> ind->Duree >> ind->imageKey;
				ind->Set_Individu();

				fileStream >> ind->objects;
			}

			TypeDonnee = "";
		}

		fileStream.close();
	}

	/* 3. QUESTS */

	while (mainFileStream.rdstate() == 0)
	{
		mainFileStream >> TypeDonnee;
		if (TypeDonnee == "QUEST")
		{
			mainFileStream >> DonneeString1;
			getline(mainFileStream, DonneeString2);
			loadQuestFromSavedGame(DonneeString1, DonneeString2);
		}
		TypeDonnee = "";
	}

	mainFileStream.close();

	return true;
}
