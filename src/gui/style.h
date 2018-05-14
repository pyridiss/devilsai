
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

template<typename T>
void parameterize(unsigned int hash, T value);
template<typename T>
T parameter(unsigned int hash);

bool initLibrary();
enum Flags
{
    OriginXCenter = 1 << 0,
    OriginRight = 1 << 1,
    OriginYCenter = 1 << 2,
    OriginBottom = 1 << 3,
    XPositionRelativeToCenter = 1 << 4,
    XPositionRelativeToRight = 1 << 5,
    YPositionRelativeToCenter = 1 << 6,
    YPositionRelativeToBottom = 1 << 7,
    XPositionRelativeToScreenSize = 1 << 8,
    YPositionRelativeToScreenSize = 1 << 9,
    WidthRelativeToScreenSize = 1 << 10,
    WidthMeansFixedMargin = 1 << 11,
    HeightRelativeToScreenSize = 1 << 12,
    HeightMeansFixedMargin = 1 << 13,
    AdjustSizeToText = 1 << 14,
    AdjustBackgroundToSize = 1 << 15,
    RepeatBackgroundToFitSize = 1 << 16,
    VerticalScrollBar = 1 << 17,
    VerticalScrollBarInUse = 1 << 18,
    Hidden = 1 << 19,
    Disabled = 1 << 20,
    Activated = 1 << 21,
    MouseOver = 1 << 22,
    CustomTextShader = 1 << 23
};

namespace style{

void initStyle(RenderWindow* w);

const Shader* getContrastShader(float r, float g, float b);
const Shader* getColorizeShader(Vector3f r, Vector3f g, Vector3f b);
const Shader* getFadeShader(float& value);
void textBackgroundShader(RenderWindow& app, int x, int y, int w, int h);
void highlightShader(RenderWindow& app, int x, int y, int w, int h);
void disableShader(RenderWindow& app, int x, int y, int w, int h);
void warnShader(RenderWindow& app, int x, int y, int w, int h);
void displayShader(RenderWindow& app, string shader, int x, int y, int w, int h);
RenderTexture& temporaryTexture();

} //namespace style

void updateMousePosition(RenderWindow & app);
const Vector2i& mousePosition();

} //namespace gui

#endif // GUI_STYLE_H

