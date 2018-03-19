
/*
    devilsai - An Action-RPG game
    Copyright (C) 2009-2018  Quentin Henriet

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

#ifndef DEVILSAI_RESOURCES_STATS
#define DEVILSAI_RESOURCES_STATS

#include <string_view>

namespace tinyxml2 {
    class XMLDocument;
    class XMLElement;
    class XMLHandle;
}

using namespace std;

enum Attribute {
    Life = 0, Energy, Healing,
    Strength, Power, Agility, Intellect,
    Constitution, Charisma, Dodge, HealingPower,
    RunSpeed, AttackSpeed, InjurySpeed,
    Precision,
    numberOfAttributes
};

enum AttributeAmplifier {
    LifeAmplifier = 0, EnergyAmplifier, HealingAmplifier,
    StrengthAmplifier, PowerAmplifier, AgilityAmplifier, IntellectAmplifier,
    ConstitutionAmplifier, CharismaAmplifier, DodgeAmplifier, HealingPowerAmplifier,
    RunSpeedAmplifier, AttackSpeedAmplifier, InjurySpeedAmplifier,
    PrecisionAmplifier
};

constexpr const char* AttributesNames[] = {
    "life", "energy", "healing",
    "strength", "power", "agility", "intellect",
    "constitution", "charisma", "dodge", "healingPower",
    "runSpeed", "attackSpeed", "injurySpeed",
    "precision"
};

constexpr const char* AttributesAmplifiersNames[] = {
    "lifeAmplifier", "energyAmplifier", "healingAmplifier",
    "strengthAmplifier", "powerAmplifier", "agilityAmplifier", "intellectAmplifier",
    "constitutionAmplifier", "charismaAmplifier", "dodgeAmplifier", "healingPowerAmplifier",
    "runSpeedAmplifier", "attackSpeedAmplifier", "injurySpeedAmplifier",
    "precisionAmplifier"
};

/**
 * \brief Statistics of any individual.
 */

class Stats
{
    private:
        double _stats[numberOfAttributes];

    public:
        /**
         * Default constructor.
         */
        Stats();

        /**
         * Access operator.
         */
        constexpr double operator[](Attribute a)
        {
            if (a == numberOfAttributes) return 0;
            return _stats[a];
        }

        /**
         * Modifier operator.
         *
         * \param a value to change.
         * \param value offset added to the value.
         */
        void add(Attribute a, double value);

        /**
         * Modifier operator.
         *
         * \param a value to change.
         * \param value new value.
         */
        void set(Attribute a, double value);

    public:
        void loadFromXML(tinyxml2::XMLElement* elem);
        void saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle);
};

/**
 * Casts an attribute to a const char*, mainly used for lua communications.
 */
static constexpr const char* attributeToString(Attribute a)
{
    return AttributesNames[static_cast<int>(a)];
}

/*
 * Casts an attribute amplifier to a const char*, mainly used for lua communications.
 */
static constexpr const char* attributeAmplifierToString(AttributeAmplifier a)
{
    return AttributesAmplifiersNames[static_cast<int>(a)];
}

/**
 * Casts a string to an attribute.
 */
static constexpr Attribute stringToAttribute(string_view a)
{
    for (int i = 0 ; i < numberOfAttributes ; ++i)
        if (AttributesNames[i] == a)
            return static_cast<Attribute>(i);
    return numberOfAttributes;
}

/**
 * Casts a string to an attribute amplifier.
 */
static constexpr AttributeAmplifier stringToAttributeAmplifier(string_view a)
{
    for (int i = 0 ; i < numberOfAttributes ; ++i)
        if (AttributesAmplifiersNames[i] == a)
            return static_cast<AttributeAmplifier>(i);
    return static_cast<AttributeAmplifier>(numberOfAttributes);
}

#endif // DEVILSAI_RESOURCES_STATS
