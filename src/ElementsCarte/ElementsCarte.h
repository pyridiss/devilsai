
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

#include <SFML/Graphics.hpp>

#include "tools/vector2d.h"
#include "tools/shape.h"
#include "tools/math.h"
#include "../Attributs/Attributs.h"
#include "../Bibliotheque/Constantes.h"

using namespace std;
using namespace sf;

class Carte;
class Element_Carte;
class Individu;
class Individu_Commun;
class Paysage;
class Dialog;

namespace tinyxml2{
    class XMLHandle;
};

struct Point
{
    double x;
    double y;
};

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
        tools::math::Shape size;
        tools::math::Shape viewField;
        tools::math::Shape interactionField;
		string ActDefaut         = "0";
        double lifetime         = -1;

        bool angleFixed = false;
        double fixedAngle = 0;

	public:
		unsigned int Experience = 0;

	public:
		Caracteristiques Caracs;
		list<TemplateObject> inventory;	//These objects are always put on corpses, whatever probability
		list<TemplateObject> stuff;

    public:
        string imagePrefix = "";
        string corpseImageKey = "";

		MapActivites Activites;

	public:
		Classe_Commune();
		~Classe_Commune();

	public:
		void Ajouter_Activite(string Id);
		Activite* Get_Activite(string Id);

	public:
		void Copie_Element(Individu_Commun *elem);
        void setAngleFixed(double angle);

        void loadFromXML(tinyxml2::XMLHandle &handle);
};

class Classe_Paysage
{
	public:
		string Type             = "";

        tools::math::Shape size;

		short TypeClassement    = CLASSEMENT_NORMAL;

	public:
		void Copie_Element(Paysage *elem);
};

class Classe_Paysage_Mouvant
{
	public:
		MapActivites Activites;

		string Type             = "";

        tools::math::Shape size;

		short TypeClassement    = CLASSEMENT_NORMAL;

        string imagePrefix = "";

		Caracteristiques Caracs;
		bool ActivationNecessaire	= false;
//		bool ActivationUnique;

	//Gestion :
	public:
		void Ajouter_Activite(string Id);
		Activite* Get_Activite(string Id);

	public:
		void Copie_Element(Element_Carte *elem);
};

class Element_Carte
{
	//Objet :
	public:
		int Id              = -1;
		string Type         = "DEFAUT";
		string Liste        = "DEFAUT";
		short Diplomatie    = 0;
        double lifetime     = -1;

	protected:
		short Controle      = AUCUN;

	private:
        tools::math::Vector2d _position;

    public:
        tools::math::Shape size;
        bool inert;

		short TypeClassement    = CLASSEMENT_NORMAL;

	//Constructeurs / Destructeurs :
	public:
		Element_Carte();
		virtual ~Element_Carte();

	//Getter :
	public:
		short Get_Controle();
        const tools::math::Vector2d& position();

	//Gestion :
	public:
		void Set_Controle(short nv);

		virtual int Gestion();

		virtual int Collision(Individu *elem, int TypeCollision) =0;

        void move(double x, double y);

	//Affichage :
	public:
		virtual void Disp(RenderTarget& target, float RefX, float RefY) =0;
};

class Coffre : public Element_Carte
{
	public:
		Objects objects;

		string NumeroNom = "";
		String32 Nom;

	public:
		Coffre();

	//Gestion :
	public:
		int Gestion();
		int Collision(Individu* elem, int TypeCollision);

	//Affichage :
	public:
		void Disp(RenderTarget& target, float RefX, float RefY);
};

class Cadavre : public Coffre
{
	public:
		string Ind_Id	= "";
		float Duree		= 0;
		bool Vide		= false;

        string imageKey = "";

	public:
		Cadavre();

	//Gestion :
	public:
		int Gestion();
		void Set_Individu();
        void Set_Individu(string type, string key);

	//Affichage :
	public:
		void Disp(RenderTarget& target, float RefX, float RefY);
};

class Element_Mouvant : public Element_Carte
{
	//Objet
	public:
		string Act   = "0";
        double angle = 0;
		short Num   = 0;

	public:
		string ActDefaut     = "0";
		bool ActEffectue    = true;

	public:
		float Temps         = 0;

	public:
		int Comportement        = COMPORTEMENT_ALEATOIRE;
		int NouveauComportement = COMPORTEMENT_ALEATOIRE;
		int ElementVision       = -1;

	//Constructeurs / Destructeurs :
	public:
		Element_Mouvant() : Element_Carte() {}
		virtual ~Element_Mouvant() {}

	//Getter :
	public:
		string Get_Act();
		short Get_Num();

		virtual Activite* Get_Activite(string act) =0;

	//Gestion :
	public:
		virtual int Gestion();
		virtual bool Set_Activite(string nv);
		virtual int Get_Vitesse(string act);
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
		string LieuVillage  = "village";

		float DureeCadavre  = 0.5;

		float buf_rec       = 0;

        tools::math::Shape interactionField;
        tools::math::Shape viewField;

	//Constructeurs / Destructeurs :
	public:
		Individu();
		virtual ~Individu() {}

