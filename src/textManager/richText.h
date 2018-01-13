
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

#ifndef TEXTMANAGER_RICHTEXT_H
#define TEXTMANAGER_RICHTEXT_H

#include <SFML/Graphics.hpp>

#include "tools/debug.h"
#include "tools/textManager.h"

#include "gui/style.h"

#include "textManager/plainText.h"


using namespace std;
using namespace sf;

namespace textManager{

class RichText
{
    private:
        PlainText* _source;
        vector<sf::Text> _words;
        int _width, _height;
        Font* _defaultFont;
        int _defaultSize;
        Color _defaultColor;
        uint16_t _flags;

    public:
        RichText();
        RichText(const RichText& other);
        RichText& operator=(const RichText& right);

    private:
        void alignCurrentLine(vector<sf::Text*>& line, bool endOfParagraph);
        unsigned lineHeight(vector<sf::Text*>& line);
        void reduceHeight();
        void cutParagraph();

    public:
        void setSource(PlainText* src);
        void setSize(int w, int h);
        void setDefaultProperties(const string& f, int s, Color c);
        void addFlags(uint16_t newFlags);
        int width();
        int height();
        void displayToView(RenderTarget& target, View view);
        void displayFullText(RenderTarget& target, int x, int y);
};

} //namespace textManager

#endif // TEXTMANAGER_RICHTEXT_H
