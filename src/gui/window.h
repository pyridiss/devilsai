
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
#include <vector>
#include <queue>

#include "gui/widget.h"

using namespace std;

namespace gui{

enum EventTypes { WidgetActivated, WidgetValueChanged, KeyPressed, KeyReleased, KeyHeld, SignalCaptured, NoEvent };
enum Actions { SendSignal, ModifyValue, ModifyEmbeddedData, Enable, Disable, Show, Hide, Focus, Unfocus, NoAction };

class Window : public Widget
{
    private:
        struct Trigger
        {
            Widget *sender, *dataProvider, *target;
            EventTypes event;
            optionType eventData;
            Actions action;
            string signal;
            string dataName;

            Trigger()
              : sender(nullptr),
                dataProvider(nullptr),
                target(nullptr),
                event(NoEvent),
                eventData(),
                action(NoAction),
                signal(),
                dataName()
            {
            }
            Trigger(const Trigger& other) = delete;
            Trigger(Trigger&& other) = default;
            Trigger& operator=(const Trigger& right) = delete;
            Trigger& operator=(Trigger&& right) = delete;
        };
        struct WindowEvent
        {
            Widget* widget;
            EventTypes event;
            optionType data;

            WindowEvent()
              : widget(nullptr),
                event(NoEvent),
                data()
            {
            }
            WindowEvent(Widget* s, EventTypes e, optionType d)
              : widget(s),
                event(e),
                data(d)
            {
            }
            WindowEvent(const WindowEvent& other) = delete;
            WindowEvent(WindowEvent&& other) = default;
            WindowEvent& operator=(const WindowEvent& right) = delete;
            WindowEvent& operator=(WindowEvent&& right) = delete;
        };

    private:
        map < string, Widget* > widgets;
        map < string, Window* > _subwindows;
        vector<Trigger> _triggers;
        vector<WindowEvent> _events;
        vector<Keyboard::Key> _watchedKeys;
        queue<tools::signals::Signal> _capturedSignals;

        RenderWindow* _screen;

        Widget* _focusedWidget;

    public:
        string music;

    public:
        Window();
        Window(string path, RenderWindow& window);
        Window(const Window& other) = delete;
        Window& operator=(const Window& right) = delete;
        ~Window();

    public:
        int left();
        int top();
        int width();
        int height();
        const RenderWindow* screen();
        void startWindow(RenderWindow& app);
        void display(RenderWindow& app);
        void manage(Event &event);
        void checkTriggers();
        Widget* widget(string name);
        const map<string,Widget*>& getWidgets();
        void setValue(optionType v);
        void setValue(const string& w, optionType v);
        void addEvent(Widget* s, EventTypes e, optionType d);
        void askFocus(Widget* w, bool value);

        bool mouseHovering();
        bool activated(Event event);

        void loadFromFile(string path);
        void loadFromXML(tinyxml2::XMLElement* elem);
};

} //namespace gui

#endif // GUI_WINDOW_H

