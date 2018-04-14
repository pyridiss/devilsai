
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

#ifndef DEVILSAI_RESOURCES_MANAGER
#define DEVILSAI_RESOURCES_MANAGER

#include <string>
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>

namespace devilsai{

inline std::unordered_map<std::type_index, std::unordered_map<std::string, void*>> Resources;

template<typename T>
T* getResource(const string& id)
{
    auto i = Resources.find(type_index(typeid(T)));

    if (i != Resources.end())
    {
        auto j = i->second.find(id);
        if (j != i->second.end())
            return reinterpret_cast<T*>(j->second);
    }

    return nullptr;
}

template<typename T>
T* addResource(const string& id)
{
    T* r = getResource<T>(id);

    if (r == nullptr)
    {
        r = new T(id);
        Resources[type_index(typeid(T))].emplace(id, r);
    }

    return r;
}

template<typename T>
std::vector<T*> getAllResources()
{
    auto i = Resources.find(type_index(typeid(T)));

    if (i != Resources.end())
    {
        std::vector<T*> v;
        v.reserve(i->second.size());
        for (auto j : i->second)
            v.push_back(reinterpret_cast<T*>(j.second));

        return v;
    }

    return std::vector<T*>();
}

template<typename T>
void deleteResources()
{
    auto i = Resources.find(type_index(typeid(T)));

    if (i != Resources.end())
    {
        for (auto& j : i->second)
            delete reinterpret_cast<T*>(j.second);

        Resources.erase(i);
    }
}

}  // namespace devilsai

#endif  // DEVILSAI_RESOURCES_MANAGER
