
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

/*
 * Most colours used here come from the 'Oxygen color palette'
 * Copyright (C) The Oxygen Team
 * Released under the Creative Commons Attribution-NonCommercial-NoDerivs 2.5 License.
 * http://creativecommons.org/licenses/by-nc-nd/2.5/
 */

#include "gui/style.h"

#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "tools/debug.h"
#include "tools/math.h"
#include "tools/variant.h"

using namespace std;
using namespace sf;

namespace gui{

Vector2i MousePosition;


typedef tools::math::Variant<int, unsigned int, string, Color> Option;
static unordered_map<unsigned int, Option> Gui_Parameters;


template<typename T>
void parameterize(unsigned int hash, T value)
{
    const auto& i = Gui_Parameters.find(hash);

    if (i == Gui_Parameters.end())
    {
        Option o;
        o.set<T>(value);
        Gui_Parameters.emplace(hash, o);
    }
    else
        i->second.set<T>(value);
}

//Explicit instantiations for the linker
template void parameterize<int>(unsigned int hash, int value);
template void parameterize<unsigned int>(unsigned int hash, unsigned int value);
template void parameterize<string>(unsigned int hash, string value);
template void parameterize<Color>(unsigned int hash, Color value);

template<typename T>
T parameter(unsigned int hash)
{
    const auto& i = Gui_Parameters.find(hash);
    if (i != Gui_Parameters.end())
        return i->second.get<T>();

    return T();
}

//Explicit instantiations for the linker
template int parameter<int>(unsigned int hash);
template unsigned int parameter<unsigned int>(unsigned int hash);
template string parameter<string>(unsigned int hash);
template Color parameter<Color>(unsigned int hash);

bool initLibrary()
{
    bool init = true;

    auto test = [&](const char* p)
    {
        if (Gui_Parameters.find(tools::hash(p)) == Gui_Parameters.end())
        {
            tools::debug::error("Cannot initialize gui library: parameter '" + string(p) + "' is missing.", "gui", __FILENAME__, __LINE__);
            init = false;
        }
    };

    test("text-font");
    test("text-color");
    test("text-size");
    test("button-text-font");
    test("button-text-color");
    test("button-text-size");
    test("list-text-font");
    test("list-text-color");
    test("list-text-size");
    test("list-background-color");
    test("list-highlight-color");
    test("list-selected-color");
    test("input-text-font");
    test("input-text-color");
    test("input-text-size");

    return init;
}

void updateMousePosition(sf::RenderWindow& app)
{
    MousePosition = Mouse::getPosition(app);
}

const sf::Vector2<int>& mousePosition()
{
    return MousePosition;
}

} //namespace gui
