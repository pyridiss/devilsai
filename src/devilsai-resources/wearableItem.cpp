
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

#include <lua.hpp>

#include "tools/filesystem.h"
#include "textManager/richText.h"
#include "imageManager/imageManager.h"

#include "Attributs/Attributs.h"
#include "devilsai-resources/wearableItem.h"

WearableItem::WearableItem()
  : _s(nullptr)
{
}

WearableItem::WearableItem(WearableItem&& other)
  : _s(std::move(other._s))
{
    other._s = nullptr;
}

const WearableItem& WearableItem::operator=(WearableItem&& right)
{
    _s = std::move(right._s);
    right._s = nullptr;

    return *this;
}

WearableItem::~WearableItem()
{
    if (_s != nullptr)
        lua_close(_s);
}

bool WearableItem::operator==(const WearableItem& right)
{
    return (_s == right._s);
}

void WearableItem::create(const string& file)
{
    _s = luaL_newstate();
    luaL_openlibs(_s);

    luaL_dofile(_s, (tools::filesystem::dataDirectory() + "object/" + file + ".lua").c_str());

    lua_atpanic(_s, [](lua_State* S)
    {
        tools::debug::error(lua_tostring(S, -1), "lua", __FILENAME__, __LINE__);
        return 0;
    });

    //Check if the file is well-formed

    bool fileComplete = true;

    auto check = [&](const char* f)
    {
        lua_getglobal(_s, f);
        if (lua_isnil(_s, -1))
        {
            tools::debug::error("The WearableItem '" + file + "' does not define the symbol '" + string(f) + "'", "lua", __FILENAME__, __LINE__);
            fileComplete = false;
        }
        lua_pop(_s, 1);
    };

    check("active");
    check("categoryObject");
    check("currentSlot");
    check("descriptionAutomatique");
    check("descriptionManuelle");
    check("duree");
    check("name");
    check("generateRandomObject");
    check("stackable");
    check("getCurrentObjectEffect");
    check("getObjectProperty");
    check("iconFile");
    check("imageFile");
    check("objectRecoverState");
    check("objectSave");
    check("quantite");
    check("slotForUse");

    if (!fileComplete)
    {
        lua_close(_s);
        _s = nullptr;
        return;
    }

    //Load images

    lua_getglobal(_s, "imageFile");
    imageManager::addImage("objects", file, lua_tostring(_s, -1));
    lua_pop(_s, 1);

    lua_getglobal(_s, "iconFile");
    imageManager::addImage("objectsIcons", file, lua_tostring(_s, -1));
    lua_pop(_s, 1);
}


bool WearableItem::valid() const
{
    return (_s != nullptr);
}

bool WearableItem::active() const
{
    if (!valid()) return false;

    lua_getglobal(_s, "active");
    lua_call(_s, 0, 1);
    bool b = lua_toboolean(_s, -1);
    lua_pop(_s, 1);
    return b;
}

string WearableItem::name() const
{
    if (!valid()) return string();

    lua_getglobal(_s, "name");
    string name = lua_tostring(_s, -1);
    lua_pop(_s, 1);
    return name;
}

string WearableItem::currentSlot() const
{
    if (!valid()) return string();

    lua_getglobal(_s, "currentSlot");
    string s = lua_tostring(_s, -1);
    lua_pop(_s, 1);
    return std::move(s);
}

string WearableItem::requiredSlot() const
{
    if (!valid()) return string();

    lua_getglobal(_s, "slotForUse");
    string s = lua_tostring(_s, -1);
    lua_pop(_s, 1);
    return std::move(s);
}

bool WearableItem::temporary() const
{
    if (!valid()) return false;

    lua_getglobal(_s, "categoryObject");
    bool b = (string_view(lua_tostring(_s, -1)) == "temporaire");
    lua_pop(_s, 1);
    return b;
}

double WearableItem::remainingDuration() const
{
    if (!valid()) return 0;

    lua_getglobal(_s, "duree");
    double d = lua_tonumber(_s, -1);
    lua_pop(_s, 1);
    return d;
}

