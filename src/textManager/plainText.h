
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

#ifndef TEXTMANAGER_PLAINTEXT_H
#define TEXTMANAGER_PLAINTEXT_H

#include <vector>
#include <string>

#include <SFML/System/Utf.hpp>

#include "tools/debug.h"

using namespace std;
using namespace sf;

namespace textManager{

class PlainText
{
    private:
        basic_string<unsigned int> _originalText;
        vector<basic_string<unsigned int>> _parameters;
        unsigned _parametersNeeded;
        basic_string<unsigned int> _aggregatedText;

    public:
        PlainText();
        PlainText(const string& str);
        PlainText(const basic_string<unsigned int>& str);
        PlainText& operator+=(const PlainText& right);
        PlainText& operator+=(const char* right);
        ~PlainText();

    private:
        void aggregate();
        void findParametersNeeded();

    public:
        void set(const string& str);
        void set(const basic_string<unsigned int>& str);
        template<class T>
        void addParameter(T d);
        void addParameter(const PlainText& d);
        void clear();
        void resetParameters();
        const basic_string<unsigned int>& aggregatedText();
        string toStdString();
};

} //namespace textManager

#endif // TEXTMANAGER_PLAINTEXT_H
