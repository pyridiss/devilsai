
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2016  Quentin Henriet

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

#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H

#include <string>
#include <map>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "tools/signals.h"
#include "tools/variant.h"
#include "textManager/plainText.h"
#include "textManager/richText.h"

using namespace std;
using namespace sf;

namespace tinyxml2{
    class XMLElement;
}

namespace gui{

typedef tools::math::Variant<string, textManager::PlainText, Keyboard::Key, float, tools::signals::Signal> optionType;

class Window;

class Widget
{
    protected:
        int _x, _y;
        int _width, _height;
        uint32_t _flags;
        uint16_t _textFlags;
        Window* _parent;

        textManager::RichText _text;
        string _background;
        string _backgroundShader;

        map<string, optionType> _embeddedData;

    public:
        Widget();
        Widget(const Widget& other);
        Widget& operator=(const Widget& right);
        virtual ~Widget() = default;

    protected:
        void displayBackground(RenderWindow& app);
        void displayText(RenderWindow& app);

    public:
        void setPosition(int x, int y);
        void setParent(Window* p);
        void setSize(int w, int h);
        void addFlags(uint32_t newFlags);
        void removeFlags(uint32_t newFlags);
        bool hasFlags(uint32_t flags);
        void addTextFlags(uint16_t newFlags);

        int left();
        int top();
        int getXCenter();
        int getYCenter();
        int width();
        int height();

        void setText(const textManager::PlainText& t);

        template<typename T>
        void addEmbeddedData(string name, const T& value);
        void addEmbeddedData(string name, const optionType& value);
        template<typename T>
        T& embeddedData(const string& name);
        optionType& embeddedData(const string& name);

        virtual bool mouseHovering() = 0;
        virtual bool activated(Event event) = 0;

        virtual void setValue(optionType v) = 0;

        virtual void display(RenderWindow& app);

        void loadFromXMLElement(tinyxml2::XMLElement* elem);

        void show();
        void hide();
};

} //namespace gui

#endif // GUI_WIDGET_H

