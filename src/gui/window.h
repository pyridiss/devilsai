
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

#include "tools/signals.h"

#include "gui/style.h"

using namespace std;
using namespace sf;

namespace gui{

class Widget;

class Window
{
    private:
        enum keyboardEvents { onKeyPressed, onKeyReleased, whileKeyPressed };

    private:
        map < string, Widget* > widgets;
        list < tuple<string, string, string> > signals;
        list < tuple<int, keyboardEvents, string, string> > keyboardSignals;
        list < string > exitWindowSignals;
        list < tuple<tools::signals::SignalListener, string, string> > signalListeners;

        bool exitWindow = false;

        int _x, _y;
        int _width, _height;
        uint16_t _flags;

        RenderWindow* _screen;

        string backgroundImage;
        string backgroundFullscreenShader;
        string backgroundShader;
        string foregroundShader;
        string music;

    public:
        Window();
        Window(string path, RenderWindow& window);
        Window(const Window& other);
        Window& operator=(const Window& right);
        ~Window();

    public:
        int left();
        int top();
        int width();
        int height();
        const RenderWindow* screen();
        void startWindow(RenderWindow& app);
        void display(RenderWindow& app);
        void manage(RenderWindow& app);
        void manage(RenderWindow& app, Event &event);
        void checkKeyboardState();
        Widget* widget(string name);
        const map<string,Widget*>& getWidgets();
        void setValue(const string& widget, const string& d);

        void loadFromFile(string path);
};

} //namespace gui

#endif // GUI_WINDOW_H

