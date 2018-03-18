
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

#include <cmath>

#include "../Bibliotheque/Constantes.h"
#include "ElementsCarte.h"

#include "tools/debug.h"
#include "tools/timeManager.h"
#include "tools/math.h"
#include "tools/aStar.h"
#include "imageManager/imageManager.h"
#include "textManager/richText.h"
#include "textManager/textManager.h"

#include "devilsai-resources/Carte.h"

#include "devilsai-gui/console.h"

#include "Jeu/options.h"
#include "gamedata.h"


Individu::Individu()
    : Element_Carte(),
    _animationFrame(0),
    _targetedItem(nullptr),
    _currentSkill(nullptr),
    _currentHealthStatus(),
    _attributes(),
    _behaviors(nullptr),
    _attacks(nullptr),
    _skills(nullptr),
    _species(nullptr),
    _displayedName(nullptr),
    _corpseImageKey(nullptr),
    _extraDataFile(nullptr),
    _clock(),
    interactionField(),
    viewField(),
    seenItems(),
    pathToTarget(),
    validityOfPath(),
    inventory()
{
	TypeClassement = CLASSEMENT_NORMAL;
    angle = tools::math::randomNumber(0.0, 2 * M_PI);
    inert = false;
    interactionField.setOrigin(&position());
    viewField.setOrigin(&position());
    pathToTarget.profile = tools::math::Shape::Profiles::None;
    validityOfPath.circle(tools::math::Vector2d(0, 0), 20);
    validityOfPath.setOrigin(tools::math::absoluteOrigin());
}

Individu::Individu(const Individu& other)
  : Element_Carte(other),
    angle(other.angle),
    ActEffectue(other.ActEffectue),
    Temps(other.Temps),
    Comportement(other.Comportement),
    NouveauComportement(other.NouveauComportement),
    _animationFrame(other._animationFrame),
    _targetedItem(other._targetedItem),
    _currentSkill(other._currentSkill),
    _currentHealthStatus(other._currentHealthStatus),
    _attributes(other._attributes),
    _behaviors(nullptr),
    _attacks(nullptr),
    _skills(nullptr),
    _species(other._species),
    _displayedName(nullptr),
    _corpseImageKey(nullptr),
    _experience(other._experience),
    _extraDataFile(nullptr),
    _clock(other._clock),
    RecuperationFixe(other.RecuperationFixe),
    EnergieMax(other.EnergieMax),
    interactionField(other.interactionField),
    viewField(other.viewField),
    seenItems(other.seenItems),
    pathToTarget(other.pathToTarget),
    validityOfPath(other.validityOfPath),
    inventory(other.inventory)
{
    if (other._behaviors != nullptr)
    {
        _behaviors = new string[Behaviors::enumSize];
        for (unsigned i = 0 ; i < Behaviors::enumSize ; ++i)
            _behaviors[i] = other._behaviors[i];
    }
    if (other._attacks != nullptr)
    {
        _attacks = new vector<string>;
        *_attacks = *(other._attacks);
    }
    if (other._skills != nullptr)
    {
        _skills = new unordered_map<string, Skill>;
        *_skills = *(other._skills);
    }
    if (other._displayedName != nullptr)
    {
        _displayedName = new textManager::PlainText;
        *_displayedName = *(other._displayedName);
    }
    if (other._corpseImageKey != nullptr)
    {
        _corpseImageKey = new string;
        *_corpseImageKey = *(other._corpseImageKey);
    }
    if (other._extraDataFile != nullptr)
    {
        _extraDataFile = new string;
        *_extraDataFile = *(other._extraDataFile);
    }

    interactionField.setOrigin(&position());
    viewField.setOrigin(&position());
    validityOfPath.setOrigin(tools::math::absoluteOrigin());
}

