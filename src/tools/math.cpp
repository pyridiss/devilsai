
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

#include <cmath>
#include <random>
#include <chrono>

#include "tools/math.h"

namespace tools{

namespace math{

default_random_engine randomGenerator;

void initLibrary()
{
    chrono::high_resolution_clock::time_point t = chrono::high_resolution_clock::now();
    chrono::high_resolution_clock::duration   d = t.time_since_epoch();
    randomGenerator.seed(d.count());
}

double angle(double x, double y)
{
    if (x > 0 && y >= 0)
        return atan(y / x);
    if (x > 0 && y < 0)
        return atan(y / x) + 2 * M_PI;
    if (x < 0)
        return atan(y / x) + M_PI;
    if (x == 0 && y > 0)
        return M_PI / 2;
    if (x == 0 && y < 0)
        return 3 * M_PI / 2;
}

// binomial_distribution

double randomNumber(double min, double max)
{
    double n = randomGenerator() / 1000.0;
    n -= floor(n);

    return min + (max - min) * n;
}

double randomNumber_BinomialLaw(double min, double max)
{
    binomial_distribution<int> distribution(1000*(max - min), 0.5);

    return min + distribution(randomGenerator) / 1000.0;
}

} //namespace math

} //namespace tools
