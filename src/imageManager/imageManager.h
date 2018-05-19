
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

#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <string>

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

using std::string;

namespace sf{
    class Font;
    class RenderTarget;
    class RenderWindow;
    class Shader;
}

namespace multimedia{

// Assets

/**
 * \brief Base class for shaders.
 *
 * The ShaderInstance class and the associated helpers are used to store shaders and use them through
 * a common interface.
 * The multimedia library does not define any shader: a shader must be defined by derivating the
 * ShaderInstance class.
 */
class ShaderInstance
{
    public:

        /**
         * Destructor.
         */
        virtual ~ShaderInstance()
        {
        }

        /**
         * Create a new set of data to which the user can refer with the identifier returned.
         *
         * \return identifier to pass to other functions.
         */
        virtual unsigned int createNewInstance() = 0;

        /**
         * Return the sf::Shader, already parameterized with the data of the instance.
         *
         * \param instance the instance to use.
         * \return pointer to the sf::Shader.
         */
        virtual sf::Shader* shader(unsigned int instance) = 0;

        /**
         * Apply the shader on a part of the screen.
         *
         * \param instance the instance to use.
         * \param app reference to the window in which the shader should be applied.
         * \param x x-value of the top-left corner of the rectangle.
         * \param y y-value of the top-left corner of the rectangle.
         * \param w width of the rectangle.
         * \param h height of the rectangle.
         */
        virtual void applyOnScreen(unsigned int instance, sf::RenderWindow& app, int x, int y, int w, int h) = 0;
};

void addFont(const string& name, const string& file);
sf::Font* font(const string& name);

/**
 * Add a new shader to the list of known shaders.
 *
 * \param name the name of the shader.
 * \param i non-owning pointer to the new shader, which can be of any class derived from ShaderInstance.
 */
void addShader(const string& name, ShaderInstance* i);

/**
 * Calls the createNewInstance() function of the shader \a name.
 *
 * An instance stores the data needed to define the varying and uniform variables of a shader.
 *
 * \param name the name of the shader.
 * \return an identifier that will be necessary to apply the shader with the good data.
 */
unsigned int createShaderInstance(const string& name);

/**
 * Returns a pointer to the sf::Shader that can directly be used in graphic functions.
 *
 * The shader is already parameterized which the data of the instance.
 *
 * \param name the name of the shader.
 * \param instance the instance of the shader.
 * \return pointer to the sf::Shader.
 */
sf::Shader* shader(const string& name, unsigned int instance);

/**
 * Calls the applyOnScreen() function of the shader \a name.
 */
void applyShaderOnScreen(sf::RenderWindow& app, const string& shaderName, unsigned int instance, int x, int y, int w, int h);

}  // namespace multimedia

namespace imageManager{

class Image;
class Animation;

void addContainer(unsigned int container);
void addImage(unsigned int container, const string& key, const string& file, sf::Vector2i of = sf::Vector2i(0, 0), sf::Shader* sh = nullptr);
void addArchiveFile(string path);
string getCurrentArchiveFile();
void removeArchiveFile(string path);
imageManager::Image* getImage(unsigned int container, const string& key);
void changeHSL(unsigned int container, const string& key, double h, double s, double l);
void display(sf::RenderTarget& target, unsigned int container, const string& key, float x, float y, bool atCenter = false, const sf::Shader* shader = nullptr);
void displayStretched(sf::RenderTarget& target, unsigned int container, const string& key, double x, double y, double w, double h, bool atCenter = false, const sf::Shader* shader = nullptr);
void addAnimation(const string& name, const string& file);
imageManager::Animation* getAnimation(const string& name);
void lockGLMutex(int id);
void unlockGLMutex(int id);

} //namespace imageManager

#endif //IMAGEMANAGER_H
