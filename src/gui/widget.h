
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

using namespace std;
using namespace sf;

typedef basic_string<Uint32> String32;

namespace gui{

class Widget
{
    protected:
        struct minimalistWidget
        {
            Text text;
            string background;
            void (*backgroundShader)(RenderWindow&, int, int, int, int) = nullptr;
            void (*foregroundShader)(RenderWindow&, int, int, int, int) = nullptr;
        };

    protected:
        int xTopLeft = 0, yTopLeft = 0;
        int xCenter = 0, yCenter = 0;
        int xOrigin = 0, yOrigin = 0;
        int width = 0, height = 0;

        map < string, minimalistWidget> states;

        string currentState = "normal";

    public:
        virtual ~Widget() = default;

    public:
        void setTopLeftCoordinates(int x, int y);
        void setCenterCoordinates(int x, int y);
        void setOriginCoordinates(int x, int y);
        void setSize(int w, int h);

        void setDisabled(bool d);
        void setActive(bool a);

        int getXTopLeft();
        int getYTopLeft();
        int getXCenter();
        int getYCenter();

        void setAllText(String32& t);
        void setAllBackground(string b);

        void setText(string state, String32& t);
        void setTextFont(const Font& f, float s);
        void setTextColor(string state, Color c);
        void addOffsetToText(string state, int x, int y);
        void setBackground(string state, string b);
        void setBackgroundShader(string state, void (*s)(RenderWindow&, int, int, int, int));
        void setForegroundShader(string state, void (*s)(RenderWindow&, int, int, int, int));

        void updateTextPosition();
        void updateSize();

        virtual bool mouseHovering(RenderWindow& app) = 0;
        virtual bool activated(RenderWindow& app, Event event) = 0;

        virtual void setData(tools::signals::SignalData& data) = 0;
        virtual void display(RenderWindow& app) = 0;
};

} //namespace gui

#endif // GUI_WIDGET_H

