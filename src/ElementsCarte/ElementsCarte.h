
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

#ifndef header_elementscarte
#define header_elementscarte

#include <string>
#include <fstream>

#include <SFML/Window.hpp>

#include "../Attributs/Attributs.h"
#include "../Bibliotheque/Constantes.h"

using namespace std;
using namespace sf;

class Element_Carte;
class Individu;
class Individu_Commun;
class Paysage;
class Dialogue;


/** INDIVIDUS **/

struct TemplateObject
{
	string fileName = "";
	int quality = 0;
	int probability = 0;
};

class Classe_Commune
{
	public:
		String32 Nom;
		string Type             = "";
		int Diplomatie          = 0;
		int RayonCollision      = 0;
		int ChampVision         = 0;
		int RayonInteraction    = 0;
		short ActDefaut         = 0;

	public:
		unsigned int Experience = 0;

	public:
		Caracteristiques Caracs;
		list<TemplateObject> inventory;	//These objects are always put on corpses, whatever probability
		list<TemplateObject> stuff;

		MapActivites Activites;

	public:
		Classe_Commune();
		~Classe_Commune();

	public:
		void Ajouter_Activite(int Id);
		Activite* Get_Activite(int Id);

	public:
		void Copie_Element(Individu_Commun *elem);
};

class Classe_Paysage
{
	public:
		Classe_Paysage *next;

		string Type             = "";

		bool ModeCollision      = MODE_COLLISION_CERCLE;
		short RayonCollision    = 0;
		short RayX = 0, RayY = 0;
		short TypeClassement    = CLASSEMENT_NORMAL;

	public:
		Classe_Paysage();
		~Classe_Paysage();

		void Copie_Element(Paysage *elem);
};

class Classe_Paysage_Mouvant
{
	public:
		MapActivites Activites;

		string Type             = "";

		bool ModeCollision      = MODE_COLLISION_CERCLE;
		short RayonCollision    = 0;
		short RayX = 0, RayY = 0;
		short TypeClassement    = CLASSEMENT_NORMAL;

		Caracteristiques Caracs;
		bool ActivationNecessaire	= false;
//		bool ActivationUnique;

	public:
		Classe_Paysage_Mouvant *next;

	//Constructeurs / Destructeurs :
	public:
		Classe_Paysage_Mouvant();
		~Classe_Paysage_Mouvant();

	//Gestion :
	public:
		void Ajouter_Activite(int Id);
		Activite* Get_Activite(int Id);

	public:
		void Copie_Element(Element_Carte *elem);
};

class Element_Carte
{
	//Eléments de la liste chainée :
	public:
		Element_Carte *next;

	//Objet :
	public:
		int Id              = -1;
		string Type         = "DEFAUT";
		string Liste        = "DEFAUT";
		short Diplomatie    = 0;

	protected:
		short Controle      = AUCUN;

	public:
		float PosX = 0, PosY = 0;

		bool ModeCollision      = MODE_COLLISION_CERCLE;
		short RayonCollision    = 0;
		short RayX = 0, RayY = 0;
		bool AjouterDansListeCollision  = true;
		short TypeClassement    = CLASSEMENT_NORMAL;

	//Constructeurs / Destructeurs :
	public:
		Element_Carte();
		virtual ~Element_Carte();

	//Getter :
	public:
		short Get_Controle();

	//Gestion :
	public:
		void Lag_Pos(float x, float y);
		void Set_Controle(short nv);

		virtual int Gestion();

		virtual int Collision(Individu *elem, int TypeCollision) =0;

	//Affichage :
	public:
		virtual void Disp(float RefX, float RefY) =0;
		virtual void Disp_Masks(float RefX, float RefY) =0;
};

class Coffre : public Element_Carte
{
	public:
		Objects objects;

		string NumeroNom = "";
		String32 Nom;

	public:
		Coffre() : Element_Carte() {}

	//Gestion :
	public:
		int Gestion();
		int Collision(Individu* elem, int TypeCollision);

	//Affichage :
	public:
		void Disp(float RefX, float RefY);
		void Disp_Masks(float RefX, float RefY);
};

class Cadavre : public Coffre
{
	public:
		string Ind_Id	= "";
		short Ind_Dir	= 0;
		short Ind_Num	= 0;
		float Duree		= 0;
		bool Vide		= false;

		string FichierIndividu = "";

	public:
		Cadavre();

	//Gestion :
	public:
		int Gestion();
		void Set_Individu();
		void Set_Individu(string type, short dir, short num, bool Unique);

	//Affichage :
	public:
		void Disp(float RefX, float RefY);
};

