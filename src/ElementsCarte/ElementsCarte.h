
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

namespace tinyxml2{
    class XMLHandle;
    class XMLDocument;
};

/** INDIVIDUS **/

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
        Objects inventory;

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

class Element_Carte
{
	//Objet :
	public:
		int Id              = -1;
		string Type         = "DEFAUT";
		string Liste        = "DEFAUT";
        int Diplomatie    = 0;
        double lifetime     = -1;

	private:
        tools::math::Vector2d _position;

    public:
        tools::math::Shape size;
        bool inert;
        bool ignoreCollision = false;

		int TypeClassement    = CLASSEMENT_NORMAL;

	//Constructeurs / Destructeurs :
	public:
		Element_Carte();
		virtual ~Element_Carte();

	//Getter :
	public:
        const tools::math::Vector2d& position();

	//Gestion :
	public:
		virtual int Gestion();

		virtual int Collision(Individu *elem, int TypeCollision) =0;

        void move(double x, double y);

        virtual void loadFromXML(tinyxml2::XMLHandle &handle) =0;
        virtual void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle) =0;

	//Affichage :
	public:
		virtual void Disp(RenderTarget& target) =0;
};

class Coffre : public Element_Carte
{
	public:
        string imageContainer;
		Objects objects;

		string NumeroNom = "";
		String32 Nom;

    private:
        bool _highlight;
        bool _closeWhenEmpty;

	public:
		Coffre();

	//Gestion :
	public:
		int Gestion();
		int Collision(Individu* elem, int TypeCollision);
        void loadFromXML(tinyxml2::XMLHandle &handle);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);
        bool empty();
        void close();
        void Set_Individu(string species, string key);

	//Affichage :
	public:
		void Disp(RenderTarget& target);
        void highlight(RenderTarget& target);
};

class Individu : public Element_Carte
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

	protected:
		Statistiques Stats;
        Caracteristiques _currentHealthStatus;
        Clock _clock;

	public:
        bool RecuperationFixe   = false;
        bool EnergieMax         = false;

        tools::math::Shape interactionField;
        tools::math::Shape viewField;

        Objects inventory;

	//Constructeurs / Destructeurs :
	public:
		Individu();
		virtual ~Individu() {}

	//Getters :
	public:
		virtual String32& Get_Nom() = 0;
		virtual int Get_Experience() = 0;
		virtual Caracteristiques& attributes() =0;
        virtual bool angleFixed() =0;
        string Get_Act();
		short Get_Num();
		virtual Activite* Get_Activite(string act) =0;

	//Gestion :
	public:
		int Gestion();
        int GestionElementMouvant();
		void MouvementAleatoire(int Iteration);
		bool MouvementChasse(Element_Carte *elem);
		virtual void Gestion_Recuperation();
		virtual bool Set_Activite(string nv);
        int Get_Vitesse(const string& act);
		int Collision(Individu *elem, int TypeCollision);
		void IncrementNum(bool RaZ = false);
        void updateAngle(const tools::math::Vector2d& p);

	public:
        int currentHealthStatus(Statistiques::Attribute a);
        int currentHealthStatus(Caracteristiques::Attribute a, bool forceUpdate = false);
        void setHealthStatus(Statistiques::Attribute a, double value);
        virtual void modifyHealthStatus(Statistiques::Attribute a, double value);

	//Affichage
	public:
		void Disp(RenderTarget& target);
        void displayLifeGauge(RenderTarget& target);
};

class Individu_Unique : public Individu
{
	public:
		String32 Nom;
		unsigned int Experience = 0;
        string dataFile;

    public:
        string imagePrefix = "";
        string corpseImageKey = "";

	public:
		MapActivites Activites;

		Caracteristiques Caracs;

	//Constructeurs / Destructeurs :
	public:
		Individu_Unique();
		virtual ~Individu_Unique();

	public:
		void Ajouter_Activite(string Id);

	//Getter :
	public:
		String32& Get_Nom();
		int Get_Experience();
		Caracteristiques& attributes();
		virtual Activite* Get_Activite(string act);
        bool angleFixed();

        void modifyHealthStatus(Statistiques::Attribute a, double value);

		bool Set_Activite(string nv);

        void loadFromXML(tinyxml2::XMLHandle &handle);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);
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
		Caracteristiques& attributes();
		Activite* Get_Activite(string act);
        bool angleFixed();

		bool Set_Activite(string nv);

        void loadFromXML(tinyxml2::XMLHandle &handle);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);
};

class Joueur : public Individu_Unique
{
    private:
        bool _automove = false;
        tools::math::Vector2d _automoveEndpoint;
        bool _hunting = false;
        int _hunted;
        string _skillForHunted;

	public:
		float DureeEveil               = 0;
		int ElementInteraction         = -1;
        int selectedIndividual = -1;

    private:
        Caracteristiques _displayedAttributes;

	public:
		Joueur();
		virtual ~Joueur() {}

	public:
		int Gestion();
		void Gestion_Statistiques();
		void Repos();

		string TabToAct(int TabAppui);
		void CoupCritique(Individu* ennemi);
		void BlessureGrave(Individu* ennemi);
		void CoupEsquive(Individu* ennemi);
		void GainExperience(Individu* ennemi, float Degats, int Exp = 0);
		void BlessuresMultiples(Individu* ennemi);

		bool ApplicationAmeliorations();
        void resetDisplayedAttributes();

        void automove(const tools::math::Vector2d& p);
        void hunt(int id, const string& skill);
        void stopAutomoving();
        void stopHunting();

        void loadFromXML(tinyxml2::XMLHandle &handle);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);
};

class Paysage : public Element_Carte
{
	public:
        Vector2i extent;

	public:
		Paysage();
		~Paysage();

	public:
		int Gestion();
		int Collision(Individu *elem, int TypeCollision);

        void loadFromXML(tinyxml2::XMLHandle &handle);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);

	public:
		void Disp(RenderTarget& target);
};

class CheckPoint : public Element_Carte
{
    public:
        CheckPoint();

		int Gestion();
		int Collision(Individu* elem, int TypeCollision);

        void loadFromXML(tinyxml2::XMLHandle &handle);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);

		void Disp(RenderTarget& target);
};

class Trigger : public Element_Carte
{
	public:
        string triggerScript;
        string data;
        lua_State* script;

	public:
		Trigger();
		~Trigger();

	public:
		int Gestion();
		int Collision(Individu *elem, int TypeCollision);

        void loadFromXML(tinyxml2::XMLHandle &handle);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);

	public:
		void Disp(RenderTarget& target);
};

void Disp_Personnage();

#endif
