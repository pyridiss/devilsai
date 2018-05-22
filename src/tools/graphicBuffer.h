
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

inline sf::RenderTexture TemporaryTexture;
inline sf::RenderWindow* MainWindow = nullptr;

inline void initLibrary(sf::RenderWindow* w)
{
    MainWindow = w;
    if (w != nullptr)
        TemporaryTexture.create(w->getSize().x, w->getSize().y);
    TemporaryTexture.setSmooth(true);
}

/**
 * Return a RenderTexture to render stuff off-screen.
 *
 * If the size is not given, the buffer will have the size of the screen.
 * Using this function instead of creating a new RenderTexture when needed
 * will avoid a huge overhead: the buffer is recreated only if the size
 * needed is not the same as the current size.
 *
 * \param x x-size of the buffer, default value (0) creates a buffer with the size of the screen
 * \param y y-size of the buffer, default value (0) creates a buffer with the size of the screen
 * \return non-owning pointer to the buffer
 */
inline sf::RenderTexture& buffer(unsigned x = 0, unsigned y = 0)
{
    if (x == 0 || y == 0)
    {
        if (MainWindow == nullptr)
        {
            tools::debug::error("The graphic buffer has not been initialized.", "tools", __FILENAME__, __LINE__);
            return TemporaryTexture;
        }

        x = MainWindow->getSize().x;
        y = MainWindow->getSize().y;
    }

    if (TemporaryTexture.getSize() != sf::Vector2u(x, y))
        TemporaryTexture.create(x, y);

    return TemporaryTexture;
}

}  // namespace tools::graphicBuffer


#endif  // TOOLS_GRAPHICBUFFER
