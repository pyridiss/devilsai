
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

#ifndef TOOLS_GRAPHICBUFFER
#define TOOLS_GRAPHICBUFFER

#include <SFML/Graphics.hpp>

#include "tools/debug.h"

namespace tools::graphicBuffer{

inline RenderTexture TemporaryTexture;
inline RenderWindow* MainWindow = nullptr;

inline void initLibrary(RenderWindow* w)
{
    MainWindow = w;
    if (w != nullptr)
        TemporaryTexture.create(w->getSize().x, w->getSize().y);
    TemporaryTexture.setSmooth(true);
}

inline RenderTexture& buffer()
{
    if (MainWindow == nullptr)
    {
        tools::debug::error("The graphic buffer has not been initialized.", "tools", __FILENAME__, __LINE__);
        return TemporaryTexture;
    }

    if (MainWindow->getSize() != TemporaryTexture.getSize())
        TemporaryTexture.create(MainWindow->getSize().x, MainWindow->getSize().y);

    return TemporaryTexture;
}

}  // namespace tools::graphicBuffer


#endif  // TOOLS_GRAPHICBUFFER
