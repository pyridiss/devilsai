
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

#include <cmath>

#include "tools/filesystem.h"
#include "tools/timeManager.h"
#include "gui/style.h"

namespace gui{

namespace style{

Font liberation;
Font liberationBold;
Font dayroman;

Shader blurShader;
Shader contrastShader;

void initStyle()
{
    liberation.loadFromFile(tools::filesystem::dataDirectory() + "LiberationSans-Regular.ttf");
    liberationBold.loadFromFile(tools::filesystem::dataDirectory() + "LiberationSans-Bold.ttf");
    dayroman.loadFromFile(tools::filesystem::dataDirectory() + "DayRoman.ttf");
    blurShader.loadFromFile(tools::filesystem::dataDirectory() + "blurShader.frag", Shader::Type::Fragment);
    contrastShader.loadFromFile(tools::filesystem::dataDirectory() + "contrastShader.frag", Shader::Type::Fragment);

    blurShader.setUniform("texture", Shader::CurrentTexture);
    contrastShader.setUniform("texture", Shader::CurrentTexture);
}

const Font& defaultTextFont()
{
    return liberation;
}

const Font& fontFromString(string s)
{
    if (s == "liberation") return liberation;
    if (s == "liberation-bold") return liberationBold;
    if (s == "dayroman") return dayroman;

    return defaultTextFont();
}

int defaultTextSize()
{
    return 10;
}

const Font& buttonTextFont()
{
    return liberationBold;
}

int buttonTextSize()
{
    return 11;
}

Color normalButtonTextColor()
{
    //Color 'brick red4' from Oxygen color palette
    return Color(226, 8, 0, 255);
}

Color activeButtonTextColor()
{
    //Color 'brick red3' from Oxygen color palette
    return Color(232, 87, 82, 255);
}

Color hoverButtonTextColor()
{
    //Color 'brick red5' from Oxygen color palette
    return Color(191, 3, 3, 255);
}

Color disabledButtonTextColor()
{
    return Color(100, 100, 100, 192);
}

const Shader* getContrastShader(float r, float g, float b)
{
    contrastShader.setUniform("luminosity", Glsl::Vec3(r, g, b));
    return &contrastShader;
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
    blurShader.setUniform("textureSize", Glsl::Vec2(tex.getSize().x, tex.getSize().y));
    app.draw(rect, states);
}

void useContrastShader(RenderWindow& app, int x, int y, int w, int h, float r, float g, float b)
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
    contrastShader.setUniform("luminosity", Glsl::Vec3(r, g, b));
    app.draw(rect, states);
}

void highlightShader(RenderWindow& app, int x, int y, int w, int h)
{
    useContrastShader(app, x, y, w, h, 1.f, 1.f, 1.f);
}

void disableShader(RenderWindow& app, int x, int y, int w, int h)
{
    useContrastShader(app, x, y, w, h, 0.25f, 0.25f, 0.25f);
}

void warnShader(RenderWindow& app, int x, int y, int w, int h)
{
    static double time;
    time += tools::timeManager::I(0.2);
    useContrastShader(app, x, y, w, h, 1.f + 0.25f*sin(time), 0.75f, 0.75f);
}

void displayShader(RenderWindow& app, string shader, int x, int y, int w, int h)
{
    if (shader == "textBackground")
        textBackgroundShader(app, x, y, w, h);
    else if (shader == "highlight")
        highlightShader(app, x, y, w, h);
    else if (shader == "disable")
        disableShader(app, x, y, w, h);
    else if (shader == "warn")
        warnShader(app, x, y, w, h);
}

} //namespace style

} //namespace gui