Individu::Individu(Individu&& other) noexcept
  : Element_Carte(std::move(other)),
    angle(other.angle),
    ActEffectue(other.ActEffectue),
    Temps(other.Temps),
    Comportement(other.Comportement),
    NouveauComportement(other.NouveauComportement),
    _animationFrame(other._animationFrame),
    _targetedItem(other._targetedItem),
    _currentSkill(other._currentSkill),
    _currentHealthStatus(std::move(other._currentHealthStatus)),
    _attributes(std::move(other._attributes)),
    _behaviors(other._behaviors),
    _attacks(other._attacks),
    _skills(other._skills),
    _species(other._species),
    _displayedName(other._displayedName),
    _corpseImageKey(other._corpseImageKey),
    _experience(other._experience),
    _extraDataFile(other._extraDataFile),
    _clock(std::move(other._clock)),
    RecuperationFixe(other.RecuperationFixe),
    EnergieMax(other.EnergieMax),
    interactionField(std::move(other.interactionField)),
    viewField(std::move(other.viewField)),
    seenItems(std::move(other.seenItems)),
    pathToTarget(std::move(other.pathToTarget)),
    validityOfPath(std::move(other.validityOfPath)),
    inventory(std::move(other.inventory))
{
    other._behaviors = nullptr;
    other._attacks = nullptr;
    other._skills = nullptr;
    other._displayedName = nullptr;
    other._corpseImageKey = nullptr;
    other._extraDataFile = nullptr;
}

Individu::~Individu()
{
    if (_behaviors != nullptr)
        delete[] _behaviors;

    if (_attacks != nullptr)
        delete _attacks;

    if (_skills != nullptr)
        delete _skills;

    if (_displayedName != nullptr)
        delete _displayedName;

    if (_corpseImageKey != nullptr)
        delete _corpseImageKey;

    if (_extraDataFile != nullptr)
        delete _extraDataFile;
}

bool Individu::Set_Activite(const string& nv)
{
    if (skill(nv) == nullptr) return false;

    if (_currentSkill == nullptr)
    {
        _currentSkill = skill(nv);
        _currentSkill->atBegin(this);
        _animationFrame = 0;
    }

    if (_currentSkill->priority > skill(nv)->priority && (!ActEffectue || _animationFrame != 0)) return false;

    if (_currentSkill->Id == nv) return true;

    _currentSkill = skill(nv);
    _currentSkill->atBegin(this);

    if (_currentSkill->Id == behavior(Behaviors::Dying))
    {
        setHealthStatus(Life, 0);
        setHealthStatus(Energy, 0);
        setHealthStatus(Healing, 0);
        Diplomatie = DIPLOM_NEUTRE;
    }

    if (nv == behavior(Behaviors::Dying)) size.circle(tools::math::Vector2d(0, 0), 0);

    if (_currentSkill->priority > 0) nextAnimationFrame(true);
    return true;
}

bool Individu::Set_Activite(Behaviors b)
{
    return Set_Activite(behavior(b));
}

int Individu::Collision(Individu *elem, bool apply)
{
    return COLL_PRIM_MVT;
}

void Individu::nextAnimationFrame(bool RaZ)
{
    if (_currentSkill->Id == behavior(Behaviors::Dying) && _animationFrame == _currentSkill->numberOfImages-1) return;

    if (RaZ)
    {
        _animationFrame = 0;
        ActEffectue = false;
    }
    else
    {
        ++_animationFrame;
        if (_animationFrame == _currentSkill->numberOfImages) _animationFrame = 0;
        ActEffectue = true;
    }
}

int Individu::GestionElementMouvant()
{
    if (_currentSkill == nullptr) return 0;

    double speed = _currentSkill->speed + currentHealthStatus(_currentSkill->speedImprover);
    if (Temps < 1.0/speed)
    {
        Temps += tools::timeManager::I(1/60.);
        return ETAT_NORMAL;
    }
    else Temps = tools::timeManager::I(1/60.);

    return ETAT_CONTINUER;
}

void Individu::otherItemDeleted(Element_Carte* other)
{
    if (_targetedItem == other)
        _targetedItem = nullptr;
}

Element_Carte* Individu::targetedItem()
{
    return _targetedItem;
}

string& Individu::behavior(Behaviors b)
{
    if (_behaviors != nullptr)
        return _behaviors[b];

    if (_species != nullptr)
        return _species->_behaviors[b];

    //In case there is no custom behaviors nor species...
    createCustomBehaviors();
    return _behaviors[b];
}

void Individu::createCustomBehaviors()
{
    _behaviors = new string[Behaviors::enumSize];
}

vector<string>& Individu::attacks()
{
    if (_attacks != nullptr)
        return *_attacks;

    if (_species != nullptr)
        return _species->_attacks;

    //In case there is no custom attacks nor species...
    _attacks = new vector<string>;
    return *_attacks;
}

string& Individu::corpseImageKey()
{
    if (_corpseImageKey != nullptr)
        return *_corpseImageKey;

    if (_species != nullptr)
        return _species->corpseImageKey;

    //In case there is no custom corpse image key nor species...
    _corpseImageKey = new string;
    return *_corpseImageKey;
}

