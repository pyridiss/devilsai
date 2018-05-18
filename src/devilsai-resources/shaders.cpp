
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

#include <cmath>

#include <SFML/Graphics.hpp>

#include "tools/filesystem.h"
#include "tools/signals.h"
#include "tools/timeManager.h"
#include "imageManager/imageManager.h"

using namespace std;
using namespace sf;

namespace devilsai{

class BlurShader : public multimedia::ShaderInstance
{
    private:
        Shader _shader;

    public:
        BlurShader()
          : _shader()
        {
            _shader.loadFromFile(tools::filesystem::dataDirectory() + "blurShader.frag", Shader::Type::Fragment);
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

class ContrastShader : public multimedia::ShaderInstance
{
    private:
        struct Data
        {
            Glsl::Vec3 luminosity = Glsl::Vec3(0, 0, 0);
        };

        Shader _shader;
        vector<Data> _instances;

    public:
        ContrastShader()
          : _shader(),
            _instances()
        {
            _shader.loadFromFile(tools::filesystem::dataDirectory() + "contrastShader.frag", Shader::Type::Fragment);
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

        Shader _shader;
        vector<Data> _instances;

    public:
        FadeShader()
          : _shader(),
            _instances()
        {
            _shader.loadFromFile(tools::filesystem::dataDirectory() + "fadeShader.frag", Shader::Type::Fragment);
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

        Shader _shader;
        vector<Data> _instances;

    public:
        WarnShader()
          : _shader(),
            _instances()
        {
            _shader.loadFromFile(tools::filesystem::dataDirectory() + "contrastShader.frag", Shader::Type::Fragment);
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

void initShaders()
{
    BlurShader* blur = new BlurShader();
    wearableItemShaderInstance = blur->createNewInstance();
    multimedia::addShader("blur", blur);

    ContrastShader* contrast = new ContrastShader();
    guiBackgroundDisabledShaderInstance = contrast->createNewInstance();
    guiBackgroundMouseoverShaderInstance = contrast->createNewInstance();
    guiTextDisabledShaderInstance = contrast->createNewInstance();
    guiTextMouseoverShaderInstance = contrast->createNewInstance();
    storageBoxShaderInstance = contrast->createNewInstance();
    skillPanelShaderInstance = contrast->createNewInstance();
    contrast->setData(guiBackgroundDisabledShaderInstance, Glsl::Vec3(0.25, 0.25, 0.25));
    contrast->setData(guiBackgroundMouseoverShaderInstance, Glsl::Vec3(1.0, 1.0, 1.0));
    contrast->setData(guiTextDisabledShaderInstance, Glsl::Vec3(0.25, 0.25, 0.25));
    contrast->setData(guiTextMouseoverShaderInstance, Glsl::Vec3(1.0, 0.0, 0.0));
    contrast->setData(storageBoxShaderInstance, Glsl::Vec3(1.8, 1.8, 1.8));
    contrast->setData(skillPanelShaderInstance, Glsl::Vec3(0.1, 0.1, 0.1));
    multimedia::addShader("contrast", contrast);

    FadeShader* fade = new FadeShader();
    multimedia::addShader("fade", fade);

    WarnShader* warn = new WarnShader();
    multimedia::addShader("warn", warn);
}

}  // namespace devilsai