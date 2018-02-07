
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2018  Quentin Henriet

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

#include "textManager/richText.h"

namespace textManager{

RichText::RichText()
  : _words(),
    _width(0),
    _height(0),
    _defaultFont(nullptr),
    _defaultSize(0),
    _defaultColor(),
    _flags(0)
{
}

RichText::RichText(const RichText& other)
  : _words(other._words),
    _width(other._width),
    _height(other._height),
    _defaultFont(other._defaultFont),
    _defaultSize(other._defaultSize),
    _defaultColor(other._defaultColor),
    _flags(other._flags)
{
}

RichText& RichText::operator=(const RichText& right)
{
    _words = right._words;
    _width = right._width;
    _height = right._height;
    _defaultFont = right._defaultFont;
    _defaultSize = right._defaultSize;
    _defaultColor = right._defaultColor;
    _flags = right._flags;
    return *this;
}

void RichText::alignCurrentLine(vector<sf::Text*>& line, bool endOfParagraph)
{
    //Auto set width if it has not be done yet (so there is only one line)
    if (_width == 0)
        _width = line.back()->getPosition().x + line.back()->getLocalBounds().width;

    //Horizontal alignment
    //By default, the line is left-aligned. Only check other alignments.
    if ((_flags & HAlignRight) == HAlignRight)
    {
        int padding = _width - line.back()->getPosition().y - line.back()->getLocalBounds().width;
        for (auto i : line)
            i->move(padding, 0);
    }
    else if ((_flags & HAlignCenter) == HAlignCenter)
    {
        int size = 0;
        for (auto i : line)
            size += i->getLocalBounds().width;
        size += 5 * (line.size() - 1); //spaces

        int offset = (_width - size)/2;
        for (auto i : line)
            i->move(offset, 0);
    }
    else if ((_flags & HAlignJustify) == HAlignJustify)
    {
        if (!endOfParagraph) //The last line is left-aligned
        {
            int size = 0;
            for (auto i : line)
                size += i->getLocalBounds().width;

            float blank = (float)(_width - size) / (float)(line.size() - 1);
            for (unsigned i = 1 ; i < line.size() ; ++i)
                line[i]->move((int)((blank - 5) * i), 0);
        }
    }

    //Vertical alignment
    for (auto i : line)
        i->move(0, (int)(lineHeight(line) * 4./5.));
}

unsigned RichText::lineHeight(vector<sf::Text*>& line)
{
    unsigned h = 0;
    for (auto i : line)
        h = max(h, (unsigned)i->getCharacterSize());
    return h;
}

void RichText::reduceHeight()
{
    //Due to vertical alignment, the first line may have a margin above it. This function removes this.
    float minY = 100;
    for (auto& i : _words)
        minY = min(minY, i.getPosition().y);

    for (auto& i : _words)
        i.move(0, -minY);

    //If FixedHeight and VAlignCenter are set, add a new margin
    if ((_flags & FixedHeight) == FixedHeight && (_flags & VAlignCenter) == VAlignCenter)
    {
        float realHeight = 0;
        for (auto& i : _words)
            realHeight = max(realHeight, i.getGlobalBounds().top + i.getCharacterSize());

        for (auto& i : _words)
            i.move(0, (height() - (int)realHeight)/2);
    }
}

void RichText::create(const PlainText& src)
{
    _words.clear();

    if (_defaultFont == nullptr)
        return;

    const basic_string<unsigned int>& str = src.aggregatedText();
    if (str.empty()) return;

    unsigned spaceCharacter = 32; // ' ' (space)

    size_t oldPos = 0;
    size_t pos = str.find(spaceCharacter);

    while(1)
    {
        Text t;
        t.setString(str.substr(oldPos, pos - oldPos));
        _words.push_back(std::move(t));

        if (pos < str.size())
        {
            oldPos = pos + 1;
            pos = str.find(spaceCharacter, pos+1);
        }
        else
            break;
    }

    float currentX = 0, currentY = 0;
    vector<sf::Text*> currentLine;

    Font* customFont = _defaultFont;
    int customSize = _defaultSize;
    Color customColor = _defaultColor;

    //Check if this is no empty word (unexpected spaces)
    for (auto i = _words.begin() ; i != _words.end() ; ++i)
    {
        if (i->getString().getSize() == 0)
            i = _words.erase(i);
    }

    for (auto& i : _words)
    {
        while (i.getString()[0] == 64) // '@', change format
        {
            int parameterBegin = 0, parameterEnd = 1;
            if (i.getString().getSize() > 2 && i.getString()[2] == 91) // '[', parameter begin
            {
                parameterBegin = 2;
                parameterEnd = i.getString().find("]");
            }

            switch (i.getString()[1])
            {
                case 110 : // 'n', new line
                {
                    int height = 5;
                    if (parameterBegin == 2)
                        height = textManager::toInt(i.getString().substring(parameterBegin + 1, parameterEnd - parameterBegin - 1));

                    currentX = 0;
                    currentY += lineHeight(currentLine) + height;
                    alignCurrentLine(currentLine, true);
                    currentLine.clear();
                    break;
                }
                case 102 : // 'f', change font
                {
                    string newFont = i.getString().substring(parameterBegin + 1, parameterEnd - parameterBegin - 1);
                    customFont = gui::style::fontFromString(newFont);
                    break;
                }
                case 115 : // 's', change size
                {
                    string newSize = i.getString().substring(parameterBegin + 1, parameterEnd - parameterBegin - 1);
                    customSize = textManager::toInt(newSize);
                    break;
                }
                case 99 : // 'c', change color
                {
                    string newColor = i.getString().substring(parameterBegin + 1, parameterEnd - parameterBegin - 1);
                    size_t comma1 = newColor.find(",");
                    size_t comma2 = newColor.find(",", comma1 + 1);
                    int r = textManager::toInt(newColor.substr(0, comma1));
                    int g = textManager::toInt(newColor.substr(comma1 + 1, comma2 - comma1 - 1));
                    int b = textManager::toInt(newColor.substr(comma2 + 1));
                    customColor = Color(r, g, b);
                    break;
                }
                case  116 : // 't', tabulation
                {
                    string tab = i.getString().substring(parameterBegin + 1, parameterEnd - parameterBegin - 1);
                    int t = textManager::toInt(tab);
                    if (currentX < t) currentX = t;
                    break;
                }
                case 100 : // 'd', back to default
                    customFont = _defaultFont;
                    customSize = _defaultSize;
                    customColor = _defaultColor;
                    break;

                default:
                    break;
            }
            i.setString(i.getString().substring(parameterEnd + 1));
        }

        i.setFont(*customFont);
        i.setCharacterSize(customSize);
        i.setFillColor(customColor);

        if (_width != 0 && currentX + i.getLocalBounds().width + 5 > _width)
        {
            currentX = 0;
            currentY += lineHeight(currentLine) + 3;
            alignCurrentLine(currentLine, false);
            currentLine.clear();
        }

        i.setPosition(currentX, currentY + _defaultSize - customSize);

        currentLine.push_back(&i);

        currentX += i.getLocalBounds().width + 5;
    }
    alignCurrentLine(currentLine, true); //Last line
    reduceHeight();

    if ((_flags & FixedHeight) == 0)
        _height = 0;
}

void RichText::setSize(int w, int h)
{
    _width = w;
    _height = h;
}

void RichText::setDefaultProperties(const string& f, int s, Color c)
{
    _defaultFont = gui::style::fontFromString(f);
    _defaultSize = s;
    _defaultColor = c;
}

void RichText::addFlags(uint16_t newFlags)
{
    _flags |= newFlags;
}

int RichText::width()
{
    return _width;
}

int RichText::height()
{
    if (_height == 0)
    {
        for (auto& i : _words)
            _height = max(_height, (int)i.getGlobalBounds().top + (int)i.getCharacterSize());
    }

    return _height;
}

void RichText::displayToView(RenderTarget& target, View view)
{
    View currentView = target.getView();
    target.setView(view);

    int minY = view.getCenter().y - view.getSize().y - 40;
    int maxY = view.getCenter().y + view.getSize().y + 40;
    for (auto& i : _words)
    {
        if ((_flags & Shaded) == Shaded)
        {
            Color originalColor = i.getFillColor();
            i.setFillColor(Color(0, 0, 0, 192));

            i.move(1, 1);
            target.draw(i);

            i.move(-1, -1);
            i.setFillColor(originalColor);
        }

        if (i.getPosition().y > minY && i.getPosition().y < maxY)
            target.draw(i);
    }

    target.setView(currentView);
}

void RichText::displayFullText(RenderTarget& target, int x, int y, const Shader* shader)
{
    x = target.mapCoordsToPixel(Vector2f(x, y)).x - 2;
    y = target.mapCoordsToPixel(Vector2f(x, y)).y - 2;
    float w = width() + 4;
    float h = height() + 4;

    if ((_flags & OriginRight) == OriginRight)
        x -= w;
    else if ((_flags & OriginXCenter) == OriginXCenter)
        x -= w/2;

    if ((_flags & OriginBottom) == OriginBottom)
        y -= h;
    else if ((_flags & OriginYCenter) == OriginYCenter)
        y -= h/2;

    View newView(FloatRect(-2, -2, w, h));

    if (shader == nullptr)
    {
        newView.setViewport(FloatRect((float)x/(float)target.getSize().x, (float)y/(float)target.getSize().y, w/(float)target.getSize().x, h/(float)target.getSize().y));
        displayToView(target, newView);
    }
    else
    {
        RenderTexture& tex = gui::style::temporaryTexture();
        tex.clear(Color::Transparent);

        newView.setViewport(FloatRect((float)x/(float)tex.getSize().x, (float)y/(float)tex.getSize().y, w/(float)tex.getSize().x, h/(float)tex.getSize().y));
        displayToView(tex, newView);

        tex.display();

        Sprite sprite(tex.getTexture());
        target.draw(sprite, shader);
    }
}

} //namespace textManager
