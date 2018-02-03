
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

#include "tools/debug.h"
#include "tools/filesystem.h"
#include "tools/signals.h"
#include "tools/timeManager.h"
#include "textManager/textManager.h"

#include "imageManager/image.h"
#include "imageManager/imageManager.h"

#include "musicManager/musicManager.h"

#include "gui/window.h"
#include "gui/textWidget.h"
#include "gui/button.h"
#include "gui/inputField.h"
#include "gui/dropDownList.h"
#include "gui/scrollingList.h"
#include "gui/style.h"

using namespace tinyxml2;

namespace gui{

Window::Window()
  : widgets(),
    _triggers(),
    _events(),
    _watchedKeys(),
    _capturedSignals(),
    _x(0),
    _y(0),
    _width(0),
    _height(0),
    _flags(0),
    _screen(nullptr),
    backgroundImage(),
    backgroundFullscreenShader(),
    backgroundShader(),
    music()
{
}

Window::Window(const Window& other)
  : widgets(other.widgets),
    _triggers(other._triggers),
    _events(other._events),
    _watchedKeys(other._watchedKeys),
    _capturedSignals(other._capturedSignals),
    _x(other._x),
    _y(other._y),
    _width(other._width),
    _height(other._height),
    _flags(other._flags),
    _screen(other._screen),
    backgroundImage(other.backgroundImage),
    backgroundFullscreenShader(other.backgroundFullscreenShader),
    backgroundShader(other.backgroundShader),
    music(other.music)
{
}

Window::Window(string path, RenderWindow& window) : Window()
{
    _screen = &window;
    loadFromFile(path);
}

Window& Window::operator=(const Window& right)
{
    widgets = right.widgets;
    _triggers = right._triggers;
    _events = right._events;
    _watchedKeys = right._watchedKeys;
    _capturedSignals = right._capturedSignals;
    _x = right._x;
    _y = right._y;
    _width = right._width;
    _height = right._height;
    _flags = right._flags;
    _screen = right._screen;
    backgroundImage = right.backgroundImage;
    backgroundFullscreenShader = right.backgroundFullscreenShader;
    backgroundShader = right.backgroundShader;
    music = right.music;

    return *this;
}

Window::~Window()
{
    for (auto& w : widgets)
        delete w.second;
}

int Window::left()
{
    if (_screen == nullptr)
    {
        tools::debug::error("A window has been badly started", "gui", __FILENAME__, __LINE__);
        return 0;
    }

    int x = _x;

    if ((_flags & XPositionRelativeToCenter) == XPositionRelativeToCenter)
        x = _screen->getSize().x/2 + x;
    if ((_flags & XPositionRelativeToRight) == XPositionRelativeToRight)
        x = _screen->getSize().x - x;
    if ((_flags & XPositionRelativeToScreenSize) == XPositionRelativeToScreenSize)
        x *= _screen->getSize().x / 100.0;

    if ((_flags & OriginXCenter) == OriginXCenter)
        return x - width()/2;
    if ((_flags & OriginRight) == OriginRight)
        return x - width();

    return x;
}

int Window::top()
{
    if (_screen == nullptr)
    {
        tools::debug::error("A window has been badly started", "gui", __FILENAME__, __LINE__);
        return 0;
    }

    int y = _y;

    if ((_flags & YPositionRelativeToCenter) == YPositionRelativeToCenter)
        y = _screen->getSize().y/2 + y;
    if ((_flags & YPositionRelativeToBottom) == YPositionRelativeToBottom)
        y = _screen->getSize().y - y;
    if ((_flags & YPositionRelativeToScreenSize) == YPositionRelativeToScreenSize)
        y *= _screen->getSize().y / 100.0;

    if ((_flags & OriginYCenter) == OriginYCenter)
        return y - height()/2;
    if ((_flags & OriginBottom) == OriginBottom)
        return y - height();

    return y;
}

int Window::width()
{
    if (_screen == nullptr)
    {
        tools::debug::error("A window has been badly started", "gui", __FILENAME__, __LINE__);
        return 0;
    }

    if ((_flags & WidthRelativeToScreenSize) == WidthRelativeToScreenSize)
        return _screen->getSize().x * _width / 100.0;

    if ((_flags & WidthMeansFixedMargin) == WidthMeansFixedMargin)
        return _screen->getSize().x - _width;

    return _width;
}

int Window::height()
{
    if (_screen == nullptr)
    {
        tools::debug::error("A window has been badly started", "gui", __FILENAME__, __LINE__);
        return 0;
    }

    if ((_flags & HeightRelativeToScreenSize) == HeightRelativeToScreenSize)
        return _screen->getSize().y * _height / 100.0;

    if ((_flags & HeightMeansFixedMargin) == HeightMeansFixedMargin)
        return _screen->getSize().y - _height;

    return _height;
}

const RenderWindow* Window::screen()
{
    return _screen;
}

void Window::startWindow(RenderWindow& app)
{
    _screen = &app;

    exitWindow = false;
}

void Window::display(RenderWindow& app)
{
    if (!backgroundShader.empty())
        gui::style::displayShader(app, backgroundShader, left(), top(), width(), height());

    if (!backgroundImage.empty())
    {
        if ((_flags & AdjustBackgroundToSize) == AdjustBackgroundToSize)
        {
            imageManager::Image* image = imageManager::getImage("gui", backgroundImage);
            View currentView = app.getView();

            View newView(FloatRect(0, 0, image->getSize().x, image->getSize().y));
            newView.setViewport(FloatRect((float)left()/(float)app.getSize().x, (float)top()/(float)app.getSize().y, width()/(float)app.getSize().x, height()/(float)app.getSize().y));

            app.setView(newView);
            imageManager::display(app, "gui", backgroundImage, 0, 0);

            app.setView(currentView);
        }
        else if ((_flags & RepeatBackgroundToFitSize) == RepeatBackgroundToFitSize)
        {
            imageManager::Image* image = imageManager::getImage("gui", backgroundImage);
            View currentView = app.getView();

            View newView(FloatRect(0, 0, width(), height()));
            newView.setViewport(FloatRect((float)left()/(float)app.getSize().x, (float)top()/(float)app.getSize().y, width()/(float)app.getSize().x, height()/(float)app.getSize().y));

            app.setView(newView);
            for (unsigned i = 0 ; i <= width() / image->getSize().x + 1 ; ++i)
                    for (unsigned j = 0 ; j <= height() / image->getSize().y + 1 ; ++j)
                        imageManager::display(app, "gui", backgroundImage, i * image->getSize().x, j * image->getSize().y);

            app.setView(currentView);
        }
        else
        {
            imageManager::display(app, "gui", backgroundImage, left(), top());
        }
    }

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

    while (!exitWindow)
    {
        Event event;
        while (app.pollEvent(event))
        {
            gui::updateMousePosition(app);

            manage(app, event);
        }

        checkTriggers();

        app.setView(View(FloatRect(0, 0, app.getSize().x, app.getSize().y)));
        display(app);

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

        widget.second->activated(app, event);
    }

    for (auto i : _watchedKeys)
    {
        if (event.type == Event::KeyPressed && event.key.code == i)
        {
            optionType o;
            o.set<Keyboard::Key>(i);
            addEvent(nullptr, KeyPressed, o);
        }
        if (event.type == Event::KeyReleased && event.key.code == i)
        {
            optionType o;
            o.set<Keyboard::Key>(i);
            addEvent(nullptr, KeyReleased, o);
        }
        if (Keyboard::isKeyPressed(i))
        {
            optionType o;
            o.set<Keyboard::Key>(i);
            addEvent(nullptr, KeyHeld, o);
        }
    }
}

void Window::checkTriggers()
{
    while (!_capturedSignals.empty())
    {
        optionType o;
        o.set<tools::signals::Signal>(_capturedSignals.front());
        addEvent(nullptr, SignalCaptured, o);
        _capturedSignals.pop();
    }

    for (auto& e : _events) for (auto& t : _triggers) if (e.widget == t.sender && e.event == t.event)
    {
        if (t.event == KeyPressed || t.event == KeyReleased || t.event == KeyHeld)
        {
            if (t.eventData.get<Keyboard::Key>() != e.data.get<Keyboard::Key>())
                continue;
        }
        if (t.event == SignalCaptured)
        {
            if (t.eventData.get<string>() != e.data.get<tools::signals::Signal>().first)
                continue;
        }

        switch (t.action)
        {
            case SendSignal:
            {
                string value;
                if (t.dataProvider != nullptr)
                    value = t.dataProvider->embeddedData<string>(t.dataName);
                tools::signals::addSignal(t.signal, value);
                break;
            }
            case ModifyEmbeddedData:
            {
                optionType o;
                if (t.dataProvider != nullptr)
                    o = t.dataProvider->embeddedData<optionType>(t.dataName);
                if (t.target != nullptr)
                    t.target->addEmbeddedData<optionType>(t.signal, o);
                break;
            }
            case Enable:
                if (t.target != nullptr)
                    t.target->removeFlags(Disabled);
                break;
            case Disable:
                if (t.target != nullptr)
                    t.target->addFlags(Disabled);
                break;
            case Show:
                if (t.target != nullptr)
                    t.target->removeFlags(Hidden);
                break;
            case Hide:
                if (t.target != nullptr)
                    t.target->addFlags(Hidden);
                break;
            case ExitWindow:
                exitWindow = true;
                break;
            default:
                break;
        }
    }

    _events.clear();
}

Widget* Window::widget(string name)
{
    auto i = widgets.find(name);
    if (i != widgets.end())
    {
        return i->second;
    }
    else
    {
        tools::debug::error("Unknown widget: " + name, "gui", __FILENAME__, __LINE__);
        auto result = widgets.emplace(name, new TextWidget());
        result.first->second->setParent(this);
        result.first->second->addFlags(Hidden);
        return result.first->second;
    }
}

const map<string,Widget*>& Window::getWidgets()
{
    return widgets;
}

void Window::setValue(const string& widget, optionType v)
{
    auto i = widgets.find(widget);
    if (i != widgets.end())
    {
        i->second->setValue(v);
    }
}

void Window::addEvent(Widget* s, EventTypes e, optionType d)
{
    _events.emplace_back(s, e, d);
}

void Window::loadFromFile(string path)
{
    path = tools::filesystem::dataDirectory() + path;

    XMLDocument file;
    file.LoadFile(path.c_str());

    XMLHandle hdl(file);
    XMLElement *elem = hdl.FirstChildElement().FirstChildElement().ToElement();

    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "loadImage")
        {
            string key = elem->Attribute("key");
            string pathToImage = elem->Attribute("path");
            imageManager::addContainer("gui");
            imageManager::addImage("gui", key, pathToImage);
        }

