
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
    _timer(0),
    _targetedItem(nullptr),
    _currentSkill(nullptr),
    _currentHealthStatus(),
    _attributes(),
    _behaviors(nullptr),
    _attacks(nullptr),
    _skills(),
    _species(nullptr),
    _displayedName(nullptr),
    _corpseImageKey(nullptr),
    _extraDataFile(nullptr),
    _clock(),
    _owner(),
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
    Comportement(other.Comportement),
    NouveauComportement(other.NouveauComportement),
    _animationFrame(other._animationFrame),
    _timer(other._timer),
    _targetedItem(other._targetedItem),
    _currentSkill(other._currentSkill),
    _currentHealthStatus(other._currentHealthStatus),
    _attributes(other._attributes),
    _behaviors(nullptr),
    _attacks(nullptr),
    _skills(other._skills),
    _species(other._species),
    _displayedName(nullptr),
    _corpseImageKey(nullptr),
    _experience(other._experience),
    _extraDataFile(nullptr),
    _clock(other._clock),
    _owner(other._owner),
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
    Comportement(other.Comportement),
    NouveauComportement(other.NouveauComportement),
    _animationFrame(other._animationFrame),
    _timer(other._timer),
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
    _owner(std::move(other._owner)),
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
    other._displayedName = nullptr;
    other._corpseImageKey = nullptr;
    other._extraDataFile = nullptr;
}

Individu::Individu(Classe_Commune* s, double x, double y)
  : Individu()
{
    _species = s;
    _species->Copie_Element(this);
    Type = _species->Type;
    move(x, y);
}

Individu::~Individu()
{
    if (_behaviors != nullptr)
        delete[] _behaviors;

    if (_attacks != nullptr)
        delete _attacks;

    if (_displayedName != nullptr)
        delete _displayedName;

    if (_corpseImageKey != nullptr)
        delete _corpseImageKey;

    if (_extraDataFile != nullptr)
        delete _extraDataFile;
}