void Individu::createCorpse()
{
    int key = 1;

    Coffre *corpse = gamedata::currentWorld()->AjouterCoffre("storage-boxes", position().x, position().y);
    corpse->Set_Individu(Type, corpseImageKey());
    corpse->size.circle(tools::math::Vector2d(0, 0), 20);
    corpse->size.setOrigin(&corpse->position());

    if (_species != nullptr)
    {
        for (auto& i : _species->inventory.objects)
        {
            corpse->objects.addObject(i.name(), "storagebox" + textManager::toString(key, 2));
            ++key;
        }
    }

    for (auto& i : inventory.objects)
    {
        i.setSlot("storagebox" + textManager::toString(key, 2));
        corpse->objects.objects.push_back(std::move(i));
        ++key;
    }
    inventory.objects.clear();

    corpse->close();
}

Stats& Individu::attributes()
{
    return _attributes;
}

Skill* Individu::skill(const string& s)
{
    if (_skills != nullptr)
    {
        auto i = _skills->find(s);
        if (i != _skills->end()) return &i->second;
    }

    if (_species != nullptr)
    {
        auto i = _species->_skills.find(s);
        if (i != _species->_skills.end()) return &i->second;
    }

    return nullptr;
}

const textManager::PlainText& Individu::displayedName()
{
    if (_displayedName != nullptr)
        return *_displayedName;

    if (_species != nullptr)
        return _species->_displayedName;

    //In case there is no custom name nor species...
    setCustomDisplayedName(textManager::PlainText());
    return *_displayedName;
}

unsigned int Individu::experience()
{
    if (_species != nullptr)
        return _experience + _species->_experience;

    return _experience;
}

bool Individu::angleFixed()
{
    if (_species != nullptr)
        return _species->angleFixed;

    return false;
}

void Individu::Gestion_Recuperation()
{
    modifyHealthStatus(Life, currentHealthStatus(Healing)/1000.0 * tools::timeManager::I(1.0));
    modifyHealthStatus(Energy, currentHealthStatus(Healing)/1000.0 * tools::timeManager::I(1.0));

    if (currentHealthStatus(Healing) > 95)
    {
        modifyHealthStatus(Life, tools::timeManager::I(1.0));
        modifyHealthStatus(Energy, tools::timeManager::I(1.0));
    }

    double diff = currentHealthStatus(HealingPower) + (currentHealthStatus(Life)-800.0)/100.0 - currentHealthStatus(Healing);
    modifyHealthStatus(Healing, diff / 1000.0 * tools::timeManager::I(1.0));

    //Test de récupération forcée (potion, …)
    if (RecuperationFixe || abs(currentHealthStatus(HealingPower)) >= 95)
        setHealthStatus(Healing, currentHealthStatus(HealingPower));

    if (EnergieMax) setHealthStatus(Energy, 1000);

    //Diminue la durée de vie des objets utilisés
    for (auto& i : inventory.objects)
    {
        if (i.active() && i.remainingDuration() > 0)
        {
            i.setRemainingDuration(i.remainingDuration() - tools::timeManager::I(1));

            if (i.remainingDuration() <= 0)
            {
                inventory.deleteObject(i);
                continue;
            }
        }
    }
}

int Individu::currentHealthStatus(Attribute a, bool forceUpdate)
{
    //Update values
    if (forceUpdate || _clock.getElapsedTime().asSeconds() > 1)
    {
        _currentHealthStatus = _attributes;

        for (int i = Strength ; i != numberOfAttributes ; ++i)
        {
            Attribute att = static_cast<Attribute>(i);
            AttributeAmplifier attAmplifier = static_cast<AttributeAmplifier>(i);

            if (att != HealingPower)
                _currentHealthStatus.set(att, _currentHealthStatus[att] / 2.0 * (1 + 1.2*_currentHealthStatus[Life]/1000.0));

            for (auto& o : inventory.objects)
            {
                double add = o.currentObjectEffect(attributeToString(att));
                double mul = o.currentObjectEffect(attributeAmplifierToString(attAmplifier));
                _currentHealthStatus.add(att, add + _attributes[att] * mul / 100.0);
            }
        }
        _clock.restart();
    }

    //Return asked value
    return floor(_currentHealthStatus[a]);
}