        if (elemName == "properties")
        {
            elem->QueryAttribute("x", &_x);
            elem->QueryAttribute("y", &_y);

            if (elem->Attribute("OriginXCenter"))
                _flags |= OriginXCenter;
            if (elem->Attribute("OriginRight"))
                _flags |= OriginRight;
            if (elem->Attribute("OriginYCenter"))
                _flags |= OriginYCenter;
            if (elem->Attribute("OriginBottom"))
                _flags |= OriginBottom;
            if (elem->Attribute("OriginCenter"))
                _flags |= (OriginXCenter | OriginYCenter);

            if (elem->Attribute("XPositionRelativeToCenter"))
                _flags |= XPositionRelativeToCenter;
            if (elem->Attribute("XPositionRelativeToRight"))
                _flags |= XPositionRelativeToRight;
            if (elem->Attribute("YPositionRelativeToCenter"))
                _flags |= YPositionRelativeToCenter;
            if (elem->Attribute("YPositionRelativeToBottom"))
                _flags |= YPositionRelativeToBottom;
            if (elem->Attribute("PositionRelativeToCenter"))
                _flags |= (XPositionRelativeToCenter | YPositionRelativeToCenter);
            if (elem->Attribute("XPositionRelativeToScreenSize"))
                _flags |= XPositionRelativeToScreenSize;
            if (elem->Attribute("YPositionRelativeToScreenSize"))
                _flags |= YPositionRelativeToScreenSize;
            if (elem->Attribute("PositionRelativeToScreenSize"))
                _flags |= (XPositionRelativeToScreenSize | YPositionRelativeToScreenSize);

            elem->QueryAttribute("width", &_width);
            elem->QueryAttribute("height", &_height);

            if (elem->Attribute("WidthRelativeToScreenSize"))
                _flags |= WidthRelativeToScreenSize;
            if (elem->Attribute("WidthMeansFixedMargin"))
                _flags |= WidthMeansFixedMargin;
            if (elem->Attribute("HeightRelativeToScreenSize"))
                _flags |= HeightRelativeToScreenSize;
            if (elem->Attribute("HeightMeansFixedMargin"))
                _flags |= HeightMeansFixedMargin;

            if (elem->Attribute("Fullscreen"))
            {
                _width = 100;
                _height = 100;
                _flags |= (WidthRelativeToScreenSize | HeightRelativeToScreenSize);
            }

            if (elem->Attribute("backgroundImage"))
                backgroundImage = elem->Attribute("backgroundImage");

            if (elem->Attribute("backgroundFullscreenShader"))
                backgroundFullscreenShader = elem->Attribute("backgroundFullscreenShader");

            if (elem->Attribute("backgroundShader"))
                backgroundShader = elem->Attribute("backgroundShader");

            if (elem->Attribute("AdjustBackgroundToSize"))
                _flags |= AdjustBackgroundToSize;
            if (elem->Attribute("RepeatBackgroundToFitSize"))
                _flags |= RepeatBackgroundToFitSize;

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
            if (type == "scrolling-list") widget = new ScrollingList();

            widgets.insert(map<string, Widget*>::value_type(widgetName, widget));

            widget->setParent(this);

            int x = 0, y = 0;
            elem->QueryAttribute("x", &x);
            elem->QueryAttribute("y", &y);
            widget->setPosition(x, y);

            if (elem->Attribute("OriginXCenter"))
                widget->addFlags(OriginXCenter);
            if (elem->Attribute("OriginRight"))
                widget->addFlags(OriginRight);
            if (elem->Attribute("OriginYCenter"))
                widget->addFlags(OriginYCenter);
            if (elem->Attribute("OriginBottom"))
                widget->addFlags(OriginBottom);
            if (elem->Attribute("OriginCenter"))
                widget->addFlags(OriginXCenter | OriginYCenter);

            if (elem->Attribute("XPositionRelativeToCenter"))
                widget->addFlags(XPositionRelativeToCenter);
            if (elem->Attribute("XPositionRelativeToRight"))
                widget->addFlags(XPositionRelativeToRight);
            if (elem->Attribute("YPositionRelativeToCenter"))
                widget->addFlags(YPositionRelativeToCenter);
            if (elem->Attribute("YPositionRelativeToBottom"))
                widget->addFlags(YPositionRelativeToBottom);
            if (elem->Attribute("PositionRelativeToCenter"))
                widget->addFlags(XPositionRelativeToCenter | YPositionRelativeToCenter);
            if (elem->Attribute("XPositionRelativeToScreenSize"))
                widget->addFlags(XPositionRelativeToScreenSize);
            if (elem->Attribute("YPositionRelativeToScreenSize"))
                widget->addFlags(YPositionRelativeToScreenSize);
            if (elem->Attribute("PositionRelativeToScreenSize"))
                widget->addFlags(XPositionRelativeToScreenSize | YPositionRelativeToScreenSize);
            if (elem->Attribute("AdjustSizeToText"))
                widget->addFlags(AdjustSizeToText);

            int w = 0, h = 0;
            elem->QueryAttribute("width", &w);
            elem->QueryAttribute("height", &h);
            widget->setSize(w, h);

            if (elem->Attribute("WidthRelativeToScreenSize"))
                widget->addFlags(WidthRelativeToScreenSize);
            if (elem->Attribute("WidthMeansFixedMargin"))
                widget->addFlags(WidthMeansFixedMargin);
            if (elem->Attribute("HeightRelativeToScreenSize"))
                widget->addFlags(HeightRelativeToScreenSize);
            if (elem->Attribute("HeightMeansFixedMargin"))
                widget->addFlags(HeightMeansFixedMargin);

            if (elem->Attribute("Fullscreen"))
            {
                widget->setSize(100, 100);
                widget->addFlags(WidthRelativeToScreenSize | HeightRelativeToScreenSize);
            }

            if (elem->Attribute("allBackground"))
                widget->setBackground(elem->Attribute("allBackground"));

            if (elem->Attribute("AdjustBackgroundToSize"))
                widget->addFlags(AdjustBackgroundToSize);
            if (elem->Attribute("RepeatBackgroundToFitSize"))
                widget->addFlags(RepeatBackgroundToFitSize);

            if (elem->Attribute("VerticalScrollBar"))
                widget->addFlags(VerticalScrollBar);

            if (elem->Attribute("textShaded"))
                widget->addTextFlags(textManager::Shaded);

            if (elem->Attribute("allText"))
                widget->setText(textManager::getText("gui", elem->Attribute("allText")));

            if (elem->Attribute("backgroundShader"))
                widget->setBackgroundShader(elem->Attribute("backgroundShader"));

            XMLHandle hdl2(elem);
            XMLElement *elem2 = hdl2.FirstChildElement().ToElement();
            while (elem2)
            {
                string elem2Name = elem2->Name();

                if (elem2Name == "addEmbeddedData" && elem2->Attribute("name") && elem2->Attribute("value"))
                {
                    string name = elem2->Attribute("name");

                    if (elem2->Attribute("type"))
                    {
                        string_view dataType = elem2->Attribute("type");
                        if (dataType == "plaintext")
                            widget->addEmbeddedData<textManager::PlainText>(name, textManager::PlainText(elem2->Attribute("value")));
                        else if (dataType == "string")
                            widget->addEmbeddedData<string>(name, elem2->Attribute("value"));
                    }
                    else
                    {
                        widget->addEmbeddedData<string>(name, elem2->Attribute("value"));
                    }
                }
                if (elem2Name == "addEntry" && type == "drop-down-list")
                {
                    DropDownList* d = dynamic_cast<DropDownList*>(widget);
                    string t = elem2->Attribute("text");
                    string data = elem2->Attribute("data");
                    d->addEntry(textManager::getText("gui", t), data);
                }

                elem2 = elem2->NextSiblingElement();
            }

        }

