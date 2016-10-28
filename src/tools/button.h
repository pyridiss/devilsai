
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

#ifndef BUTTON_H
#define BUTTON_H

#include <string>

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

namespace tools{

class Button
{
    struct minimalistButton
    {
        Text text;
        string background;
    };
    enum States
    {
        Normal,
        Active,
        Hover,
        Disabled
    };

	private:
        int xTopLeft = 0, yTopLeft = 0;
        int xCenter = 0, yCenter = 0;
        int width = 0, height = 0;

        minimalistButton normal;
        minimalistButton active;
        minimalistButton hover;
        minimalistButton disabled;

		States state = Normal;

	public:
        void setTopLeftCoordinates(int x, int y);
        void setCenterCoordinates(int x, int y);
        void setSize(int w, int h);

        void setNormalText(Text t);
		void setActiveText(Text t);
        void setHoverText(Text t);
        void setDisabledText(Text t);

        void setNormalBackground(string b);
        void setActiveBackground(string b);
        void setHoverBackground(string b);
        void setDisabledBackground(string b);

        void applyDefaultColors();
        void applyDefaultFont();

        bool mouseHovering(RenderWindow& app);
        bool activated(RenderWindow& app, Event::EventType event);

        void display(RenderWindow app);
};

}
#endif // BUTTON_H

