
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

#include "config.h"

#include "Bibliotheque/Bibliotheque.h"

#include "tools/style.h"

namespace tools{

namespace style{

Font liberation;

Shader blurShader;
Shader contrastShader;

void initStyle()
{
    liberation.loadFromFile(INSTALL_DIR + "LiberationSans-Regular.ttf");
    blurShader.loadFromFile(INSTALL_DIR + "blurShader.frag", Shader::Type::Fragment);
    contrastShader.loadFromFile(INSTALL_DIR + "contrastShader.frag", Shader::Type::Fragment);
}

const Font& defaultFont()
{
    return liberation;
}

Color normalButtonTextColor()
{
    return Color(200, 170, 0, 255);
}

Color activeButtonTextColor()
{
    return Color(200, 100, 0, 255);
}

Color hoverButtonTextColor()
{
    return Color(200, 135, 0, 255);
}

Color disabledButtonTextColor()
{
    return Color(200, 200, 200, 255);
}

int textSize()
{
    return 10;
}

void textBackgroundShader(RenderWindow& app, int x, int y, int w, int h)
{
    Texture tex;
    tex.create(app.getSize().x, app.getSize().y);
    tex.update(app);
    RectangleShape rect;
    rect.setSize(Vector2f(w, h));
    rect.setTexture(&tex, true);
    rect.setTextureRect(IntRect(x, y, w, h));
    rect.setPosition(x, y);
    RenderStates states;
    states.shader = &blurShader;
    blurShader.setParameter("texture", tex);
    blurShader.setParameter("textureSize", tex.getSize().x, tex.getSize().y);
    app.draw(rect, states);
}

void highlightShader(RenderWindow& app, int x, int y, int w, int h)
{
    Texture tex;
    tex.create(app.getSize().x, app.getSize().y);
    tex.update(app);
    RectangleShape rect;
    rect.setSize(Vector2f(w, h));
    rect.setTexture(&tex, true);
    rect.setTextureRect(IntRect(x, y, w, h));
    rect.setPosition(x, y);
    RenderStates states;
    states.shader = &contrastShader;
    contrastShader.setParameter("texture", tex);
    contrastShader.setParameter("luminosity", 1.f, 1.f, 1.f);
    app.draw(rect, states);
}

void warnShader(RenderWindow& app, int x, int y, int w, int h)
{
    static double time;
    time += I(1);
    Texture tex;
    tex.create(app.getSize().x, app.getSize().y);
    tex.update(app);
    RectangleShape rect;
    rect.setSize(Vector2f(w, h));
    rect.setTexture(&tex, true);
    rect.setTextureRect(IntRect(x, y, w, h));
    rect.setPosition(x, y);
    RenderStates states;
    states.shader = &contrastShader;
    contrastShader.setParameter("texture", tex);
    contrastShader.setParameter("luminosity", 1.f + 0.25f*sin(time), 0.75f, 0.75f);
    app.draw(rect, states);
}

} //namespace style

} //namespace tools