bool WearableItem::stackable() const
{
    if (!valid()) return false;

    //stackable can be defined as a function or as a global variable in scripts.

    lua_getglobal(_s, "stackable");

    if (lua_isfunction(_s, -1))
        lua_call(_s, 0, 1);

    bool b = lua_toboolean(_s, -1);
    lua_pop(_s, 1);
    return b;
}

int WearableItem::quantity() const
{
    if (!valid()) return 0;

    lua_getglobal(_s, "quantite");
    int i = lua_tonumber(_s, -1);
    lua_pop(_s, 1);
    return i;
}

double WearableItem::currentObjectEffect(const char* a) const
{
    if (!valid()) return 0;

    lua_getglobal(_s, "getCurrentObjectEffect");
    lua_pushstring(_s, a);
    lua_call(_s, 1, 1);
    double result = lua_tonumber(_s, -1);
    lua_pop(_s, 1);
    return result;
}

void WearableItem::generateRandomItem(int quality)
{
    if (!valid()) return;

    if (quality > 0)
    {
        lua_getglobal(_s, "generateRandomObject");
        lua_pushnumber(_s, quality);
        lua_call(_s, 1, 0);
    }
}
void WearableItem::setSlot(string_view slot)
{
    if (!valid()) return;

    lua_pushstring(_s, slot.data());
    lua_setglobal(_s, "currentSlot");
}

void WearableItem::setRemainingDuration(double d)
{
    if (!valid()) return;

    lua_pushnumber(_s, d);
    lua_setglobal(_s, "duree");
}

void WearableItem::setQuantity(int q)
{
    if (!valid()) return;

    lua_pushnumber(_s, q);
    lua_setglobal(_s, "quantite");
}

string WearableItem::currentState() const
{
    if (!valid()) return string();

    lua_getglobal(_s, "objectSave");
    lua_call(_s, 0, 1);
    string s = lua_tostring(_s, -1);
    lua_pop(_s, 1);
    return std::move(s);
}

void WearableItem::recoverState(string_view state)
{
    if (!valid()) return;

    lua_getglobal(_s, "objectRecoverState");
    lua_pushstring(_s, state.data());
    lua_call(_s, 1, 0);
}

void WearableItem::displayDescription(RenderWindow& target)
{
    if (!valid()) return;

    textManager::PlainText description;

    description += "@f[dayroman]@s[20]@c[255,220,255]";
    description += textManager::getText("objects", name());
    description += " @d";

    lua_getglobal(_s, "descriptionManuelle");
    if (lua_toboolean(_s, -1))
    {
        description += " @n";
        description += textManager::getText("objects", name() + "-description");
    }

    if (remainingDuration() > 0)
    {
        description += " @n";
        description += textManager::getText("devilsai", "EQUIP_DUREE");
        description.addParameter((int)(remainingDuration()/60.f));
    }

    lua_getglobal(_s, "descriptionAutomatique");
    if (lua_toboolean(_s, -1))
    {
        for (auto& p : AttributesNames)
        {
            lua_getglobal(_s, "getObjectProperty");
            lua_pushstring(_s, p);
            lua_call(_s, 1, 1);
            int value = lua_tonumber(_s, -1);
            if (value != 0)
            {
                description += " @n";
                description += textManager::getText("devilsai", string("object-property-") + p);
                description.addParameter(value);
            }
        }
        for (auto& p : AttributesAmplifiersNames)
        {
            lua_getglobal(_s, "getObjectProperty");
            lua_pushstring(_s, p);
            lua_call(_s, 1, 1);
            int value = lua_tonumber(_s, -1);
            if (value != 0)
            {
                description += " @n";
                description += textManager::getText("devilsai", string("object-property-") + p);
                description.addParameter(value);
            }
        }
    }

    textManager::RichText rich;
    rich.setSize(300, 0);
    rich.setDefaultProperties("liberation", 12, Color(255, 255, 255));
    rich.addFlags(textManager::HAlignCenter);
    rich.create(description);

    float x = gui::mousePosition().x;
    float y = gui::mousePosition().y;
    float w = 300;
    float h = rich.height();

    if (y + h > target.getSize().y)
        y -= h;

    gui::style::textBackgroundShader(target, x - 10, y - 10, w + 2*10, h + 2*10);

    rich.displayFullText(target, x, y);
}