void Individu::setHealthStatus(Attribute a, double value)
{
    if (a == Life || a == Energy)
    {
        value = min(max(0.0, value), 1000.0);
        _currentHealthStatus.set(a, value);
        _attributes.set(a, value);
    }
    if (a == Healing && !RecuperationFixe)
    {
        value = min(max(-100.0, value), 100.0);
        _currentHealthStatus.set(a, value);
        _attributes.set(a, value);
    }
}

void Individu::modifyHealthStatus(Attribute a, double value)
{
    if (a == Life || a == Energy)
    {
        value = min(max(-_currentHealthStatus[a], value), 1000.0 - _currentHealthStatus[a]);
        _currentHealthStatus.add(a, value);
        _attributes.add(a, value);
    }
    if (a == Healing && !RecuperationFixe)
    {
        value = min(max(-100.0 - _currentHealthStatus[a], value), 100.0 - _currentHealthStatus[a]);
        _currentHealthStatus.add(a, value);
        _attributes.add(a, value);
    }
}

void Individu::setSpecies(Classe_Commune* s)
{
    _species = s;
}

Classe_Commune* Individu::species()
{
    return _species;
}

Skill* Individu::createSkill(string skillName)
{
    if (_skills == nullptr)
        _skills = new unordered_map<string, Skill>;

    auto result = _skills->try_emplace(skillName, skillName);
    return &(result.first->second);
}

void Individu::setCustomDisplayedName(const textManager::PlainText& newName)
{
    if (_displayedName == nullptr)
        _displayedName = new textManager::PlainText;

    *_displayedName = newName;
}

void Individu::updateAngle(const tools::math::Vector2d& p)
{
    angle = tools::math::angle(p.x - position().x, p.y - position().y);
}

void Individu::findPath(const tools::math::Vector2d& destination, int nodesNumber, bool reduceCollisionWithIndividuals)
{
    tools::aStar::clear();

    tools::aStar::setPoints(position(), destination);
    tools::aStar::setField(viewField);

    tools::math::Shape node;
    node.circle(tools::math::Vector2d(0, 0), size.radius1);
    tools::aStar::setNodesProperties(node, nodesNumber, 30, 150);

    int weightOfIndividuals = (reduceCollisionWithIndividuals) ? 3000 : 10000;

    vector<pair<tools::math::Shape*, int>> obstacles;
    for (auto& i : seenItems)
    {
        switch (i.second)
        {
            case COLL_PRIM :
                obstacles.emplace_back(&i.first->size, 100000);
                break;
            case COLL_PRIM_MVT :
                obstacles.emplace_back(&i.first->size, weightOfIndividuals);
                break;
            default:
                break;
        }
    }

    tools::aStar::setObstacles(obstacles);

    pathToTarget = tools::aStar::aStar();

    //If A* doesn't find a way, try the straight line
    if (pathToTarget.profile == tools::math::Shape::Profiles::None)
        pathToTarget.line(position(), destination, size.radius1);

    pathToTarget.setOrigin(tools::math::absoluteOrigin());
    pathToTarget.radius1 = size.radius1;
}

