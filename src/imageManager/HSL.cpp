
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

/*
 * This code is derived largely from https://github.com/SFML/SFML/wiki/Source:-HSL-Color
 */

#include "imageManager/HSL.h"

using namespace std;

namespace imageManager {

const double D_EPSILON = 0.00000000000001;

HSL::HSL() : hue(0), saturation(0), luminance(0)
{

}

HSL::HSL(int h, int s, int l)
{
    //Range control for hue.
    if (h <= 360 && h >= 0) hue = h;
    else
    {
        if(h > 360) hue = h%360;
        else if(h < 0 && h > -360) hue = -h;
        else if(h < 0 && h < -360) hue = -(h%360);
    }

    //Range control for saturation.
    if (s <= 100 && s >= 0) saturation = s;
    else
    {
        if(s > 100) saturation = s%100;
        else if(s < 0 && s > -100) saturation = -s;
        else if(s < 0 && s < -100) saturation = -(s%100);
    }

    //Range control for luminance
    if (l <= 100 && l >= 0) luminance = l;
    else
    {
        if(l > 100) luminance = l%100;
        if(l < 0 && l > -100) luminance = -l;
        if(l < 0 && l < -100) luminance = -(l%100);
    }
}

double HSL::hueToRGB(double arg1, double arg2, double h)
{
   if ( h < 0 ) h += 1;
   if ( h > 1 ) h -= 1;
   if ( ( 6 * h ) < 1 ) return arg1 + (arg2 - arg1) * 6 * h;
   if ( ( 2 * h ) < 1 ) return arg2;
   if ( ( 3 * h ) < 2 ) return arg1 + (arg2 - arg1) * ((2.0/3.0) - h) * 6;
   return arg1;
}

sf::Color HSL::turnToRGB()
{
    //Reconvert to range [0,1]
    double H = hue/360.0;
    double S = saturation/100.0;
    double L = luminance/100.0;

    float arg1, arg2;

    if (S <= D_EPSILON)
    {
        sf::Color C(L*255, L*255, L*255);
        return C;
    }
    else
    {
        if ( L < 0.5 ) { arg2 = L * ( 1 + S ); }
        else { arg2 = ( L + S ) - ( S * L ); }
        arg1 = 2 * L - arg2;

        sf::Uint8 r =( 255 * hueToRGB( arg1, arg2, (H + 1.0/3.0 ) ) );
        sf::Uint8 g =( 255 * hueToRGB( arg1, arg2, H ) );
        sf::Uint8 b =( 255 * hueToRGB( arg1, arg2, (H - 1.0/3.0 ) ) );
        sf::Color C(r,g,b);
        return C;
    }

}

HSL turnToHSL(const sf::Color& C)
{
    //Trivial cases
    if(C == sf::Color::White)
        return HSL(0, 0, 100);

    if(C == sf::Color::Black)
        return HSL(0, 0, 0);

    if(C == sf::Color::Red)
        return HSL(0, 100, 50);

    if(C == sf::Color::Yellow)
        return HSL(60, 100, 50);

    if(C == sf::Color::Green)
        return HSL(120, 100, 50);

    if(C == sf::Color::Cyan)
        return HSL(180, 100, 50);

    if(C == sf::Color::Blue)
        return HSL(240, 100, 50);

    if(C == sf::Color::Cyan)
        return HSL(300, 100, 50);

    double R, G, B;
    R = C.r/255.0;
    G = C.g/255.0;
    B = C.b/255.0;

    double max, min, l, s;
    max = std::max(std::max(R,G),B);
    min = std::min(std::min(R,G),B);


    HSL A;
    l = ((max + min)/2.0);

    if (max - min <= D_EPSILON )
    {
        A.hue = 0;
        A.saturation = 0;
    }
    else
    {
        double diff = max - min;

        if(A.luminance < 0.5)
            s = diff/(max + min);
        else
            s = diff/(2 - max - min);

        double diffR = ( (( max - R ) * 60) + (diff/2.0) ) / diff;
        double diffG = ( (( max - G ) * 60) + (diff/2.0) ) / diff;
        double diffB = ( (( max - B ) * 60) + (diff/2.0) ) / diff;


        if (max - R <= D_EPSILON) { A.hue = diffB - diffG; }
        else if ( max - G <= D_EPSILON ) { A.hue = (1*360)/3.0 + (diffR - diffB); }
        else if ( max - B <= D_EPSILON ) { A.hue = (2*360)/3.0 + (diffG - diffR); }

        if (A.hue <= 0 || A.hue >= 360) { fmod(A.hue, 360); }

        s *= 100;
    }

    l *= 100;
    A.saturation = s;
    A.luminance = l;
    return A;
}

} //namespace imageManager
