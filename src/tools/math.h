
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

#ifndef TOOLS_MATH_H
#define TOOLS_MATH_H

//M_PI seems to cause problems on some platforms; define it if not done yet.
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace tools{

namespace math{

struct Shape;

void initLibrary();
int newUniqueIdentifier();
double angle(double x, double y);
double randomNumber(double min, double max);
int randomNumber(int min, int max);
double randomNumber_BinomialLaw(double min, double max);
bool intersection(Shape& shape1, Shape& shape2);

} //namespace math

constexpr unsigned int hash(const char* str)
{
    unsigned int h = 0;
    for (unsigned i = 0 ; str[i] != '\0' ; ++i)
        h = str[i] + (h << 6) + (h << 16) - h;

    return h;
}

} //namespace tools

#endif // TOOLS_MATH_H