bool Individu::Set_Activite(const string& nv)
{
    if (skill(nv).none()) return false;
    if (skill(nv).unavailability() > 0) return false;

    if (_currentSkill.none())
    {
        _currentSkill = skill(nv);
        _currentSkill->atBegin(this);
        //Force the update of health status, as skills can change the speed
        currentHealthStatus(Strength, true);
        _animationFrame = 0;
        ActEffectue = false;
    }

    if (_currentSkill->priority > skill(nv)->priority && !ActEffectue) return false;

    if (_currentSkill == skill(nv)) return true;

    _currentSkill = skill(nv);
    _currentSkill->atBegin(this);
    ActEffectue = false;
    //Force the update of health status, as skills can change the speed
    currentHealthStatus(Strength, true);

    if (_currentSkill == skill(behavior(Behaviors::Dying)))
    {
        setHealthStatus(Life, 0);
        setHealthStatus(Energy, 0);
        setHealthStatus(Healing, 0);
        Diplomatie = DIPLOM_NEUTRE;
    }

    if (nv == behavior(Behaviors::Dying)) size.circle(tools::math::Vector2d(0, 0), 0);

    if (_currentSkill->priority > 0)
        _animationFrame = 0;

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

void Individu::nextAnimationFrame()
{
    if (_currentSkill == skill(behavior(Behaviors::Dying)) && (unsigned)_animationFrame == _currentSkill->numberOfImages-1) return;

    if (_currentSkill->step > 0)
        _animationFrame += currentHealthStatus(_currentSkill->speedAttribute) * _timer / (double)_currentSkill->step;
    else
        _animationFrame += currentHealthStatus(_currentSkill->speedAttribute) * _timer / 10.0;

    if (_animationFrame > _currentSkill->numberOfImages)
    {
        ActEffectue = true;
        _currentSkill->atEnd(this);
        _currentSkill.unavailability() = _currentSkill.maxUnavailability();
    }
    else ActEffectue = false;

    while (_animationFrame > _currentSkill->numberOfImages)
        _animationFrame -= _currentSkill->numberOfImages;

    _timer = 0;
}

void Individu::otherItemDeleted(Element_Carte* other)
{
    if (_targetedItem == other)
        _targetedItem = nullptr;
    if (owner() == other)
    {
        _owner.set<Individu*>(nullptr);
        lifetime = 0;
    }
}

Element_Carte* Individu::targetedItem()
{
    return _targetedItem;
}

bool Individu::interact(Element_Carte* item)
{
    return intersection(interactionField, item->size);
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

Individu::SkillAccess Individu::skill(const string& s)
{
    auto i = _skills.find(s);
    if (i != _skills.end()) return SkillAccess(&i->second);

    return SkillAccess(nullptr);
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
    double t = tools::timeManager::I(1.0);

    modifyHealthStatus(Life, currentHealthStatus(Healing)/1000.0 * t);
    modifyHealthStatus(Energy, currentHealthStatus(Healing)/1000.0 * t);

    if (currentHealthStatus(Healing) > 95)
    {
        modifyHealthStatus(Life, t);
        modifyHealthStatus(Energy, t);
    }

    double diff = currentHealthStatus(HealingPower) + (currentHealthStatus(Life)-800.0)/100.0 - currentHealthStatus(Healing);
    modifyHealthStatus(Healing, diff / 1000.0 * t);

    //Test de récupération forcée (potion, …)
    if (RecuperationFixe || abs(currentHealthStatus(HealingPower)) >= 95)
        setHealthStatus(Healing, currentHealthStatus(HealingPower));

    if (EnergieMax) setHealthStatus(Energy, 1000);

    //Diminue la durée de vie des objets utilisés
    for (auto& i : inventory.objects)
    {
        if (i.active() && i.remainingDuration() > 0)
        {
            i.setRemainingDuration(i.remainingDuration() - t);

            if (i.remainingDuration() <= 0)
            {
                inventory.deleteObject(i);
                continue;
            }
        }
    }

    //Update the unavailability of the skills
    for (auto& i : _skills)
        i.second.unavailability = max(0.0, i.second.unavailability - t/60.0);
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

            if (!_currentSkill.none())
            {
                _currentHealthStatus.add(att, _currentSkill.extraStats(att));
                _currentHealthStatus.add(att, _currentSkill.extraStatsAmplifiers(att) * _attributes[att] / 100.0);
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

Classe_Commune* Individu::species()
{
    return _species;
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

void Individu::fight(Individu *enemy)
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

        double TauxEsquive = (1.0 + (enemy->currentHealthStatus(Dodge) - Att_Agilite)/Att_Agilite) * 50.0;
        bool Esquive = (rand()%100 < TauxEsquive) ? true : false;

        if (Esquive)
        {
            Degats = 0;
            improveAttribute(Dodge, 10, enemy);
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
                improveAttribute(Agility, 10, enemy);
                improveAttribute(Charisma, 10, enemy);
                enemy->improveAttribute(Constitution, -10, this);
                enemy->improveAttribute(Charisma, -10, this);
            }
            else if (currentHealthStatus(Precision) < 100)
            {
                double reduction = rand() % (int)(100 - currentHealthStatus(Precision) + 1);
                Degats *= (100.0 - reduction)/100.0;
            }

            Degats *= (1 + (Att_Puissance - enemy->currentHealthStatus(Constitution))/Att_Puissance)/2.0;
            if (Degats < 5) Degats = 5;

            if (enemy->_currentSkill == enemy->skill(enemy->behavior(Behaviors::Hurt)))
            {
                enemy->improveAttribute(Strength, -10, this);
                enemy->improveAttribute(Power, -10, this);
                enemy->improveAttribute(Dodge, -10, this);
            }
        }

        enemy->modifyHealthStatus(Life, -Degats);
        enemy->modifyHealthStatus(Healing, -3-Degats/20);

        if (enemy->currentHealthStatus(Life) > 0)
            gainExperience(Degats / 5000. * enemy->experience());
        else gainExperience(enemy->experience());

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

        if (Degats) enemy->Set_Activite(enemy->behavior(Behaviors::Hurt));
    }
}

void Individu::setOwner(Individu* o)
{
    _owner.set<Individu*>(o);
}

Individu* Individu::owner()
{
    if (!_owner.valid()) return nullptr;

    if (_owner.is<int>())
        _owner.set<Individu*>(static_cast<Individu*>(gamedata::findElement(_owner.get<int>())));

    return _owner.get<Individu*>();
}

void Individu::gainExperience(int exp)
{
    for (int i = 0 ; i < 1 + exp / 100 ; ++i)
    {
        improveAttribute(Intellect, 1, nullptr);
        improveAttribute(Strength, 1, nullptr);
        improveAttribute(Power, 1, nullptr);
        improveAttribute(Constitution, 1, nullptr);
    }

    _experience += exp;
    if (owner() != nullptr) owner()->gainExperience(exp);
}

void Individu::improveAttribute(Attribute a, int chance, Individu* enemy)
{
    if (chance == 0 || rand()%100 >= abs(chance))
        return;

    auto c1 = [&](Attribute s)
    {
        double x = currentHealthStatus(s);
        double y = atan(log(1+x) * M_PI) / M_PI;
        return min(1.0, y);
    };
    auto c2 = [&](Attribute s, Attribute o)
    {
        if (enemy == nullptr) return 0.0;
        double x = enemy->currentHealthStatus(o) / max(1, currentHealthStatus(s));
        double y = atan(log(1+x) * M_PI) / M_PI;
        return min(1.0, y);
    };

    double result = 0;

    if (chance > 0) switch (a)
    {
        case Strength:
            [[fallthrough]]
        case Power:
            [[fallthrough]]
        case Intellect:
            [[fallthrough]]
        case Constitution:
            result = c1(Intellect);
            break;
        case Agility:
            [[fallthrough]]
        case Charisma:
            result = c2(a, a);
            break;
        case Dodge:
            result = c2(Dodge, Agility);
            break;
        default:
            break;
    }
    else switch (a)
    {
        case Strength:
            [[fallthrough]]
        case Power:
            [[fallthrough]]
        case Constitution:
            [[fallthrough]]
        case Dodge:
            result = - c2(Constitution, Power);
            break;
        case Agility:
            [[fallthrough]]
        case Intellect:
            result = - (70.0-currentHealthStatus(Energy))/100000.0*tools::timeManager::I(1);
            break;
        case Charisma:
            result = - c2(Charisma, Charisma);
            break;
        default:
            break;
    }

    _attributes.add(a, result);
}

void Individu::Disp(RenderTarget& target)
{
    if (options::option<bool>(tools::hash("displayShapes")))
    {
        viewField.display(target, Color(255, 255, 0, 20));
        interactionField.display(target, Color(255, 0, 0, 30));
        size.display(target, Color(255, 255, 255, 50));
    }

    if (_currentSkill.none()) return;

    imageManager::display(target, tools::hash("individuals"), _currentSkill->getImageKey(angle, _animationFrame), position().x, position().y, true);
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
