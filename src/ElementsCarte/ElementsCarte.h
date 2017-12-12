
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
class Paysage;

namespace tinyxml2{
    class XMLHandle;
    class XMLDocument;
};

enum Behaviors
{
    Random,
    Blocked,
    Defense,
    Hunting,
    Attacking,
    REGEN,
    Hurt,
    Dying,
    enumSize
};

/** INDIVIDUS **/

class Classe_Commune
{
	public:
        String32 _displayedName;
		string Type             = "";
		int Diplomatie          = 0;
        tools::math::Shape size;
        tools::math::Shape viewField;
        tools::math::Shape interactionField;
        string _behaviors[Behaviors::enumSize];
        vector<string> _attacks;
        double lifetime         = -1;

        bool angleFixed = false;
        double fixedAngle = 0;
        unsigned int _experience = 0;

	public:
        Statistiques commonStats;
        Objects inventory;

    public:
        string corpseImageKey = "";

        MapActivites _skills;

	public:
		Classe_Commune();
		~Classe_Commune();

	public:
        Activite* Ajouter_Activite(string Id);

	public:
        void Copie_Element(Individu *elem);
        void setAngleFixed(double angle);

        void loadFromXML(tinyxml2::XMLHandle &handle);

    friend class Individu;
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
        const tools::math::Vector2d& position() const;

	//Gestion :
	public:
		virtual int Gestion();

		virtual int Collision(Individu *elem, bool apply) =0;

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
		int Collision(Individu* elem, bool apply);
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
        double angle = 0;
		short Num   = 0;

	public:
		bool ActEffectue    = true;

	public:
		float Temps         = 0;

	public:
        Behaviors Comportement = Behaviors::Random;
        Behaviors NouveauComportement = Behaviors::Random;
		int ElementVision       = -1;

	protected:
        Activite* _currentSkill;
        Statistiques _currentHealthStatus;
        Statistiques _attributes;
        string* _behaviors;
        vector<string>* _attacks;
        MapActivites* _skills;
        Classe_Commune* _species;
        String32* _displayedName;
        string* _corpseImageKey;
        unsigned int _experience = 0;
        string* _extraDataFile;
        Clock _clock;

	public:
        bool RecuperationFixe   = false;
        bool EnergieMax         = false;

        tools::math::Shape interactionField;
        tools::math::Shape viewField;
        vector<pair<Element_Carte*, int>> seenItems;

        tools::math::Shape pathToTarget;
        tools::math::Shape validityOfPath;

        Objects inventory;

	//Constructeurs / Destructeurs :
	public:
		Individu();
        virtual ~Individu();

	//Getters :
    protected:
        string& behavior(Behaviors b);
        vector<string>& attacks();
        void createCustomBehaviors();
        MapActivites& skills();
        string& corpseImageKey();
        void createCorpse();

	public:
        Statistiques& attributes();
        Activite* skill(const string& s);
        const String32& displayedName();
        unsigned int experience();
        bool angleFixed();
		short Get_Num();

	//Gestion :
	public:
		int Gestion();
        int GestionElementMouvant();
        void MouvementAleatoire(bool newDirection);
        void findPath(const tools::math::Vector2d& destination);
		bool MouvementChasse(Element_Carte *elem);
		virtual void Gestion_Recuperation();
        bool Set_Activite(string nv);
		int Collision(Individu *elem, bool apply);
		void IncrementNum(bool RaZ = false);
        void updateAngle(const tools::math::Vector2d& p);
        void GainExperience(Individu* ennemi, float Degats, int Exp = 0);

	public:
        int currentHealthStatus(Attribute a, bool forceUpdate = false);
        void setHealthStatus(Attribute a, double value);
        virtual void modifyHealthStatus(Attribute a, double value);
        void setSpecies(Classe_Commune* s);
        Classe_Commune* species();
        Activite* createSkill(string Id);
        void setCustomDisplayedName(const String32& newName);

    public:
        void loadFromXML(tinyxml2::XMLHandle &handle);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);

	//Affichage
	public:
		void Disp(RenderTarget& target);
        void displayLifeGauge(RenderTarget& target);

    friend class Classe_Commune;
    friend void Combat(Individu *Attaquant, Individu *Blesse, lua_State *L);
};

class Joueur : public Individu
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
        Statistiques _displayedAttributes;

	public:
		Joueur();
		virtual ~Joueur() {}

	public:
		int Gestion();
		void Gestion_Statistiques();
		void Repos();

		void CoupCritique(Individu* ennemi);
		void BlessureGrave(Individu* ennemi);
		void CoupEsquive(Individu* ennemi);
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
		int Collision(Individu *elem, bool apply);

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
		int Collision(Individu* elem, bool apply);

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
		int Collision(Individu *elem, bool apply);

        void loadFromXML(tinyxml2::XMLHandle &handle);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);

	public:
		void Disp(RenderTarget& target);
};

void Disp_Personnage();

#endif
