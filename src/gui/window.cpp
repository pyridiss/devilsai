
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2017  Quentin Henriet

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

#include <tinyxml2.h>

#include "tools/filesystem.h"
#include "tools/signals.h"
#include "tools/timeManager.h"
#include "tools/textManager.h"

#include "imageManager/imageManager.h"

#include "musicManager/musicManager.h"

#include "gui/window.h"
#include "gui/textWidget.h"
#include "gui/button.h"
#include "gui/inputField.h"
#include "gui/dropDownList.h"
#include "gui/style.h"

using namespace tinyxml2;

namespace gui{

Window::Window(string path, RenderWindow& app) : Window()
{
    loadFromFile(path, app);
}

Window::~Window()
{
    for (auto& w : widgets)
        delete w.second;
}

void Window::setTopLeftCoordinates(int x, int y)
{
    xTopLeft = x;
    yTopLeft = y;
    xCenter = -1;
    yCenter = -1;
}

void Window::setCenterCoordinates(int x, int y)
{
    xCenter = x;
    yCenter = y;
    xTopLeft = -1;
    yTopLeft = -1;
}

void Window::setSize(int w, int h)
{
    width = w;
    height = h;
}

int Window::getXTopLeft()
{
    if (xTopLeft == -1)
        return xCenter - width/2;

    return xTopLeft;
}

int Window::getYTopLeft()
{
    if (yTopLeft == -1)
        return yCenter - height/2;

    return yTopLeft;
}

int Window::getXCenter()
{
    if (xCenter == -1)
        return xTopLeft + width/2;

    return xCenter;
}

int Window::getYCenter()
{
    if (yCenter == -1)
        return yTopLeft + height/2;

    return yCenter;
}

void Window::startWindow(RenderWindow& app)
{
    exitWindow = false;

    if (xCenterOfScreen) setCenterCoordinates(app.getSize().x / 2, getYCenter());
    if (yCenterOfScreen) setCenterCoordinates(getXCenter(), app.getSize().y / 2);

    for (auto& widget : widgets)
    {
        widget.second->setOriginCoordinates(getXTopLeft(), getYTopLeft());
        widget.second->updateTextPosition();
    }
}

void Window::display(RenderWindow& app)
{
    for (auto& l : signalListeners)
    {
        if (get<0>(l).signalSent)
        {
            auto i = widgets.find(get<1>(l));
            if (i != widgets.end())
            {
                i->second->setCurrentState(get<2>(l));
                get<0>(l).signalSent = false;
            }
        }
    }

    if (!backgroundImage.empty())
        imageManager::display(app, "misc", backgroundImage, getXTopLeft(), getYTopLeft());

    Widget* priorityWidget = nullptr;
    for (auto& widget : widgets)
    {
        if (widget.second->needsFocus())
            priorityWidget = widget.second;
        else
            widget.second->display(app);
    }
    if (priorityWidget != nullptr)
        priorityWidget->display(app);
}

void Window::manage(RenderWindow& app)
{
    startWindow(app);

    if (!music.empty())
        musicManager::playMusic(music);

    if (!backgroundFullscreenShader.empty())
        gui::style::displayShader(app, backgroundFullscreenShader, 0, 0, app.getSize().x, app.getSize().y);

    if (!backgroundShader.empty())
        gui::style::displayShader(app, backgroundShader, getXTopLeft(), getYTopLeft(), width, height);

    while (!exitWindow)
    {
        Event event;
        while (app.pollEvent(event))
        {
            manage(app, event);
        }

        display(app);

        if (!foregroundShader.empty())
            gui::style::displayShader(app, foregroundShader, getXTopLeft(), getYTopLeft(), width, height);

        tools::timeManager::frameDone();
        app.display();
        musicManager::manageRunningMusics();
    }
}

void Window::manage(RenderWindow& app, Event &event)
{
    Widget* priorityWidget = nullptr;

    for (auto& widget : widgets)
    {
        if (widget.second->needsFocus())
        {
            priorityWidget = widget.second;
        }
    }

    for (auto& widget : widgets)
    {
        if (priorityWidget != nullptr && widget.second != priorityWidget)
            continue;

        if (widget.second->activated(app, event))
        {
            for (auto& signal : signals)
            {
                if (get<0>(signal) == widget.first)
                {
                    tools::signals::SignalData d;

                    if (!get<2>(signal).empty() && widgets.find(get<2>(signal)) != widgets.end())
                        widgets.find(get<2>(signal))->second->setData(d);

                    tools::signals::addSignal(get<1>(signal), d);
                }
            }
            for (auto& exitWindowSignal : exitWindowSignals)
            {
                if (exitWindowSignal == widget.first)
                {
                    exitWindow = true;
                }
            }
        }
    }

    if (event.type == Event::KeyReleased)
        for (auto& k : keyboardSignals)
        {
            if (event.key.code == k.first) tools::signals::addSignal(k.second);
        }
}

void Window::setValue(string widget, const tools::signals::SignalData& d)
{
    auto i = widgets.find(widget);
    if (i != widgets.end())
    {
        i->second->setValue(d);
    }
}

void Window::loadFromFile(string path, RenderWindow& app)
{
    path = tools::filesystem::dataDirectory() + path;

    XMLDocument file;
    file.LoadFile(path.c_str());

    XMLHandle hdl(file);
    XMLElement *elem = hdl.FirstChildElement().FirstChildElement().ToElement();

    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "properties")
        {
            if (elem->Attribute("xTopLeft"))
            {
                int x = 0, y = 0;
                elem->QueryAttribute("xTopLeft", &x);
                elem->QueryAttribute("yTopLeft", &y);
                setTopLeftCoordinates(x, y);
            }
            if (elem->Attribute("xCenter"))
            {
                int x = 0, y = 0;
                elem->QueryAttribute("xCenter", &x);
                elem->QueryAttribute("yCenter", &y);
                setCenterCoordinates(x, y);
            }
            if (elem->Attribute("xCenterOfScreen"))
            {
                xCenterOfScreen = true;
            }
            if (elem->Attribute("yCenterOfScreen"))
            {
                yCenterOfScreen = true;
            }
            if (elem->Attribute("width"))
            {
                int w = 0, h = 0;
                elem->QueryAttribute("width", &w);
                elem->QueryAttribute("height", &h);
                setSize(w, h);
            }
            if (elem->Attribute("fullscreen"))
            {
                setTopLeftCoordinates(0, 0);
                setSize(app.getSize().x, app.getSize().y);
            }

            if (elem->Attribute("backgroundImage"))
                backgroundImage = elem->Attribute("backgroundImage");

            if (elem->Attribute("backgroundFullscreenShader"))
                backgroundFullscreenShader = elem->Attribute("backgroundFullscreenShader");

            if (elem->Attribute("backgroundShader"))
                backgroundShader = elem->Attribute("backgroundShader");

            if (elem->Attribute("foregroundShader"))
                foregroundShader = elem->Attribute("foregroundShader");

            if (elem->Attribute("music"))
                music = elem->Attribute("music");
        }

