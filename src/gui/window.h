
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

#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include <string>
#include <list>
#include <map>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <tools/signals.h>

using namespace std;
using namespace sf;

namespace gui{

class Widget;

class Window
{
    private:
        map < string, Widget* > widgets;
        list < tuple<string, string, string> > signals;
        list < pair<int, string> > keyboardSignals;
        list < string > exitWindowSignals;
        list < tuple<tools::signals::SignalListener, string, string> > signalListeners;

        bool exitWindow = false;

        int xTopLeft = 0, yTopLeft = 0;
        int xCenter = 0, yCenter = 0;
        bool xCenterOfScreen = false, yCenterOfScreen = false;
        int width = 0, height = 0;

        string backgroundImage;
        string backgroundFullscreenShader;
        string backgroundShader;
        string foregroundShader;
        string music;

    public:
        Window() = default;
        Window(string path, RenderWindow& app);
        ~Window();

    private:
        void setTopLeftCoordinates(int x, int y);
        void setCenterCoordinates(int x, int y);
        void setSize(int w, int h);

        int getXTopLeft();
        int getYTopLeft();
        int getXCenter();
        int getYCenter();

    public:
        void startWindow(RenderWindow& app);
        void display(RenderWindow& app);
        void manage(RenderWindow& app);
        void manage(RenderWindow& app, Event &event);
        Widget* widget(string name);
        const map<string,Widget*>& getWidgets();
        void setValue(const string& widget, const string& d);

        void loadFromFile(string path, RenderWindow& app);
};

} //namespace gui

#endif // GUI_WINDOW_H

