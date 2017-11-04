
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2014  Quentin Henriet

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

#ifndef header_templates
#define header_templates

#include <sstream>

typedef basic_string<Uint32> String32;

//Ce fichier regroupe les fonctions templates utilisées par le programme

//Opérateur d'affichage des String32 (sous forme numérique) pour la sortie standard
template <class charT, class Traits>
basic_ostream<charT, Traits> &operator<< (basic_ostream<charT, Traits> &flux, String32 str)
{
	typename basic_ostream<charT, Traits>::sentry init(flux);
	if (init)
	{
		for (unsigned i = 0 ; i < str.length() ; ++i)
			flux	<< str[i] << " ";
	}
	return flux;
}


//Opérateur de lecture sur String32 depuis un fichier
template <class charT, class Traits>
basic_istream<charT, Traits> &operator>> (basic_istream<charT, Traits> &flux, String32 &str)
{
	typename basic_istream<charT, Traits>::sentry init(flux);
	if (init)
	{
		string Buffer;
		getline(flux, Buffer);
		str.clear();
		Utf8::toUtf32(Buffer.begin(), Buffer.end(), back_inserter(str));
	}
	return flux;
}

template <class charT, class Traits, class T>
basic_istream<charT, Traits> &operator>> (basic_istream<charT, Traits> &flux, T& t)
{
	typename basic_istream<charT, Traits>::sentry init(flux);
	if (init)
	{
		int i;
		flux >> i;
		t = (T)i;
	}
    return flux;
}

#endif