void Individu::fight(Individu *enemy, Skill* s)
{
    if (enemy == nullptr)
    {
        tools::debug::warning("The function fight() has been called with a nullptr", "", __FILENAME__, __LINE__);
        return;
    }

    if (Diplomatie == enemy->Diplomatie) return;

    //Force the updating of attributes
    currentHealthStatus(Strength, true);
    enemy->currentHealthStatus(Strength, true);

    double Att_Agilite = currentHealthStatus(Agility);
    double Att_Intelli = currentHealthStatus(Intellect);
    double Att_Puissance = currentHealthStatus(Power);
    if (Att_Agilite <= 0) Att_Agilite = 1;
    if (Att_Intelli <= 0) Att_Intelli = 1;
    if (Att_Puissance <= 0) Att_Puissance = 1;

    double TauxReussite = (1.0 + (Att_Agilite - enemy->currentHealthStatus(Agility))/Att_Agilite) * 50.0;
    if (TauxReussite > 95) TauxReussite = 95;
    if (TauxReussite < 5) TauxReussite = 5;

    bool Succes = (rand()%100 < TauxReussite) ? true : false;

    if (Succes)
    {
        double Degats = currentHealthStatus(Strength);
        if (s != nullptr)
        {
            double d = s->damage();
            double a = s->amplitude();
            Degats += d - a + rand()%(2*int(a));
        }

        double TauxEsquive = (1.0 + (enemy->currentHealthStatus(Dodge) - Att_Agilite)/Att_Agilite) * 50.0;
        bool Esquive = (rand()%100 < TauxEsquive) ? true : false;

        if (Esquive)
        {
            Degats = 0;
            if (enemy == gamedata::player())
            {
                gamedata::player()->CoupEsquive(this);
                addConsoleEntry(textManager::getText("devilsai", "ESQUIVE"));
            }
        }
        else
        {
            double TauxCritique = (1 + (Att_Intelli - enemy->currentHealthStatus(Charisma))/Att_Intelli) * 10.0;
            bool Critique = (rand()%100 < TauxCritique) ? true : false;
            if (Critique)
            {
                Degats *= 1.5;
                modifyHealthStatus(Energy, -10);
                enemy->modifyHealthStatus(Healing, -30);
                if (this == gamedata::player())
                {
                    gamedata::player()->CoupCritique(enemy);
                    addConsoleEntry(textManager::getText("devilsai", "CRITIQUE"));
                }
                if (enemy == gamedata::player())
                {
                    gamedata::player()->BlessureGrave(enemy);
                }
            }

            Degats *= (1 + (Att_Puissance - enemy->currentHealthStatus(Constitution))/Att_Puissance)/2.0;
            if (Degats < 5) Degats = 5;
        }

        enemy->modifyHealthStatus(Life, -Degats);
        enemy->modifyHealthStatus(Healing, -3-Degats/20);

        GainExperience(enemy, Degats);

        textManager::PlainText resultText = textManager::getText("devilsai", "console-damageDone");

        textManager::PlainText a;
        if (Diplomatie == DIPLOM_ALLIE)
            a += "@c[128,255,128]";
        if (Diplomatie == DIPLOM_ENNEMI)
            a += "@c[255,128,128]";
        a += displayedName();

        textManager::PlainText b;
        if (enemy->Diplomatie == DIPLOM_ALLIE)
            b += "@c[128,255,128]";
        if (enemy->Diplomatie == DIPLOM_ENNEMI)
            b += "@c[255,128,128]";
        b += enemy->displayedName();

        resultText.addParameter(a);
        resultText.addParameter((int)Degats);
        resultText.addParameter(b);

        addConsoleEntry(resultText);
        if (this == gamedata::player())
        {
            gamedata::player()->ApplicationAmeliorations();
        }
        if (enemy == gamedata::player())
        {
            if (gamedata::player()->_currentSkill->Id == enemy->behavior(Behaviors::Hurt))
                gamedata::player()->BlessuresMultiples(this);
        }

        if (Degats) enemy->Set_Activite(enemy->behavior(Behaviors::Hurt));
    }
    else if (this == gamedata::player()) addConsoleEntry(textManager::getText("devilsai", "ECHEC"));
}

void Individu::Disp(RenderTarget& target)
{
    if (options::option<bool>(tools::math::sdbm_hash("displayShapes")))
    {
        viewField.display(target, Color(255, 255, 0, 20));
        interactionField.display(target, Color(255, 0, 0, 30));
        size.display(target, Color(255, 255, 255, 50));
    }

    imageManager::display(target, "individuals", _currentSkill->getImageKey(angle, _animationFrame), position().x, position().y, true);
}

void Individu::displayLifeGauge(RenderTarget& target)
{
    if (Diplomatie == DIPLOM_NEUTRE || Diplomatie == DIPLOM_BREAKABLE) return;

    //x and y are used to convert position to integers; otherwise, the text may be blurred
    int x = position().x;
    int y = position().y;

    textManager::RichText individualName;
    individualName.setSize(160, 0);

    if (Diplomatie == DIPLOM_ALLIE)
        individualName.setDefaultProperties("liberation", 10, Color(128, 255, 128));
    else
        individualName.setDefaultProperties("liberation", 10, Color(255, 255, 255));

    individualName.addFlags(textManager::HAlignCenter | textManager::OriginXCenter);
    individualName.create(displayedName());
    individualName.displayFullText(target, x, y + 20);

    RectangleShape background(Vector2f(50, 4));
    background.setPosition(x - 25, y + 35);
    background.setFillColor(Color(0, 0, 0, 175));
    target.draw(background);

    RectangleShape foreground(Vector2f(currentHealthStatus(Life)/20, 4));
    foreground.setPosition(x - 25, y + 35);
    foreground.setFillColor(Color(228, 0, 0, 255));
    target.draw(foreground);
}