class Element_Mouvant : public Element_Carte
{
	//Objet
	protected:
		short Act   = 0;
		short Dir   = SUD;
		short Num   = 0;

	public:
		short ActDefaut     = 0;
		bool ActEffectue    = true;

	public:
		short FinMouvement  = 0;
		float Temps         = 0;

	public:
		int Comportement        = COMPORTEMENT_ALEATOIRE;
		int NouveauComportement = COMPORTEMENT_ALEATOIRE;
		int ElementVision       = -1;

		bool ObstacleRencontre  = false;
		//8 directions principales au maximum, puis déviation.
		float Dist[8]           = {0};
		short Pref[8]           = {0};
		int Devn[8]             = {0};
		short NombrePas[6]      = {100};
		short DerniersPas       = 0;
		short Pietinement       = false;
		short DirPietinement    = 0;
		bool ArreterChasse      = false;


	//Constructeurs / Destructeurs :
	public:
		Element_Mouvant() : Element_Carte() {}
		virtual ~Element_Mouvant() {}

	//Getter :
	public:
		short Get_Act();
		short Get_Dir();
		short Get_Num();

		virtual Activite* Get_Activite(short act) =0;

	//Gestion :
	public:
		virtual int Gestion();
		virtual bool Set_Activite(int nv);
		virtual int Get_Vitesse(short act);
		void Set_Dir(int nv);
		int Collision(Individu *elem, int TypeCollision);
		void IncrementNum(bool RaZ = false);
};

class Individu : public Element_Mouvant
{
	//Objet :
	protected:
		Statistiques Stats;

	public:
		int IndiceLieu      = 0;
		bool LieuVillage    = false;

		float DureeCadavre  = 0.5;

		float buf_rec       = 0;

	//Constructeurs / Destructeurs :
	public:
		Individu();
		virtual ~Individu() {}

	//Getters :
	public:
		virtual String32& Get_Nom() = 0;
		virtual short Get_ChampVision() = 0;
		virtual short Get_RayonInteraction() = 0;
		virtual short Get_NombreDir() = 0;
		virtual int Get_Experience() = 0;
		virtual Caracteristiques* Get_Caracs() =0;
		virtual Statistiques* Get_Stats() =0;

	//Gestion :
	public:
		int Gestion();
		void Set_Dir(int nv);
		void MouvementAleatoire(int Iteration);
		bool MouvementChasse(Element_Carte *elem);
		virtual void Gestion_Recuperation();

	//Getters stats et caracs
	public:
		float Get_Vitalite();
		float Get_Energie();
		float Get_Recuperation();

		virtual unsigned int Get_Force();
		virtual unsigned int Get_Puissance();
		virtual unsigned short Get_Agilite();
		virtual unsigned short Get_Intelligence();
		virtual unsigned short Get_Constitution();
		virtual unsigned short Get_Esquive();
		virtual unsigned short Get_Charisme();
		virtual int Get_RecuperationMoyenne();

	//Setters de vitalité, énergie et récupération
	public:
		void Set_Vitalite(float vit);
		void Lag_Vitalite(float lag);
		void Set_Energie(float ene);
		void Lag_Energie(float lag);
		void Set_Recuperation(float rec);
		virtual void Lag_Recuperation(float lag);

	//Affichage
	public:
		void Disp(float RefX, float RefY);
};

class Individu_Unique : public Individu
{
	public:
		String32 Nom;
		int ChampVision         = 0;
		int RayonInteraction    = 0;
		unsigned int Experience = 0;
		bool RecuperationFixe   = false;
		bool EnergieMax         = false;

	public:
		MapActivites Activites;
// 		MapPointeursCompetences Competences;

		Caracteristiques Caracs;

//		MapEquipement Equips;
//		mapObjects Inventaire;
		ListEmplacementsEquipements EmplacementsEquip;

	public:
 		lua_State* skillLinks[4]    = {NULL};

	//Constructeurs / Destructeurs :
	public:
		Individu_Unique();
		virtual ~Individu_Unique();

	public:
		void Ajouter_Activite(int Id);
		void Ajouter_EmplacementEquip(int x, int y, int w, int h, string Categorie, string Type, string Classe);

	//Getter :
	public:
		String32& Get_Nom();
		short Get_ChampVision();
		short Get_RayonInteraction();
		short Get_NombreDir();
		int Get_Experience();
		Caracteristiques* Get_Caracs();
		Statistiques* Get_Stats();
		virtual Activite* Get_Activite(short act);

		unsigned int Get_Force();
		unsigned int Get_Puissance();
		unsigned short Get_Agilite();
		unsigned short Get_Intelligence();
		unsigned short Get_Constitution();
		unsigned short Get_Esquive();
		unsigned short Get_Charisme();
		int Get_RecuperationMoyenne();

