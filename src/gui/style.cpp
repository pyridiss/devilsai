
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

#include <cmath>
#include <unordered_map>

#include "tools/debug.h"
#include "tools/filesystem.h"
#include "tools/math.h"
#include "tools/signals.h"
#include "tools/timeManager.h"
#include "tools/variant.h"

using namespace std;
using namespace sf;

namespace gui{

Vector2i MousePosition;


typedef tools::math::Variant<int, string, Color> Option;
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

namespace style{

Shader blurShader;
Shader contrastShader;
Shader colorizeShader;
Shader fadeShader;

RenderTexture TemporaryTexture;
RenderWindow* MainWindow = nullptr;

void initStyle(RenderWindow* w)
{
    blurShader.loadFromFile(tools::filesystem::dataDirectory() + "blurShader.frag", Shader::Type::Fragment);
    contrastShader.loadFromFile(tools::filesystem::dataDirectory() + "contrastShader.frag", Shader::Type::Fragment);
    colorizeShader.loadFromFile(tools::filesystem::dataDirectory() + "colorizeShader.frag", Shader::Type::Fragment);
    fadeShader.loadFromFile(tools::filesystem::dataDirectory() + "fadeShader.frag", Shader::Type::Fragment);

    blurShader.setUniform("texture", Shader::CurrentTexture);
    contrastShader.setUniform("texture", Shader::CurrentTexture);
    colorizeShader.setUniform("texture", Shader::CurrentTexture);
    fadeShader.setUniform("texture", Shader::CurrentTexture);

    MainWindow = w;
    if (w != nullptr)
        TemporaryTexture.create(w->getSize().x, w->getSize().y);
    TemporaryTexture.setSmooth(true);

}

const Shader* getContrastShader(float r, float g, float b)
{
    contrastShader.setUniform("luminosity", Glsl::Vec3(r, g, b));
    return &contrastShader;
}

const Shader* getColorizeShader(Vector3f r, Vector3f g, Vector3f b)
{
    colorizeShader.setUniform("red", r);
    colorizeShader.setUniform("green", g);
    colorizeShader.setUniform("blue", b);
    return &colorizeShader;
}

const Shader* getFadeShader(float& value)
{
    if (value > 0)
        value -= tools::timeManager::I((256-value)/24);

    fadeShader.setUniform("value", value/256.f);

    if (value < 0)
        tools::signals::addSignal("fade-shader:end");

    return &fadeShader;
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

RenderTexture& temporaryTexture()
{
    if (MainWindow->getSize() != TemporaryTexture.getSize())
        TemporaryTexture.create(MainWindow->getSize().x, MainWindow->getSize().y);

    return TemporaryTexture;
}

} //namespace style

void updateMousePosition(RenderWindow & app)
{
    MousePosition = Mouse::getPosition(app);
}

const Vector2i& mousePosition()
{
    return MousePosition;
}

} //namespace gui
