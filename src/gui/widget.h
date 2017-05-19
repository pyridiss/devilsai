
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

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Color.hpp>


using namespace std;
using namespace sf;

typedef basic_string<Uint32> String32;

namespace gui{

class Widget
{
    struct minimalistWidget
    {
        Text text;
        string background;
        void (*shader)(RenderWindow&, int, int, int, int) = NULL;
    };

    protected:
        enum States
        {
            Normal,
            Active,
            Hover,
            Disabled
        };

    protected:
        int xTopLeft = 0, yTopLeft = 0;
        int xCenter = 0, yCenter = 0;
        int width = 0, height = 0;

        minimalistWidget normal;
        minimalistWidget active;
        minimalistWidget hover;
        minimalistWidget disabled;

        States state = Normal;

    public:
        void setTopLeftCoordinates(int x, int y);
        void setCenterCoordinates(int x, int y);
        void setSize(int w, int h);

        void setDisabled(bool d);
        void setActive(bool a);

        int getXTopLeft();
        int getYTopLeft();
        int getXCenter();
        int getYCenter();

        void setAllText(String32& t);
        void setNormalText(String32& t);
        void setActiveText(String32& t);
        void setHoverText(String32& t);
        void setDisabledText(String32& t);

        void setTextFont(const Font& f, float s);

        void setNormalTextColor(Color c);
        void setActiveTextColor(Color c);
        void setHoverTextColor(Color c);
        void setDisabledTextColor(Color c);

        void addOffsetToNormalText(int x, int y);
        void addOffsetToActiveText(int x, int y);
        void addOffsetToHoverText(int x, int y);
        void addOffsetToDisabledText(int x, int y);

        void setAllBackground(string b);
        void setNormalBackground(string b);
        void setActiveBackground(string b);
        void setHoverBackground(string b);
        void setDisabledBackground(string b);

        void setNormalShader(void (*s)(RenderWindow&, int, int, int, int));
        void setActiveShader(void (*s)(RenderWindow&, int, int, int, int));
        void setHoverShader(void (*s)(RenderWindow&, int, int, int, int));
        void setDisabledShader(void (*s)(RenderWindow&, int, int, int, int));

        virtual bool mouseHovering(RenderWindow& app) = 0;
        virtual bool activated(RenderWindow& app, Event::EventType event) = 0;

        virtual void display(RenderWindow& app) = 0;
};

} //namespace gui

#endif // GUI_WIDGET_H