        if (elemName == "trigger")
        {
            Trigger t;

            if (elem->Attribute("sender"))
                t.sender = widget(elem->Attribute("sender"));

            if (elem->Attribute("event", "WidgetActivated")) t.event = WidgetActivated;
            else if (elem->Attribute("event", "WidgetValueChanged")) t.event = WidgetValueChanged;
            else if (elem->Attribute("event", "KeyPressed")) t.event = KeyPressed;
            else if (elem->Attribute("event", "KeyReleased")) t.event = KeyReleased;
            else if (elem->Attribute("event", "KeyHeld")) t.event = KeyHeld;
            else if (elem->Attribute("event", "SignalCaptured")) t.event = SignalCaptured;

            if (elem->Attribute("eventDataType") && elem->Attribute("eventData"))
            {
                string_view dataType = elem->Attribute("eventDataType");

                if (dataType == "string")
                {
                    t.eventData.set<string>(string(elem->Attribute("eventData")));
                }
                else if (dataType == "key")
                {
                    int k = 0;
                    elem->QueryAttribute("eventData", &k);
                    t.eventData.set<Keyboard::Key>(static_cast<Keyboard::Key>(k));
                    _watchedKeys.push_back(static_cast<Keyboard::Key>(k));
                }
            }

            if (elem->Attribute("action", "SendSignal")) t.action = SendSignal;
            else if (elem->Attribute("action", "ModifyEmbeddedData")) t.action = ModifyEmbeddedData;
            else if (elem->Attribute("action", "Enable")) t.action = Enable;
            else if (elem->Attribute("action", "Disable")) t.action = Disable;
            else if (elem->Attribute("action", "Show")) t.action = Show;
            else if (elem->Attribute("action", "Hide")) t.action = Hide;
            else if (elem->Attribute("action", "ExitWindow")) t.action = ExitWindow;

            if (elem->Attribute("signal"))
                t.signal = elem->Attribute("signal");

            if (elem->Attribute("dataProvider"))
                t.dataProvider = widget(elem->Attribute("dataProvider"));

            if (elem->Attribute("dataName"))
                t.dataName = elem->Attribute("dataName");

            if (elem->Attribute("target"))
                t.target = widget(elem->Attribute("target"));

            if (t.event == SignalCaptured)
                tools::signals::registerListener(t.eventData.get<string>(), &_capturedSignals);

            _triggers.push_back(std::move(t));
        }

        elem = elem->NextSiblingElement();
    }
}

} //namespace gui
