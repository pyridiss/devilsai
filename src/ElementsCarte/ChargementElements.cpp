
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

#include <physfs.h>

#include "../Bibliotheque/Constantes.h"
#include "../Jeu/Jeu.h"
#include "../Carte/Carte.h"
#include "ElementsCarte.h"


void Load_IndividuUnique(string Type, Individu_Unique *ind)
{
	string fichier = Partie.DATA + "individu/" + Type + ".one";

	ind->Nom = Get_NomElement(Type);

	Activite *act = NULL;

	ifstream Fichier(fichier, ios_base::in);

	if (Fichier == NULL) Erreur("Le fichier suivant n'a pu être chargé :", fichier);
	if (Fichier != NULL) MESSAGE(" Fichier \"" + fichier + "\" ouvert", FICHIER)

	short Act, Dir, Num;
	float Matrice[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

	string TypeDonnee;

	string ArchiveImg = Partie.DATA + "img/";

	while (Fichier.rdstate() == 0)
	{
		Fichier >> TypeDonnee;

		if (TypeDonnee == "ARCHIVE_IMG")
		{
			string bufferString;	Fichier >> bufferString;	ArchiveImg += bufferString;
			if (PHYSFS_addToSearchPath(ArchiveImg.c_str(), 1) == 0)
			{
				Erreur("Le fichier suivant n'a pu être chargé :", ArchiveImg.c_str());
			}
			else MESSAGE(" Fichier \"" + ArchiveImg + "\" ouvert", FICHIER)
		}
		if (TypeDonnee == "MATRICE_TEINTE")
		{
			Fichier >> Matrice[0][0] >> Matrice[0][1] >> Matrice[0][2]
					>> Matrice[1][0] >> Matrice[1][1] >> Matrice[1][2]
					>> Matrice[2][0] >> Matrice[2][1] >> Matrice[2][2];
		}
		if (TypeDonnee == "EMPLACEMENT_EQUIP")
		{
			int x, y, w, h;
			string cat, type, classe;
			Fichier >> x >> y >> w >> h >> cat >> type >> classe;
			ind->Ajouter_EmplacementEquip(x, y, w, h, cat, type, classe);
		}
		if (TypeDonnee == "EQUIPEMENT")
		{
			string numero = "", IdEmplacement = "";
			Fichier >> IdEmplacement >> numero;

			ind->Get_Caracs()->objects.addObject(numero, IdEmplacement);
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
							ind->Get_Caracs()->objects.addObject(IdObjet, intToString(CLEF_INVENTAIRE + NumeroObjet), Qualite);
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
		if (TypeDonnee == "SKILL")
		{
			string numero = ""; Fichier >> numero;
			ind->Get_Caracs()->addSkill(numero, (int)ind);
		}
		if (TypeDonnee == "DEFAUT_CTRL")
		{
			string numero = ""; Fichier >> numero;
			mapSkills::iterator i = ind->Get_Caracs()->skills.find(numero);
			if (i != ind->Get_Caracs()->skills.end()) ind->skillLinks[COMPETENCE_CTRL] = i->second;
		}
		if (TypeDonnee == "DEFAUT_SHIFT")
		{
			string numero = ""; Fichier >> numero;
			mapSkills::iterator i = ind->Get_Caracs()->skills.find(numero);
			if (i != ind->Get_Caracs()->skills.end()) ind->skillLinks[COMPETENCE_SHIFT] = i->second;
		}
		if (TypeDonnee == "DEFAUT_TAB")
		{
			string numero = ""; Fichier >> numero;
			mapSkills::iterator i = ind->Get_Caracs()->skills.find(numero);
			if (i != ind->Get_Caracs()->skills.end()) ind->skillLinks[COMPETENCE_TAB] = i->second;
		}
		if (TypeDonnee == "DEFAUT_SPACE")
		{
			string numero = ""; Fichier >> numero;
			mapSkills::iterator i = ind->Get_Caracs()->skills.find(numero);
			if (i != ind->Get_Caracs()->skills.end()) ind->skillLinks[COMPETENCE_SPACE] = i->second;
		}
		if (TypeDonnee == "DIPLOM")		Fichier >> ind->Diplomatie;
		if (TypeDonnee == "CMP_VIS")
		{
			Fichier >> ind->ChampVision;
			CalculerRayonMax(ind->ChampVision);
		}
		if (TypeDonnee == "RAY_COL")	Fichier >> ind->RayonCollision;
		if (TypeDonnee == "RAY_ATT" || TypeDonnee == "RAY_INTER")	Fichier >> ind->RayonInteraction;
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
		if (TypeDonnee == "DIR")
		{
			Fichier >> Dir;
		}

		if (TypeDonnee == "img")
		{
			Fichier >> Num;
			AjouterImagesIndividu(Type, Act, Dir, Num, Matrice);
			act->Num_Max[Dir] = Num;
		}

		if (TypeDonnee == "FIN_ACT")
		{
			for (int i = 0 ; i < Num ; ++i)
				act->addImage(Act, i);
			Act = 0; Dir = 0; Num = 0;
			act = NULL;
		}

		if (TypeDonnee == "FORCE")		Fichier >> ind->Caracs["Force"];
		if (TypeDonnee == "PUISS")		Fichier >> ind->Caracs["Puissance"];
		if (TypeDonnee == "AGILITE")	Fichier >> ind->Caracs["Agilite"];
		if (TypeDonnee == "INTELLI")	Fichier >> ind->Caracs["Intelligence"];
		if (TypeDonnee == "CONSTIT")	Fichier >> ind->Caracs["Constitution"];
		if (TypeDonnee == "ESQUIVE")	Fichier >> ind->Caracs["Esquive"];
		if (TypeDonnee == "CHARISM")	Fichier >> ind->Caracs["Charisme"];
		if (TypeDonnee == "RECUP")
		{
			Fichier >> ind->Caracs["RecuperationMoyenne"];
			ind->Set_Recuperation(ind->Caracs["RecuperationMoyenne"]);
		}
		if (TypeDonnee == "EXP")		Fichier >> ind->Experience;
		if (TypeDonnee == "CADAVRE")	Fichier >> ind->DureeCadavre;
		if (TypeDonnee == "RECUP_FIXE")		ind->RecuperationFixe = true;
		if (TypeDonnee == "ENERGIE_MAX")	ind->EnergieMax = true;

		TypeDonnee = "";
	}

	if (ArchiveImg != Partie.DATA + "img/") PHYSFS_removeFromSearchPath(ArchiveImg.c_str());

	Fichier.close();
}

void Load_ClasseCommune(string Type)
{
	if (getCommonClass(Type) == NULL)
	{
		addCommonClass(Type);
		getCommonClass(Type)->Nom = Get_NomElement(Type);
	}
	else return;

	string fichier = Partie.DATA + "individu/" + Type + ".com";

	ifstream Fichier(fichier, ios_base::in);

	if (Fichier == NULL) Erreur("Le fichier suivant n'a pu être chargé :", fichier);
	if (Fichier != NULL) MESSAGE(" Fichier \"" + fichier + "\" ouvert", FICHIER)

	Classe_Commune *cl_com = getCommonClass(Type);

	Activite *act = NULL;
	short Act, Dir, Num;
	string TypeDonnee;
//	int ClefStuff = CLEF_STUFF;
	float Matrice[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

	string ArchiveImg = Partie.DATA + "img/";

	while (Fichier.rdstate() == 0)
	{
		Fichier >> TypeDonnee;

		if (TypeDonnee == "ARCHIVE_IMG")
		{
			string bufferString;	Fichier >> bufferString;	ArchiveImg += bufferString;
			if (PHYSFS_addToSearchPath(ArchiveImg.c_str(), 1) == 0)
			{
				Erreur("Le fichier suivant n'a pu être chargé :", ArchiveImg.c_str());
			}
			else MESSAGE(" > Fichier \"" + ArchiveImg + "\" ouvert", FICHIER)
		}
		if (TypeDonnee == "MATRICE_TEINTE")
		{
			Fichier >> Matrice[0][0] >> Matrice[0][1] >> Matrice[0][2]
					>> Matrice[1][0] >> Matrice[1][1] >> Matrice[1][2]
					>> Matrice[2][0] >> Matrice[2][1] >> Matrice[2][2];
		}
		if (TypeDonnee == "DIPLOM")		Fichier >> cl_com->Diplomatie;
		if (TypeDonnee == "CMP_VIS")
		{
			Fichier >> cl_com->ChampVision;
			CalculerRayonMax(cl_com->ChampVision);
		}
		if (TypeDonnee == "RAY_COL")	Fichier >> cl_com->RayonCollision;
		if (TypeDonnee == "RAY_ATT" || TypeDonnee == "RAY_INTER")	Fichier >> cl_com->RayonInteraction;
// 		if (TypeDonnee == "EQUIPEMENT")
// 		{
// 			string id = ""; int qualite = 0;
// 			Fichier >> id >> qualite;
// 		}
		if (TypeDonnee == "INVENTAIRE")
		{
			TemplateObject object;
			Fichier >> object.fileName >> object.quality;

			cl_com->inventory.push_back(object);
//			cl_com->Get_Caracs()->addObject(id, clef, qualite);
// 			Objet obj;
// 			MapInventaire::iterator i;

// 			cl_com->Inventaire.insert(MapInventaire::value_type(CLEF_INVENTAIRE + id/100, obj));
// 			i = cl_com->Inventaire.find(CLEF_INVENTAIRE + id/100);

// 			Load_Objet(i->second, id);
// 			i->second.QualiteSup = qualite;
		}
		if (TypeDonnee == "STUFF")
		{
//			string id = ""; int qualite = 0, probabilite = 0;
			TemplateObject object;
			Fichier >> object.fileName >> object.quality >> object.probability;

			cl_com->stuff.push_back(object);
//			cl_com->Caracs.objects.addObject(id, intToString(ClefStuff), qualite);
// 			Objet obj;
// 			MapInventaire::iterator i;

// 			cl_com->Inventaire.insert(MapInventaire::value_type(ClefStuff, obj));
// 			i = cl_com->Inventaire.find(ClefStuff);

// 			Load_Objet(i->second, id);
// 			i->second.QualiteSup = qualite;
// 			++ClefStuff;
		}
		if (TypeDonnee == "ACT_DEFAUT")	Fichier >> cl_com->ActDefaut;
		if (TypeDonnee == "ACT")
		{
			Fichier >> Act;
			cl_com->Ajouter_Activite(Act);
			act = cl_com->Get_Activite(Act);
		}
		if (TypeDonnee == "blo")	Fichier >> act->priority;

		if (TypeDonnee == "VIT")	Fichier >> act->speed;
		if (TypeDonnee == "MAJ")	Fichier >> act->step;
		if (TypeDonnee == "DIR")
		{
			Fichier >> Dir;
			//cl_com->Caracs.DirDisponibles[Dir] = true;
		}

		if (TypeDonnee == "img")
		{
			Fichier >> Num;
			AjouterImagesIndividu(Type, Act, Dir, Num, Matrice);
			act->Num_Max[Dir] = Num;
		}

		if (TypeDonnee == "FIN_ACT")
		{
			for (int i = 0 ; i < Num ; ++i)
				act->addImage(Act, i);
			Act = 0; Dir = 0; Num = 0;
			act = NULL;
		}

		if (TypeDonnee == "FORCE")		Fichier >> cl_com->Caracs["Force"];
		if (TypeDonnee == "PUISS")		Fichier >> cl_com->Caracs["Puissance"];
		if (TypeDonnee == "AGILITE")	Fichier >> cl_com->Caracs["Agilite"];
		if (TypeDonnee == "INTELLI")	Fichier >> cl_com->Caracs["Intelligence"];
		if (TypeDonnee == "CONSTIT")	Fichier >> cl_com->Caracs["Constitution"];
		if (TypeDonnee == "ESQUIVE")	Fichier >> cl_com->Caracs["Esquive"];
		if (TypeDonnee == "CHARISM")	Fichier >> cl_com->Caracs["Charisme"];
		if (TypeDonnee == "RECUP")		Fichier >> cl_com->Caracs["RecuperationMoyenne"];
		if (TypeDonnee == "EXP")		Fichier >> cl_com->Experience;

		TypeDonnee = "";
	}

	if (ArchiveImg != Partie.DATA + "img/") PHYSFS_removeFromSearchPath(ArchiveImg.c_str());

	Fichier.close();
}

void Load_ClassePaysage(string Type)
{
	if (getLandsClass(Type) == NULL) addLandsClass(Type);
	else return;

	string fichier = Partie.DATA + "individu/" + Type + ".pay";

	ifstream Fichier(fichier, ios_base::in);

	if (Fichier == NULL) Erreur("Le fichier suivant n'a pu être chargé :", fichier);
	if (Fichier != NULL) MESSAGE(" Fichier \"" + fichier + "\" ouvert", FICHIER)

	Classe_Paysage *cl_pay = getLandsClass(Type);

	string TypeDonnee;
	int ExX = 0, ExY = 0;

	while (Fichier.rdstate() == 0)
	{
		Fichier >> TypeDonnee;

		if (TypeDonnee == "EXCENTR")
		{
			Fichier >> ExX >> ExY;
		}
		if (TypeDonnee == "RAY_COL")
		{
			Fichier >> cl_pay->RayonCollision;
			cl_pay->ModeCollision = MODE_COLLISION_CERCLE;
			CalculerRayonMax(cl_pay->RayonCollision);
		}
		if (TypeDonnee == "RECT_COL")
		{
			Fichier >> cl_pay->RayX >> cl_pay->RayY;
			cl_pay->ModeCollision = MODE_COLLISION_RECT;
			CalculerRayonMax(cl_pay->RayX); CalculerRayonMax(cl_pay->RayY);
		}
		if (TypeDonnee == "CLASSEMENT_SOL")		cl_pay->TypeClassement = CLASSEMENT_SOL;
		if (TypeDonnee == "CLASSEMENT_CADAVRE")	cl_pay->TypeClassement = CLASSEMENT_CADAVRE;
		if (TypeDonnee == "CLASSEMENT_NORMAL")	cl_pay->TypeClassement = CLASSEMENT_NORMAL;
		if (TypeDonnee == "CLASSEMENT_HAUT")	cl_pay->TypeClassement = CLASSEMENT_HAUT;
		if (TypeDonnee == "CLASSEMENT_NUAGE")	cl_pay->TypeClassement = CLASSEMENT_NUAGE;

		TypeDonnee = "";
	}

	AjouterImagePaysage(Type, ExX, ExY);

	Fichier.close();
}

void Load_ClassePaysageMouvant(string Type)
{
	if (getMovingLandsClass(Type) == NULL) addMovingLandsClass(Type);
	else return;

	string fichier = Partie.DATA + "individu/" + Type + ".mvt";

	ifstream Fichier(fichier, ios_base::in);

	if (Fichier == NULL) //Il peut s'agir d'un lanceur
	{
		fichier = Partie.DATA + "individu/" + Type + ".lan";

		Fichier.open(fichier, ios_base::in);

		//Introuvable en tant que PayMouvant ou en tant que Lanceur
		if (Fichier == NULL) Erreur("Le fichier suivant n'a pu être chargé :", fichier);
	}

	if (Fichier != NULL) MESSAGE(" Fichier \"" + fichier + "\" ouvert", FICHIER)

	Classe_Paysage_Mouvant *cl_paymvt = getMovingLandsClass(Type);

	string TypeDonnee;
	Activite *act = NULL;
	short Act, Dir, Num;
	int ExX = 0, ExY = 0;

	string ArchiveImg = Partie.DATA + "img/";

	while (Fichier.rdstate() == 0)
	{
		Fichier >> TypeDonnee;

		if (TypeDonnee == "ARCHIVE_IMG")
		{
			string bufferString;	Fichier >> bufferString;	ArchiveImg += bufferString;
			if (PHYSFS_addToSearchPath(ArchiveImg.c_str(), 1) == 0)
			{
				Erreur("Le fichier suivant n'a pu être chargé :", ArchiveImg.c_str());
			}
			else MESSAGE(" > Fichier \"" + ArchiveImg + "\" ouvert", FICHIER)
		}

		if (TypeDonnee == "EXCENTR")
		{
			Fichier >> ExX >> ExY;
		}
		if (TypeDonnee == "RAY_COL")
		{
			Fichier >> cl_paymvt->RayonCollision;
			cl_paymvt->ModeCollision = MODE_COLLISION_CERCLE;
			CalculerRayonMax(cl_paymvt->RayonCollision);
		}
		if (TypeDonnee == "RECT_COL")
		{
			Fichier >> cl_paymvt->RayX >> cl_paymvt->RayY;
			cl_paymvt->ModeCollision = MODE_COLLISION_RECT;
			CalculerRayonMax(cl_paymvt->RayX); CalculerRayonMax(cl_paymvt->RayY);
		}
		if (TypeDonnee == "CLASSEMENT_SOL")		cl_paymvt->TypeClassement = CLASSEMENT_SOL;
		if (TypeDonnee == "CLASSEMENT_CADAVRE")	cl_paymvt->TypeClassement = CLASSEMENT_CADAVRE;
		if (TypeDonnee == "CLASSEMENT_NORMAL")	cl_paymvt->TypeClassement = CLASSEMENT_NORMAL;
		if (TypeDonnee == "CLASSEMENT_HAUT")	cl_paymvt->TypeClassement = CLASSEMENT_HAUT;
		if (TypeDonnee == "CLASSEMENT_NUAGE")	cl_paymvt->TypeClassement = CLASSEMENT_NUAGE;
		if (TypeDonnee == "ACTIVATION_NECESSAIRE") cl_paymvt->ActivationNecessaire = true;
		if (TypeDonnee == "ACT")
		{
			Fichier >> Act;
			cl_paymvt->Ajouter_Activite(Act);
			act = cl_paymvt->Get_Activite(Act);
		}
		if (TypeDonnee == "blo")	Fichier >> act->priority;

		if (TypeDonnee == "VIT")	Fichier >> act->speed;
		if (TypeDonnee == "MAJ")	Fichier >> act->step;
		if (TypeDonnee == "DIR")	Fichier >> Dir;

		if (TypeDonnee == "img")
		{
			Fichier >> Num;
			AjouterImagesPaysageMouvant(Type, Act, Num, ExX, ExY);
			act->Num_Max[Dir] = Num;
		}

		if (TypeDonnee == "FIN_ACT")
		{
			Act = 0; Dir = 0; Num = 0;
			act = NULL;
		}
		TypeDonnee = "";
	}

	if (ArchiveImg != Partie.DATA + "img/") PHYSFS_removeFromSearchPath(ArchiveImg.c_str());

	Fichier.close();
}

void Load_PaysageLanceur(string Type, Paysage_Lanceur *ind)
{
	string fichier = Partie.DATA + "individu/" + Type;
	fichier += ".lan";

	ifstream Fichier(fichier, ios_base::in);

	if (Fichier == NULL) Erreur("Le fichier suivant n'a pu être chargé :", fichier);
	if (Fichier != NULL) MESSAGE(" Fichier \"" + fichier + "\" ouvert", FICHIER)

	string TypeDonnee;
	short dir = 0;

	while (Fichier.rdstate() == 0)
	{
		Fichier >> TypeDonnee;

		//Données du projectile lancé
		if (TypeDonnee == "PRJ_ID")			Fichier >> ind->ProjectileLance.Type;
		if (TypeDonnee == "PRJ_X")			Fichier >> ind->ProjectileLance.OrigineX;
		if (TypeDonnee == "PRJ_Y")			Fichier >> ind->ProjectileLance.OrigineY;
		if (TypeDonnee == "PRJ_DIPLOM")		Fichier >> ind->ProjectileLance.Diplomatie;
		if (TypeDonnee == "PRJ_RECT_COL")	Fichier >> ind->ProjectileLance.RayX >> ind->ProjectileLance.RayY;
		if (TypeDonnee == "PRJ_VIT")		Fichier >> ind->ProjectileLance.Deplacement.speed;
		if (TypeDonnee == "PRJ_MAJ")		Fichier >> ind->ProjectileLance.Deplacement.step;
		if (TypeDonnee == "PRJ_DIR")		Fichier >> dir;
		if (TypeDonnee == "PRJ_CHP_ATT")	Fichier >> ind->ProjectileLance.ChampAttaque;

		if (TypeDonnee == "PRJ_FORCE")		Fichier >> ind->ProjectileLance.Caracs["Force"];
		if (TypeDonnee == "PRJ_PUISS")		Fichier >> ind->ProjectileLance.Caracs["Puissance"];
		if (TypeDonnee == "PRJ_AGILITE")	Fichier >> ind->ProjectileLance.Caracs["Agilite"];
		if (TypeDonnee == "PRJ_INTELLI")	Fichier >> ind->ProjectileLance.Caracs["Intelligence"];

		TypeDonnee = "";
	}

	ind->ProjectileLance.Get_Activite(ind->ProjectileLance.Get_Act())->Num_Max[dir] = 0;
	ind->ProjectileLance.Set_Dir(dir);
	ind->ProjectileLance.OrigineX += ind->PosX;
	ind->ProjectileLance.OrigineY += ind->PosY;

	AjouterImageProjectile(ind->ProjectileLance.Type, ind->ProjectileLance.Get_Dir());

	Fichier.close();
}
