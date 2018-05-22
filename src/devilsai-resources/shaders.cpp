
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

#include "devilsai-resources/shaders.h"

#include "shader_definition.h"

#include <cmath>

#include <SFML/Graphics.hpp>

#include "tools/signals.h"
#include "tools/timeManager.h"
#include "imageManager/imageManager.h"

using namespace std;
using namespace sf;

namespace devilsai{

class BlurShader : public multimedia::ShaderInstance
{
    private:
        Shader _shader {};

    public:
        void load()
        {
            _shader.loadFromMemory(BLUR_SHADER, Shader::Type::Fragment);
        }
        ~BlurShader() = default;
        unsigned int createNewInstance()
        {
            return 0;
        }
        sf::Shader* shader([[maybe_unused]] unsigned int instance)
        {
            return &_shader;
        }
        void applyOnScreen(unsigned int instance, sf::RenderWindow& app, int x, int y, int w, int h)
        {
            Texture tex;
            tex.create(app.getSize().x, app.getSize().y);
            tex.update(app);

            RectangleShape rect;
            rect.setSize(Vector2f(w, h));
            rect.setTexture(&tex, true);
            rect.setTextureRect(IntRect(x, y, w, h));
            rect.setPosition(x, y);

            _shader.setUniform("textureSize", Glsl::Vec2(tex.getSize().x, tex.getSize().y));
            app.draw(rect, shader(instance));
        }
};

class ColorizeShader final : public multimedia::ShaderInstance
{
    private:
        struct Data
        {
            Vector3f red {};
            Vector3f green {};
            Vector3f blue {};
        };

        Shader _shader {};
        vector<Data> _instances {};

    public:
        void load()
        {
            _shader.loadFromMemory(COLORIZE_SHADER, Shader::Type::Fragment);
        }
        ~ColorizeShader() = default;
        unsigned int createNewInstance()
        {
            _instances.push_back(Data());
            return _instances.size() - 1;
        }
        void setData(unsigned int instance, Vector3f r, Vector3f g, Vector3f b)
        {
            if (instance >= _instances.size())
                return;

            _instances[instance].red = r;
            _instances[instance].green = g;
            _instances[instance].blue = b;
        }
        sf::Shader* shader(unsigned int instance)
        {
            if (instance >= _instances.size())
                return nullptr;

            _shader.setUniform("red", _instances[instance].red);
            _shader.setUniform("green", _instances[instance].green);
            _shader.setUniform("blue", _instances[instance].blue);

            return &_shader;
        }
        void applyOnScreen(unsigned int instance, sf::RenderWindow& app, int x, int y, int w, int h)
        {
            Texture tex;
            tex.create(app.getSize().x, app.getSize().y);
            tex.update(app);

            RectangleShape rect;
            rect.setSize(Vector2f(w, h));
            rect.setTexture(&tex, true);
            rect.setTextureRect(IntRect(x, y, w, h));
            rect.setPosition(x, y);

            app.draw(rect, shader(instance));
        }
};

class ContrastShader : public multimedia::ShaderInstance
{
    private:
        struct Data
        {
            Glsl::Vec3 luminosity = Glsl::Vec3(0, 0, 0);
        };

        Shader _shader {};
        vector<Data> _instances {};

    public:
        void load()
        {
            _shader.loadFromMemory(CONTRAST_SHADER, Shader::Type::Fragment);
        }
        ~ContrastShader() = default;
        unsigned int createNewInstance()
        {
            _instances.push_back(Data());
            return _instances.size() - 1;
        }
        void setData(unsigned int instance, Glsl::Vec3 l)
        {
            if (instance >= _instances.size())
                return;

            _instances[instance].luminosity = l;
        }
        sf::Shader* shader(unsigned int instance)
        {
            if (instance >= _instances.size())
                return nullptr;

            _shader.setUniform("luminosity", _instances[instance].luminosity);
            return &_shader;
        }
        void applyOnScreen(unsigned int instance, sf::RenderWindow& app, int x, int y, int w, int h)
        {
            Texture tex;
            tex.create(app.getSize().x, app.getSize().y);
            tex.update(app);

            RectangleShape rect;
            rect.setSize(Vector2f(w, h));
            rect.setTexture(&tex, true);
            rect.setTextureRect(IntRect(x, y, w, h));
            rect.setPosition(x, y);

            app.draw(rect, shader(instance));
        }
};

class FadeShader : public multimedia::ShaderInstance
{
    private:
        struct Data
        {
            double timer = 255;
        };