		int Get_Vitesse(short act);

		void Gestion_Recuperation();
		void Lag_Recuperation(float lag);

		bool Set_Activite(int nv);

	//Affichage :
	public:
		void Disp_Masks(float RefX, float RefY);
};

class Individu_Commun : public Individu
{
	//Objet :
	public:
		Classe_Commune *Classe;

	//Constructeurs / Destructeurs :
	public:
		Individu_Commun();
		virtual ~Individu_Commun() {}

	//Getter :
	public:
		String32& Get_Nom();
		short Get_ChampVision();
		short Get_RayonInteraction();
		short Get_NombreDir();
		int Get_Experience();
		Caracteristiques* Get_Caracs();
		Statistiques* Get_Stats();
		Activite* Get_Activite(short act);

		bool Set_Activite(int nv);

	//Affichage :
	public:
		void Disp_Masks(float RefX, float RefY);
};

class Joueur : public Individu_Unique
{
	public:
		float DureeEveil            = 0;
		int ElementInteraction      = -1;
		int SauvegardeIndiceLieu    = 0;
		bool GestionCoffresActivee  = false;

	public:
		float BufForce = 0, BufPuissance = 0, BufAgilite = 0, BufIntelligence = 0;
		float BufConstitution = 0, BufEsquive = 0, BufCharisme = 0;

	public:
		Joueur();
		virtual ~Joueur() {}

	public:
		int Gestion();
		void Gestion_Equipement();
		void Gestion_Statistiques();
		void Repos();

		int TabToAct(int TabAppui);
		void CoupCritique(Individu* ennemi);
		void BlessureGrave(Individu* ennemi);
		void CoupEsquive(Individu* ennemi);
		void GainExperience(Individu* ennemi, float Degats, int Exp = 0);
		void BlessuresMultiples(Individu* ennemi);

		bool ApplicationAmeliorations();

	//Affichage :
	public:
		void Disp(float RefX, float RefY);
};

class Paysage : public Element_Carte
{
	public:
		Classe_Paysage *Classe;

	public:
		Paysage();
		~Paysage();

	public:
		int Gestion();
		int Collision(Individu *elem, int TypeCollision);

	public:
		void Disp_Masks(float RefX, float RefY);

		void Disp(float RefX, float RefY);
};

class Paysage_Mouvant : public Element_Mouvant
{
	public:
		Classe_Paysage_Mouvant *Classe;

	//Constructeurs / Destructeurs :
	public:
		Paysage_Mouvant();
		~Paysage_Mouvant();

	//Gestion :
	public:
		void Activation();
		Activite* Get_Activite(short act);
		int Gestion();
		void IncrementNum(bool RaZ = false);
		int Collision(Individu *elem, int TypeCollision);

	//Affichage :
	public:
		void Disp(float RefX, float RefY);
		void Disp_Masks(float RefX, float RefY);
};


class Projectile : public Individu_Unique
{
	public:
		Activite Deplacement;
		int OrigineX = 0, OrigineY = 0;
		int ChampAttaque = 0;

	//Constructeurs / Destructeurs :
	public:
		Projectile() : Individu_Unique() {}

	//Gestion :
	public:
		Activite* Get_Activite(short act);
		bool Set_Activite(int nv);
		int Gestion();

	//Affichage :
	public:
		void Disp(float RefX, float RefY);
		void Disp_Masks(float RefX, float RefY);
};

class Paysage_Lanceur : public Paysage_Mouvant
{
	public:
		Projectile ProjectileLance;

	public:
		bool ActivationEnCours;

	public:
		Paysage_Lanceur();
		int Gestion();

	//Affichage :
	public:
		void Disp_Masks(float RefX, float RefY);
};


class Actionneur : public Element_Carte
{
	public:
		int Type            = 0;
		int DonneeInt       = 0;
		string DonneeString = "";
		Dialogue *dialogue  = NULL;

	public:
		Actionneur();

	//Gestion :
	public:
		int Gestion();
		int Collision(Individu* elem, int TypeCollision);
		void Load(istream &Fichier);
		void Load_Dialogue();

	//Affichage :
	public:
		void Disp(float RefX, float RefY);
		void Disp_Masks(float RefX, float RefY);
};

bool Disp_Coffre();
void Gestion_Coffre(Event&);

void Load_IndividuUnique(string, Individu_Unique*);
void Load_ClasseCommune(string);
void Load_ClassePaysage(string);
void Load_ClassePaysageMouvant(string);
void Load_PaysageLanceur(string, Paysage_Lanceur*);
void Disp_Personnage();
void Disp_MiniaturesCompetences();

#endif
