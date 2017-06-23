
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

#include "tools/filesystem.h"
#include "musicManager/musicManager.h"

#include "../Bibliotheque/Bibliotheque.h"
#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/Templates.h"
#include "Jeu.h"
#include "../Carte/Carte.h"
#include "../ElementsCarte/ElementsCarte.h"


void Save_Partie()
{
	/* 1. FICHIER PRINCIPAL DE LA SAUVEGARDE */

	string PATH = tools::filesystem::getSaveDirectoryPath() + Partie.SAVE + "/";

	string mainFileName = PATH + "save.sav";

	ofstream mainFileStream(mainFileName, ios_base::in|ios_base::trunc);

	if (!mainFileStream.good()) Erreur("Erreur de sauvegarde : Le fichier suivant n'a pu être ouvert :", mainFileName);
	if (mainFileStream.good()) MESSAGE(" Fichier \"" + mainFileName + "\" ouvert ; Sauvegarde en cours", FICHIER)

	//Liste des cartes et carte courante :
	for (auto& tmp : Partie.maps)
		mainFileStream << "CARTE " << tmp.second.Id << endl;

	mainFileStream << "CARTE_COURANTE " << Partie.CarteCourante->Id << endl;

	//ID du prochain élément qui sera chargé :
	mainFileStream << "ID_SUIVANT " << NouveauId() << endl;

	//Missions :
	saveQuests(mainFileStream);

	//Journal entries:
	for (auto i : Partie.journal.entries)
		mainFileStream << "JOURNAL " << i.reference << " " << i.done << endl;

	mainFileStream.close();


	/* 2. FICHIERS DE CARTE */

	//Création d'éléments pour récupérer le type et le comparer aux Elements de la liste
	Individu_Unique	ExempleUnique;			const type_info &TypeUnique = typeid(ExempleUnique);
	Individu_Commun	ExempleCommun;			const type_info &TypeCommun = typeid(ExempleCommun);
	Joueur			ExempleJoueur;			const type_info &TypeJoueur = typeid(ExempleJoueur);
	Paysage			ExemplePaysage;			const type_info &TypePaysage = typeid(ExemplePaysage);
	Paysage_Mouvant	ExempleMouvant;			const type_info &TypeMouvant = typeid(ExempleMouvant);
	Actionneur		ExempleActionneur;		const type_info &TypeActionneur = typeid(ExempleActionneur);
	Coffre			ExempleCoffre;			const type_info &TypeCoffre = typeid(ExempleCoffre);
	Cadavre			ExempleCadavre;			const type_info &TypeCadavre = typeid(ExempleCadavre);

	for (auto& carte : Partie.maps)
	{
		string fileName = PATH + carte.second.Id + ".map";

		ofstream fileStream(fileName, ios_base::in|ios_base::trunc);

		if (!fileStream.good()) Erreur("Erreur de sauvegarde : Le fichier suivant n'a pu être ouvert :", fileName);

		if (carte.second.backgroundImage != "") fileStream << "backgroundImage " << carte.second.backgroundImage << endl;
		fileStream << "AMBIENCE " << carte.second.ambience << endl;

		//Parcours de la liste des Elements :
		for (auto& tmp : carte.second.elements)
		{
			if (tmp->Liste == "NO_SAVE") continue;

			const type_info &TypeTmp = typeid(*tmp);

			if (TypeTmp == TypeUnique)		fileStream << "IND_UNIQUE ";
			if (TypeTmp == TypeCommun)		fileStream << "IND_COMMUN ";
			if (TypeTmp == TypeJoueur)		fileStream << "JOUEUR ";
			if (TypeTmp == TypePaysage)
			{
				Paysage* pay = dynamic_cast<Paysage*>(tmp);
				if (pay->repeatX > 1 || pay->repeatY > 1) fileStream << "PAYSAGE-REPEAT ";
				else fileStream << "PAYSAGE ";
			}
			if (TypeTmp == TypeMouvant)		fileStream << "PAYSAGE_MOUVANT ";
			if (TypeTmp == TypeActionneur)	fileStream << "ACTIONNEUR ";
			if (TypeTmp == TypeCoffre)		fileStream << "COFFRE ";
			if (TypeTmp == TypeCadavre)		fileStream << "CADAVRE ";

			fileStream << tmp->Type << " "
					   << tmp->Liste << " " << tmp->PosX << " " << tmp->PosY << " "
					   << tmp->Id << " "
					   << tmp->collisionType << " " << tmp->TypeClassement << " ";

			if (TypeTmp == TypeCommun)
			{
				Individu_Commun* ind = dynamic_cast<Individu_Commun*>(tmp);
				fileStream << *(ind->Get_Stats()) << endl;
			}

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

			if (TypeTmp == TypePaysage)
			{
				Paysage *pay = dynamic_cast<Paysage*>(tmp);
				if (pay->repeatX > 1 || pay->repeatY > 1) fileStream << pay->repeatX << " " << pay->repeatY;
			}

			if (TypeTmp == TypeMouvant) fileStream << endl;

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

	/* 3. CAPTURE D'ÉCRAN */

	Texture Capture;
	Capture.create(Options.ScreenW, Options.ScreenH);
	Capture.setSmooth(true);
	Capture.update(Jeu.App);

	Image CaptureImg = Capture.copyToImage();
	CaptureImg.saveToFile(PATH + "capture.png");

	MaJ_Sauvegarde();

	MESSAGE(">> Sauvegarde du jeu terminée <<", FICHIER)
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

			if (TypeDonnee == "PAYSAGE_MOUVANT")
			{
				fileStream >> StrType >> StrListe >> FloatX >> FloatY;
				Load_ClassePaysageMouvant(StrType);
				Paysage_Mouvant* ind = carte.second.AjouterPaysageMouvant(StrType, StrListe, FloatX, FloatY);
				fileStream >> ind->Id >> /*ind->collisionType >>*/ ind->TypeClassement;
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
