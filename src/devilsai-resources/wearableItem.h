
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

#ifndef DEVILSAI_RESOURCES_WEARABLEITEM
#define DEVILSAI_RESOURCES_WEARABLEITEM

class lua_State;

namespace sf
{
    class RenderWindow;
}


/**
 * \brief Items that can be worn or put in an inventory.
 *
 * This class is manly use to encapsulate the lua_State pointer,
 * simplify its management and avoid a potential memory leak.
 *
 * \remark WearableItem cannot be copied, only moved.
 */

class WearableItem
{
    private:
        lua_State* _s;

    public:
        /**
         * Default constructor.
         */
        WearableItem();

        /**
         * Move constructor.
         *
         * The other item is invalidated.
         */
        WearableItem(WearableItem&& other);

        /**
         * Move assignment operator.
         *
         * The other item is invalidated.
         */
        const WearableItem& operator=(WearableItem&& right);

        /**
         * Default destructor.
         */
        ~WearableItem();

        /**
         * Comparison operator.
         */
        bool operator==(const WearableItem& right);

    private:
        WearableItem(WearableItem& other) = delete;
        WearableItem(const WearableItem& other) = delete;
        const WearableItem& operator=(WearableItem& right) = delete;
        const WearableItem& operator=(const WearableItem& right) = delete;

    public:
        /**
         * Creates a WearableItem from a lua file.
         *
         * The file is analysed to check if all variables and functions needed are there.
         * If not, an error will be raised though tools::debug and the lua file will be closed.
         *
         * \param file short name of the lua file, without the whole path and extension.
         */
        void create(const string& file);

        /**
         * Tells whether the WearableItem has been successfully created.
         */
        bool valid() const;

        /**
         * Tells whether the WearableItem has currently any effect.
         *
         * Generally, WearableItems have effects only when their current slot
         * is the required slot.
         */
        bool active() const;

        /**
         * Returns the name of the WearableItem.
         */
        string name() const;

        /**
         * Returns the current slot in which the WearableItem has been put.
         */
        string currentSlot() const;

        /**
         * Retruns the required slot to make the WearableItem active.
         */
        string requiredSlot() const;

        /**
         * Tells whether the WearableItem is temporary, i.e. its effects
         * are limited in time.
         *
         * \see remainingDuration()
         * \see setRemainingDuration()
         */
        bool temporary() const;

        /**
         * Returns the remaining duration before the effects of the WearableItem fade.
         *
         * \see temporary()
         * \see setRemainingDuration()
         */
        double remainingDuration() const;

        /**
         * Tells whether the WearableItem can be stacked.
         *
         * \see quantity()
         * \see setQuantity()
         */
        bool stackable() const;

        /**
         * Returns the current quantity of the item, in case it can be stacked.
         *
         * \see stackable()
         * \see setQuantity()
         */
        int quantity() const;

        /**
         * Returns the current effect of the WearableItem on a specific attribute.
         *
         * \param a attribute for which we need the effect.
         */
        double currentObjectEffect(const char* a) const;

        /**
         * Improves the WearableItem using its own improvement function.
         *
         * \param quality quality of the newly created item.
         */
        void generateRandomItem(int quality);

        /**
         * Sets the current slot in which the WearableItem is.
         *
         * \param slot new slot.
         */
        void setSlot(string_view slot);

        /**
         * Sets the remaining duration of the WearableItem.
         *
         * For now, the WearableItem does not care about its lifetime. The
         * remaining duration must be changed manually.
         *
         * \param d new remaining duration.
         * \see temporary()
         * \see remainingDuration()
         */
        void setRemainingDuration(double d);

        /**
         * Sets the quantity of the WearableItem, if it can be stacked.
         *
         * \param q new quantity.
         * \see stackable()
         * \see quantity()
         */
        void setQuantity(int q);

        /**
         * Returns a string containing all information needed to save the
         * current state of the WearableItem.
         *
         * This string can be used to restore the state with recoverState()
         * or to clone an item.
         *
         * \see recoverState()
         */
        string currentState() const;

        /**
         * Restore the WearableItem to a previous state.
         *
         * This function is used to load items from a savepack or to clone
         * an item from another WearableItem.
         *
         * \param state string generated by currentState().
         * \see currentState()
         */
        void recoverState(string_view state);

        /**
         * Creates a textual description of the WearableItem and
         * display it on the screen.
         *
         * \param target the RenderWindow on which the description should be displayed.
         */
        void displayDescription(sf::RenderWindow& target);
};

#endif // DEVILSAI_RESOURCES_WEARABLEITEM
