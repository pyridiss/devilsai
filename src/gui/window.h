
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
#include "gui/widget.h"

using namespace std;
using namespace sf;

namespace gui{

enum EventTypes { WidgetActivated, WidgetValueChanged, KeyPressed, KeyReleased, KeyHeld, SignalCaptured, NoEvent };
enum Actions { SendSignal, ModifyValue, ModifyEmbeddedData, ExitWindow, Enable, Disable, Show, Hide, Focus, Unfocus, NoAction };

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
            Trigger(const Trigger& other)
              : sender(other.sender),
                dataProvider(other.dataProvider),
                target(other.target),
                event(other.event),
                eventData(other.eventData),
                action(other.action),
                signal(other.signal),
                dataName(other.dataName)
            {
            }
            Trigger& operator=(const Trigger& right)
            {
                sender = right.sender;
                dataProvider = right.dataProvider;
                target = right.target;
                event = right.event;
                eventData = right.eventData;
                action = right.action;
                signal = right.signal;
                dataName = right.dataName;

                return *this;
            }
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
            WindowEvent(const WindowEvent& other)
              : widget(other.widget),
                event(other.event),
                data(other.data)
            {
            }
            WindowEvent& operator=(const WindowEvent& right)
            {
                widget = right.widget;
                event = right.event;
                data = right.data;

                return *this;
            }
        };

    private:
        map < string, Widget* > widgets;
        map < string, Window* > _subwindows;
        vector<Trigger> _triggers;
        vector<WindowEvent> _events;
        vector<Keyboard::Key> _watchedKeys;
        queue<tools::signals::Signal> _capturedSignals;

        bool exitWindow = false;

        RenderWindow* _screen;

        Widget* _focusedWidget;

        string backgroundFullscreenShader;
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
        void manage(RenderWindow& app);
        void manage(RenderWindow& app, Event &event);
        void checkTriggers();
        Widget* widget(string name);
        const map<string,Widget*>& getWidgets();
        void setValue(optionType v);
        void setValue(const string& widget, optionType v);
        void addEvent(Widget* s, EventTypes e, optionType d);
        void askFocus(Widget* w, bool value);

        bool mouseHovering(RenderWindow& app);
        bool activated(RenderWindow& app, Event event);

        void loadFromFile(string path);
        void loadFromXML(tinyxml2::XMLElement* elem);
};

} //namespace gui

#endif // GUI_WINDOW_H