        Shader _shader {};
        vector<Data> _instances {};

    public:
        void load()
        {
            _shader.loadFromMemory(FADE_SHADER, Shader::Type::Fragment);
        }
        ~FadeShader() = default;
        unsigned int createNewInstance()
        {
            _instances.push_back(Data());
            return _instances.size() - 1;
        }
        sf::Shader* shader(unsigned int instance)
        {
            if (instance >= _instances.size())
                return nullptr;

            if (_instances[instance].timer > 0)
                _instances[instance].timer -= tools::timeManager::I((256.0-_instances[instance].timer)/24.0);

            _shader.setUniform("value", (float)(_instances[instance].timer/256.0));

            if (_instances[instance].timer/256.0 < 0)
            {
                tools::signals::addSignal("fade-shader:end");
                _instances[instance].timer = 255.0;
            }
            return &_shader;
        }
        void applyOnScreen(unsigned int instance, sf::RenderWindow& app, int x, int y, int w, int h)
        {
            Texture tex;
            tex.create(app.getSize().x, app.getSize().y);
            tex.update(app);

            RectangleShape rect;
            rect.setSize(Vector2f(w, h));
            rect.setTexture(&tex, true);
            rect.setTextureRect(IntRect(x, y, w, h));
            rect.setPosition(x, y);

            app.draw(rect, shader(instance));
        }
};

class WarnShader : public multimedia::ShaderInstance
{
    private:
        struct Data
        {
            double timer = 0;
        };

        Shader _shader {};
        vector<Data> _instances {};

    public:
        void load()
        {
            _shader.loadFromMemory(CONTRAST_SHADER, Shader::Type::Fragment);
        }
        ~WarnShader() = default;
        unsigned int createNewInstance()
        {
            _instances.push_back(Data());
            return _instances.size() - 1;
        }
        sf::Shader* shader(unsigned int instance)
        {
            if (instance >= _instances.size())
                return nullptr;

            _instances[instance].timer += tools::timeManager::I(0.2);

            _shader.setUniform("luminosity", Glsl::Vec3(1.0 + 0.25 * sin(_instances[instance].timer), 0.75, 0.75));;
            return &_shader;
        }
        void applyOnScreen(unsigned int instance, sf::RenderWindow& app, int x, int y, int w, int h)
        {
            Texture tex;
            tex.create(app.getSize().x, app.getSize().y);
            tex.update(app);

            RectangleShape rect;
            rect.setSize(Vector2f(w, h));
            rect.setTexture(&tex, true);
            rect.setTextureRect(IntRect(x, y, w, h));
            rect.setPosition(x, y);

            app.draw(rect, shader(instance));
        }
};

static BlurShader Blur;
static ColorizeShader Colorize;
static ContrastShader Contrast;
static FadeShader Fade;
static WarnShader Warn;

unsigned int newColorizeShaderInstance(Glsl::Vec3 v1, Glsl::Vec3 v2, Glsl::Vec3 v3)
{
    unsigned int id = Colorize.createNewInstance();
    Colorize.setData(id, v1, v2, v3);
    return id;
}

unsigned int newContrastShaderInstance(Glsl::Vec3 v)
{
    unsigned int id = Contrast.createNewInstance();
    Contrast.setData(id, v);
    return id;
}

void initShaders()
{
    Blur.load();
    wearableItemShaderInstance = Blur.createNewInstance();
    multimedia::addShader("blur", &Blur);

    Colorize.load();
    multimedia::addShader("colorize", &Colorize);

    Contrast.load();
    storageBoxShaderInstance = newContrastShaderInstance(Glsl::Vec3(1.8, 1.8, 1.8));
    skillPanelShaderInstance = newContrastShaderInstance(Glsl::Vec3(0.1, 0.1, 0.1));
    multimedia::addShader("contrast", &Contrast);

    Fade.load();
    multimedia::addShader("fade", &Fade);

    Warn.load();
    multimedia::addShader("warn", &Warn);
}

}  // namespace devilsai
