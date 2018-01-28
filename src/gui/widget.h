
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
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Color.hpp>

#include "tools/signals.h"
#include "textManager/richText.h"

#include "gui/style.h"

using namespace std;
using namespace sf;

namespace gui{

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

        map<string, string> _embeddedData;

        bool _needsFocus = false;

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
        void addTextFlags(uint16_t newFlags);

        int left();
        int top();
        int getXCenter();
        int getYCenter();
        int width();
        int height();

        void setText(const textManager::PlainText& t);
        void setBackground(string b);
        void setBackgroundShader(string s);

        void addEmbeddedData(string name, string value);
        string embeddedData(string name);

        bool needsFocus();

        virtual bool mouseHovering(RenderWindow& app) = 0;
        virtual bool activated(RenderWindow& app, Event event) = 0;

        virtual void setValue(const string& d) = 0;
        virtual string value() = 0;

        virtual void display(RenderWindow& app);

        void show();
        void hide();
};

} //namespace gui

#endif // GUI_WIDGET_H

