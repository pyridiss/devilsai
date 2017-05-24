
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

#include "tools/signals.h"
#include "tools/timeManager.h"
#include "tools/textManager.h"

#include "imageManager/imageManager.h"

#include "gui/window.h"
#include "gui/textWidget.h"
#include "gui/button.h"
#include "gui/style.h"

#include "config.h"

using namespace tinyxml2;

namespace gui{

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
    imageManager::display(app, "misc", background, getXTopLeft(), getYTopLeft());

    for (auto& widget : widgets)
    {
        widget.second->display(app);
    }
}

void Window::manage(RenderWindow& app)
{
    gui::style::disableShader(app, 0, 0, app.getSize().x, app.getSize().y);

    while (!exitWindow)
    {
        Event event;
        while (app.pollEvent(event))
        {
            manage(app, event);
        }

        display(app);
        tools::timeManager::frameDone();
        app.display();
    }
}

void Window::manage(RenderWindow& app, Event &event)
{
    for (auto& widget : widgets)
    {
        if (widget.second->activated(app, event.type))
        {
            for (auto& signal : signals)
            {
                if (signal.first == widget.first)
                {
                    tools::signals::addSignal(signal.second);
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
}

void Window::loadFromFile(string path, RenderWindow& app)
{
    path = INSTALL_DIR + path;

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

            background = elem->Attribute("background");
        }

        if (elemName == "widget")
        {
            string name = elem->Attribute("name");
            string type = elem->Attribute("type");
            Widget* widget = nullptr;
            if (type == "text")
            {
                widget = new TextWidget();
                widgets.insert(map<string, Widget*>::value_type(name, widget));
            }
            if (type == "button")
            {
                widget = new Button();
                widgets.insert(map<string, Widget*>::value_type(name, widget));
            }

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
                widget->setAllText(tools::textManager::getText("devilsai", t));
            }
            int textSize = gui::style::defaultTextSize();
            elem->QueryAttribute("textSize", &textSize);
            string font;
            if (elem->Attribute("textFont")) font = elem->Attribute("textFont");
            widget->setTextFont(gui::style::fontFromString(font), textSize);
        }

        if (elemName == "signal")
        {
            string emitter = elem->Attribute("emitter");
            string signal = elem->Attribute("signal");
            signals.push_back(pair<string, string>(emitter, signal));
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