	//Getters :
	public:
		virtual String32& Get_Nom() = 0;
		virtual int Get_Experience() = 0;
		virtual Caracteristiques* Get_Caracs() =0;
		virtual Statistiques* Get_Stats() =0;
        virtual bool angleFixed() =0;

	//Gestion :
	public:
		int Gestion();
		void MouvementAleatoire(int Iteration);
		bool MouvementChasse(Element_Carte *elem);
		virtual void Gestion_Recuperation();

	//Getter stats and characteristics
	public:
		virtual float get(string field);

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
		void Disp(RenderTarget& target, float RefX, float RefY);
        void displayLifeGauge(int x, int y);
};

class Individu_Unique : public Individu
{
	public:
		String32 Nom;
		unsigned int Experience = 0;
		bool RecuperationFixe   = false;
		bool EnergieMax         = false;

    public:
        string imagePrefix = "";
        string corpseImageKey = "";

	public:
		MapActivites Activites;
// 		MapPointeursCompetences Competences;

		Caracteristiques Caracs;

//		MapEquipement Equips;
//		mapObjects Inventaire;
		ListEmplacementsEquipements EmplacementsEquip;

	public:
        lua_State* skillLinks[4]    = {nullptr, nullptr, nullptr, nullptr};

	//Constructeurs / Destructeurs :
	public:
		Individu_Unique();
		virtual ~Individu_Unique();

	public:
		void Ajouter_Activite(string Id);
		void Ajouter_EmplacementEquip(int x, int y, int w, int h, string Categorie, string Type, string Classe);

	//Getter :
	public:
		String32& Get_Nom();
		int Get_Experience();
		Caracteristiques* Get_Caracs();
		Statistiques* Get_Stats();
		virtual Activite* Get_Activite(string act);
        bool angleFixed();

		float get(string field);

		int Get_Vitesse(string act);

		void Gestion_Recuperation();
		void Lag_Recuperation(float lag);

		bool Set_Activite(string nv);
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
		int Get_Experience();
		Caracteristiques* Get_Caracs();
		Statistiques* Get_Stats();
		Activite* Get_Activite(string act);
        bool angleFixed();

		bool Set_Activite(string nv);
};

class Joueur : public Individu_Unique
{
	public:
		float DureeEveil               = 0;
		int ElementInteraction         = -1;
		int SauvegardeIndiceLieu       = 0;
// 		bool GestionCoffresActivee     = false;
// 		bool GestionCompetencesActivee = false;
        bool automove                  = false;
        Point automoveEndpoint;

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

		string TabToAct(int TabAppui);
		void CoupCritique(Individu* ennemi);
		void BlessureGrave(Individu* ennemi);
		void CoupEsquive(Individu* ennemi);
		void GainExperience(Individu* ennemi, float Degats, int Exp = 0);
		void BlessuresMultiples(Individu* ennemi);

		bool ApplicationAmeliorations();
};

class Paysage : public Element_Carte
{
	public:
		Classe_Paysage *Classe;
		int repeatX = 1;
		int repeatY = 1;

	public:
		Paysage();
		~Paysage();

	public:
		int Gestion();
		int Collision(Individu *elem, int TypeCollision);
		void calculateCollisionRadius();

	public:
		void Disp(RenderTarget& target, float RefX, float RefY);
};

class Door : public Element_Carte
{
	//Doors are open for some Individu's and closed for others
	public:
		int deniedDiplomacy;

	public:
		Door();
		~Door();

	public:
		int Gestion();
		int Collision(Individu *elem, int TypeCollision);

	public:
		void Disp(RenderTarget& target, float RefX, float RefY);
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
		Activite* Get_Activite(string act);
		int Gestion();
		void IncrementNum(bool RaZ = false);
		int Collision(Individu *elem, int TypeCollision);

	//Affichage :
	public:
		void Disp(RenderTarget& target, float RefX, float RefY);
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
		Activite* Get_Activite(string act);
		bool Set_Activite(string nv);
		int Gestion();

	//Affichage :
	public:
		void Disp(RenderTarget& target, float RefX, float RefY);
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
};


class Actionneur : public Element_Carte
{
	public:
		int Type            = 0;
		int DonneeInt       = 0;
		string DonneeString = "";
		Dialog *dialogue    = NULL;

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
		void Disp(RenderTarget& target, float RefX, float RefY);
};

class Trigger : public Element_Carte
{
	//Triggers can be activated by the player
	public:
		string triggerFunction = "";

	public:
		Trigger();
		~Trigger();

	public:
		int Gestion();
		int Collision(Individu *elem, int TypeCollision);
		void load(istream& Fichier, Carte* carte);
		bool activated();

	public:
		void Disp(RenderTarget& target, float RefX, float RefY);
};

void Disp_Coffre();
void Gestion_Coffre(Event&);

void Load_IndividuUnique(string, Individu_Unique*);
void Load_ClasseCommune(string);
void Load_ClassePaysage(string);
void Load_ClassePaysageMouvant(string);
void Load_PaysageLanceur(string, Paysage_Lanceur*);
void Disp_Personnage();
void Disp_MiniaturesCompetences();

#endif
