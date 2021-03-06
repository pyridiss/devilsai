
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

#include "tools/variant.h"
#include "tools/vector2d.h"
#include "tools/shape.h"
#include "textManager/plainText.h"

#include "devilsai-resources/skill.h"
#include "devilsai-resources/stats.h"
#include "devilsai-resources/pack.h"

#define DIPLOM_NEUTRE       0
#define DIPLOM_ALLIE        1
#define DIPLOM_ENNEMI       2
#define DIPLOM_BREAKABLE    3

#define CLASSEMENT_SOL      0
#define CLASSEMENT_CADAVRE  1
#define CLASSEMENT_NORMAL   2
#define CLASSEMENT_HAUT     3
#define CLASSEMENT_NUAGE    4

#define COLL_OK             0
#define COLL_END            1
#define COLL_PRIM           2
#define COLL_PRIM_MVT       3
#define COLL_INTER          4
#define COLL_ATT            6
#define COLL_LIFE           7
#define COLL_VIS            8

#define ETAT_NORMAL         6
#define ETAT_MORT           8


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
        string Type;
        string definitionFile;
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

        unordered_map<string, Skill*> _skills;

	public:
        Classe_Commune(string id);
		~Classe_Commune();

	public:
        void Copie_Element(Individu *elem);
        void setAngleFixed(double angle);

        void loadFromXML(tinyxml2::XMLHandle &handle, const string& file);

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
		int Collision(Individu* elem, bool apply);
        void loadFromXML(tinyxml2::XMLHandle &handle);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);
        bool empty();
        void close();
        void Set_Individu(string species, string key);

	//Affichage :
	public:
		void Disp(RenderTarget& target);
        void highlight();
};

class Individu : public Element_Carte
{
    public:
        struct SkillProperties
        {
            Skill* skill;
            double unavailability;
            unsigned level;

            SkillProperties(Skill* s)
              : skill(s),
                unavailability(0),
                level(1)
            {
            }

            Skill* operator->()
            {
                return skill;
            }
        };
        struct SkillAccess
        {
            private:
                SkillProperties* p;

            public:
                SkillAccess()
                  : p(nullptr)
                {
                }

                SkillAccess(SkillProperties* s)
                  : p(s)
                {
                }

                Skill* operator->() const
                {
                    return p->skill;
                }

                bool operator==(const SkillAccess& right)
                {
                    return (p == right.p);
                }

                bool operator!=(const SkillAccess& right)
                {
                    return (p != right.p);
                }

                double& unavailability() const
                {
                    return p->unavailability;
                }

                unsigned& level() const
                {
                    return p->level;
                }

                double extraStats(Attribute a) const
                {
                    if (level() == 0) return 0;
                    return p->skill->levels[level()-1].extraStats[a];
                }

                double extraStatsAmplifiers(Attribute a) const
                {
                    if (level() == 0) return 0;
                    return p->skill->levels[level()-1].extraStatsAmplifiers[a];
                }

                double maxUnavailability() const
                {
                    if (level() == 0) return 0;
                    return p->skill->levels[level()-1].unavailability;
                }

                bool none() const
                {
                    return (p == nullptr || level() == 0);
                }
        };

    //Objet
	public:
        double angle = 0;

	public:
		bool ActEffectue    = true;

	public:
        Behaviors Comportement = Behaviors::Random;
        Behaviors NouveauComportement = Behaviors::Random;

	protected:
        double _animationFrame;
        double _timer;
        Element_Carte* _targetedItem;
        SkillAccess _currentSkill;
        Stats _currentHealthStatus;
        Stats _attributes;
        string* _behaviors;
        vector<string>* _attacks;
        unordered_map<string, SkillProperties> _skills;
        Classe_Commune* _species;
        textManager::PlainText* _displayedName;
        string* _corpseImageKey;
        unsigned int _experience = 0;
        string* _extraDataFile;
        Clock _clock;
        tools::math::Variant<int, Individu*> _owner;

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
        Individu(Individu&& other) noexcept;
        Individu(Classe_Commune* s, double x, double y);
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
        virtual bool interact(Element_Carte* item);
        Stats& attributes();
        SkillAccess skill(const string& s);
        const textManager::PlainText& displayedName();
        unsigned int experience();
        bool angleFixed();

	//Gestion :
	public:
		int Gestion();
        void MouvementAleatoire(bool newDirection);
        void findPath(const tools::math::Vector2d& destination, int nodesNumber, bool reduceCollisionWithIndividuals);
        bool MouvementChasse(Element_Carte *elem, int nodesNumber, bool reduceCollisionWithIndividuals);
        void Gestion_Recuperation();
        bool Set_Activite(const string& nv);
        bool Set_Activite(Behaviors b);
        void fight(Individu *enemy);
		int Collision(Individu *elem, bool apply);
        void otherItemDeleted(Element_Carte* other);
        void nextAnimationFrame();
        void updateAngle(const tools::math::Vector2d& p);
        void gainExperience(int exp);
        void setOwner(Individu* o);
        Individu* owner();
        virtual void improveAttribute(Attribute a, int chance, Individu* enemy);

	public:
        int currentHealthStatus(Attribute a, bool forceUpdate = false);
        void setHealthStatus(Attribute a, double value);
        virtual void modifyHealthStatus(Attribute a, double value);
        Classe_Commune* species();
        void setCustomDisplayedName(const textManager::PlainText& newName);

    public:
        void loadFromXML(tinyxml2::XMLHandle &handle);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);

	//Affichage
	public:
		void Disp(RenderTarget& target);
        void displayLifeGauge(RenderTarget& target);

    friend class Classe_Commune;
};

class Joueur : public Individu
{
    private:
        Stats _displayedAttributes;
        bool _automove = false;
        tools::math::Vector2d _automoveEndpoint;
        bool _hunting = false;
        Individu* _hunted;
        Individu _fakeIndividual;
        string _skillForHunted;
        double _wakingTime;
        bool _tired;

	public:
        Element_Carte* selectedIndividual;

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
        bool interact(Element_Carte* item);

        void improveAttribute(Attribute a, int chance, Individu* enemy);
        void resetDisplayedAttributes();

        void automove(const tools::math::Vector2d& p);
        void hunt(Element_Carte* item, const string& skill, bool force);
        void stopAutomoving();
        void stopHunting();

        void loadFromXML(tinyxml2::XMLHandle &handle);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);

        void displayHealthStatus(RenderTarget& target, int x, int y);

        textManager::PlainText characterDescription();
};

class Paysage : public Element_Carte
{
	public:
        Vector2i extent;

	public:
		Paysage();
        Paysage(string id);
        Paysage(const Paysage& other) = default;
        Paysage(Paysage&& other) noexcept = default;
        Paysage& operator=(const Paysage& right) = delete;
        Paysage& operator=(Paysage&& right) = delete;
        ~Paysage() = default;

	public:
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
		int Collision(Individu *elem, bool apply);

        void loadFromXML(tinyxml2::XMLHandle &handle);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);

	public:
		void Disp(RenderTarget& target);
};

#endif