        if (elemName == "widget")
        {
            string widgetName = elem->Attribute("name");
            string type = elem->Attribute("type");
            Widget* widget = nullptr;
            if (type == "text")         widget = new TextWidget();
            if (type == "button")       widget = new Button();
            if (type == "input-field")  widget = new InputField();
            if (type == "drop-down-list") widget = new DropDownList();

            widgets.insert(map<string, Widget*>::value_type(widgetName, widget));

            if (elem->Attribute("xTopLeft"))
            {
                int x = 0, y = 0;
                elem->QueryAttribute("xTopLeft", &x);
                elem->QueryAttribute("yTopLeft", &y);
                widget->setTopLeftCoordinates(x, y);
            }
            if (elem->Attribute("xCenter") || elem->Attribute("yCenter") || elem->Attribute("xWindowCenter") || elem->Attribute("yWindowCenter"))
            {
                int x = 0, y = 0;
                elem->QueryAttribute("xCenter", &x);
                elem->QueryAttribute("yCenter", &y);
                if (x == 0 && elem->Attribute("xWindowCenter"))
                    x = app.getSize().x / 2;
                if (y == 0 && elem->Attribute("yWindowCenter"))
                    y = app.getSize().y / 2;
                widget->setCenterCoordinates(x, y);
            }

            int w = 0, h = 0;
            elem->QueryAttribute("width", &w);
            elem->QueryAttribute("height", &h);
            widget->setSize(w, h);

            if (elem->Attribute("allBackground"))
            {
                string b = elem->Attribute("allBackground");
                widget->setAllBackground(b);
            }

            if (elem->Attribute("allText"))
            {
                string t = elem->Attribute("allText");
                widget->setAllText(tools::textManager::getText("gui", t));
            }
            int textSize = gui::style::defaultTextSize();
            elem->QueryAttribute("textSize", &textSize);
            string font;
            if (elem->Attribute("textFont")) font = elem->Attribute("textFont");
            widget->setTextFont(gui::style::fontFromString(font), textSize);

            XMLHandle hdl2(elem);
            XMLElement *elem2 = hdl2.FirstChildElement().ToElement();
            while (elem2)
            {
                string elem2Name = elem2->Name();

                if (elem2Name == "state")
                {
                    string stateName = elem2->Attribute("name");
                    widget->addState(stateName);

                    if (elem2->Attribute("text"))
                    {
                        string t = elem2->Attribute("text");
                        widget->setText(stateName, tools::textManager::getText("gui", t));
                    }

                    if (elem2->Attribute("backgroundShader"))
                    {
                        string s = elem2->Attribute("backgroundShader");
                        widget->setBackgroundShader(stateName, s);
                    }

                    if (elem2->Attribute("foregroundShader"))
                    {
                        string s = elem2->Attribute("foregroundShader");
                        widget->setForegroundShader(stateName, s);
                    }
                }
                if (elem2Name == "addEntry" && type == "drop-down-list")
                {
                    DropDownList* d = dynamic_cast<DropDownList*>(widget);
                    string t = elem2->Attribute("text");
                    string data = elem2->Attribute("data");
                    d->addEntry(tools::textManager::getText("gui", t), data);
                }

                elem2 = elem2->NextSiblingElement();
            }

        }

        if (elemName == "signal")
        {
            string emitter = elem->Attribute("emitter");
            string signal = elem->Attribute("signal");

            string dataProvider = "";
            if (elem->Attribute("dataProvider"))
                dataProvider = elem->Attribute("dataProvider");

            signals.push_back(tuple<string, string, string>(emitter, signal, dataProvider));
        }

        if (elemName == "keyboardSignal")
        {
            int key = 0;
            elem->QueryAttribute("key", &key);
            string signal = elem->Attribute("signal");

            keyboardSignals.push_back(pair<int, string>(key, signal));
        }

        if (elemName == "signalListener")
        {
            string signal = elem->Attribute("signal");
            string widget = elem->Attribute("widget");
            string state = elem->Attribute("state");

            tools::signals::SignalListener l;
            l.signal = signal;
            signalListeners.push_back(tuple<tools::signals::SignalListener, string, string>(l, widget, state));
            tools::signals::registerListener(&(get<0>(signalListeners.back())));
        }

        if (elemName == "exitWindowSignal")
        {
            string emitter = elem->Attribute("emitter");
            exitWindowSignals.push_back(emitter);
        }

        elem = elem->NextSiblingElement();
    }
}

} //namespace gui
