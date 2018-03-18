
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
#include <unordered_map>

#include <SFML/System.hpp>

#include "tools/vector2d.h"
#include "tools/shape.h"
#include "textManager/plainText.h"

#include "devilsai-resources/skill.h"
#include "devilsai-resources/stats.h"
#include "devilsai-resources/pack.h"
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
}

enum Behaviors
{
    Random,
    Blocked,
    Defense,
    Hunting,
    Attacking,
    LookingForLife,
    Hurt,
    Dying,
    enumSize
};

/** INDIVIDUS **/

class Classe_Commune
{
	public:
        textManager::PlainText _displayedName;
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
        Stats commonStats;
        Pack inventory;

    public:
        string corpseImageKey = "";

        unordered_map<string, Skill> _skills;

	public:
		Classe_Commune();
		~Classe_Commune();

	public:
        Skill* Ajouter_Activite(string Id);

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
        Element_Carte(const Element_Carte& other);
        Element_Carte(Element_Carte&& other) noexcept;
        Element_Carte& operator=(const Element_Carte& right) = delete;
        Element_Carte& operator=(Element_Carte&& right) = delete;
        virtual ~Element_Carte() = default;

	//Getter :
	public:
        const tools::math::Vector2d& position() const;

	//Gestion :
	public:
		virtual int Gestion();

		virtual int Collision(Individu *elem, bool apply) =0;
        virtual void otherItemDeleted(Element_Carte* other);

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
        Pack objects;

		string NumeroNom = "";
        textManager::PlainText Nom;

    private:
        bool _highlight;
        bool _closeWhenEmpty;

	public:
		Coffre();
        Coffre(const Coffre& other) = default;
        Coffre(Coffre&& other) noexcept = default;
        Coffre& operator=(const Coffre& right) = delete;
        Coffre& operator=(Coffre&& right) = delete;
        ~Coffre() = default;

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

	public:
		bool ActEffectue    = true;

	public:
		float Temps         = 0;

	public:
        Behaviors Comportement = Behaviors::Random;
        Behaviors NouveauComportement = Behaviors::Random;

	protected:
        unsigned _animationFrame;
        Element_Carte* _targetedItem;
        Skill* _currentSkill;
        Stats _currentHealthStatus;
        Stats _attributes;
        string* _behaviors;
        vector<string>* _attacks;
        unordered_map<string, Skill>* _skills;
        Classe_Commune* _species;
        textManager::PlainText* _displayedName;
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

        Pack inventory;

	//Constructeurs / Destructeurs :
	public:
		Individu();
        Individu(const Individu& other);
        Individu(Individu&& right) noexcept;
        Individu& operator=(const Individu& right) = delete;
        Individu& operator=(Individu&& right) = delete;
        virtual ~Individu();

	//Getters :
    protected:
        string& behavior(Behaviors b);
        vector<string>& attacks();
        void createCustomBehaviors();
        string& corpseImageKey();
        void createCorpse();

	public:
        Element_Carte* targetedItem();
        Stats& attributes();
        Skill* skill(const string& s);
        const textManager::PlainText& displayedName();
        unsigned int experience();
        bool angleFixed();

	//Gestion :
	public:
		int Gestion();
        int GestionElementMouvant();
        void MouvementAleatoire(bool newDirection);
        void findPath(const tools::math::Vector2d& destination, int nodesNumber, bool reduceCollisionWithIndividuals);
        bool MouvementChasse(Element_Carte *elem, int nodesNumber, bool reduceCollisionWithIndividuals);
		virtual void Gestion_Recuperation();
        bool Set_Activite(const string& nv);
        bool Set_Activite(Behaviors b);
		int Collision(Individu *elem, bool apply);
        void otherItemDeleted(Element_Carte* other);
        void nextAnimationFrame(bool RaZ = false);
        void updateAngle(const tools::math::Vector2d& p);
        void GainExperience(Individu* ennemi, float Degats, int Exp = 0);

	public:
        int currentHealthStatus(Attribute a, bool forceUpdate = false);
        void setHealthStatus(Attribute a, double value);
        virtual void modifyHealthStatus(Attribute a, double value);
        void setSpecies(Classe_Commune* s);
        Classe_Commune* species();
        Skill* createSkill(string skillName);
        void setCustomDisplayedName(const textManager::PlainText& newName);

    public:
        void loadFromXML(tinyxml2::XMLHandle &handle);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);

	//Affichage
	public:
		void Disp(RenderTarget& target);
        void displayLifeGauge(RenderTarget& target);

    friend class Classe_Commune;
    friend void Combat(Individu *Attaquant, Individu *Blesse, string skill);
};

class Joueur : public Individu
{
    private:
        bool _automove = false;
        tools::math::Vector2d _automoveEndpoint;
        bool _hunting = false;
        Individu* _hunted;
        Individu _fakeIndividual;
        string _skillForHunted;

	public:
		float DureeEveil               = 0;
        Element_Carte* selectedIndividual;

    private:
        Stats _displayedAttributes;

	public:
		Joueur();
        Joueur(const Joueur& other) = delete;
        Joueur(Joueur&& right) = delete;
        Joueur& operator=(const Joueur& right) = delete;
        Joueur& operator=(Joueur&& right) = delete;
        ~Joueur() = default;

	public:
		int Gestion();
		void Gestion_Statistiques();
		void Repos();
        void otherItemDeleted(Element_Carte* other);

		void CoupCritique(Individu* ennemi);
		void BlessureGrave(Individu* ennemi);
		void CoupEsquive(Individu* ennemi);
		void BlessuresMultiples(Individu* ennemi);

		bool ApplicationAmeliorations();
        void resetDisplayedAttributes();

        void automove(const tools::math::Vector2d& p);
        void hunt(Element_Carte* item, const string& skill, bool force);
        void stopAutomoving();
        void stopHunting();

        void loadFromXML(tinyxml2::XMLHandle &handle);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);

        textManager::PlainText characterDescription();
};

class Paysage : public Element_Carte
{
	public:
        Vector2i extent;

	public:
		Paysage();
        Paysage(const Paysage& other) = default;
        Paysage(Paysage&& other) = default;
        Paysage& operator=(const Paysage& right) = delete;
        Paysage& operator=(Paysage&& right) = delete;
        ~Paysage() = default;

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
        CheckPoint(const CheckPoint& other) = default;
        CheckPoint(CheckPoint&& other) noexcept = default;
        CheckPoint& operator=(const CheckPoint& right) = delete;
        CheckPoint& operator=(CheckPoint&& right) = delete;
        ~CheckPoint() = default;

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
        Trigger(const Trigger& other);
        Trigger(Trigger&& other) noexcept = default;
        Trigger& operator=(const Trigger& right) = delete;
        Trigger& operator=(Trigger&& right) = delete;
        ~Trigger() = default;

	public:
		int Gestion();
		int Collision(Individu *elem, bool apply);

        void loadFromXML(tinyxml2::XMLHandle &handle);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);

	public:
		void Disp(RenderTarget& target);
};

#endif
