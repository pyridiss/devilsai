
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

#ifndef GUI_STYLE_H
#define GUI_STYLE_H

#include <string>

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

namespace gui{

enum Flags
{
    TopLeftCoordinates = 1 << 0,
    CenterCoordinates = 1 << 1
};

namespace style{

void initStyle();

const Font& defaultTextFont();
const Font& fontFromString(string f);
Color colorFromString(string s);
int defaultTextSize();
const Font& buttonTextFont();
int buttonTextSize();
Color normalButtonTextColor();
Color activeButtonTextColor();
Color hoverButtonTextColor();
Color disabledButtonTextColor();
const Shader* getContrastShader(float r, float g, float b);
void textBackgroundShader(RenderWindow& app, int x, int y, int w, int h);
void highlightShader(RenderWindow& app, int x, int y, int w, int h);
void disableShader(RenderWindow& app, int x, int y, int w, int h);
void warnShader(RenderWindow& app, int x, int y, int w, int h);
void displayShader(RenderWindow& app, string shader, int x, int y, int w, int h);

} //namespace style

} //namespace gui

#endif // GUI_STYLE_H

