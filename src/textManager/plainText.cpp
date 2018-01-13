
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

#include <sstream>

#include "textManager/plainText.h"

namespace textManager{

PlainText::PlainText()
  : _originalText(),
    _parameters(),
    _parametersNeeded(0),
    _aggregatedText()
{
}

PlainText::PlainText(const string& str)
  : PlainText()
{
    set(str);
}

PlainText::PlainText(const basic_string<unsigned int>& str)
  : PlainText()
{
    set(str);
}

PlainText& PlainText::operator+=(const PlainText& right)
{
    _originalText += right._originalText;
    _parameters.insert(_parameters.end(), right._parameters.begin(), right._parameters.end());
    _parametersNeeded += right._parametersNeeded;
    _aggregatedText += right._aggregatedText;

    return *this;
}

PlainText& PlainText::operator+=(const char* right)
{
    PlainText r(right);
    return operator+=(r);
}

PlainText::~PlainText()
{
}

void PlainText::aggregate()
{
    _aggregatedText = _originalText;

    unsigned parameterCharacter = 42; // '*', parameter

    size_t pos = _aggregatedText.find(parameterCharacter);
    unsigned count = 0;

    while (pos != string::npos && count < _parameters.size())
    {
        _aggregatedText.erase(pos, 1);
        _aggregatedText.insert(pos, _parameters[count]);
        pos = _aggregatedText.find(parameterCharacter, pos+1);
        ++count;
    }
}

void PlainText::findParametersNeeded()
{
    unsigned parameterCharacter = 42; // '*', parameter

    size_t pos = _originalText.find(parameterCharacter);
    while(pos != string::npos)
    {
        ++_parametersNeeded;
        if (pos != _originalText.size())
            pos = _originalText.find(parameterCharacter, pos+1);
        else
            break;
    }
}

void PlainText::set(const string& str)
{
    clear();

    Utf8::toUtf32(str.begin(), str.end(), back_inserter(_originalText));

    findParametersNeeded();
}

void PlainText::set(const basic_string<unsigned int>& str)
{
    clear();

    _originalText = str;

    findParametersNeeded();
}

template<class T>
void PlainText::addParameter(T d)
{
    stringstream out;
    basic_string<unsigned int> var;

    out << d;

    string tmp = out.str();
    Utf8::toUtf32(tmp.begin(), tmp.end(), back_inserter(var));

    _parameters.push_back(std::move(var));

    aggregate();
}

//Explicit instantiations for the linker
template void PlainText::addParameter(int d);
template void PlainText::addParameter(unsigned d);

void PlainText::addParameter(const PlainText& d)
{
    _parameters.push_back(d._originalText);

    aggregate();
}

void PlainText::clear()
{
    _originalText.clear();
    _parameters.clear();
    _parametersNeeded = 0;
    _aggregatedText.clear();
}

void PlainText::resetParameters()
{
    _parameters.clear();
    _aggregatedText.clear();
}

const basic_string<unsigned int>& PlainText::aggregatedText()
{
    if (_parameters.size() < _parametersNeeded)
    {
        tools::debug::error("This text needs more parameters:", "textManager", __FILENAME__, __LINE__);
        tools::debug::error(toStdString(), "textManager", __FILENAME__, __LINE__);
        return _originalText;
    }

    if (_parametersNeeded == 0)
        return _originalText;

    if (_aggregatedText.empty())
        aggregate();

    return _aggregatedText;
}

string PlainText::toStdString() const
{
    string str;

    Utf32::toUtf8(_originalText.begin(), _originalText.end(), back_inserter(str));

    return str;
}

} //namespace textManager