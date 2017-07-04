
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

#include "Bibliotheque.h"
#include "../Jeu/Jeu.h"
#include "../Attributs/Attributs.h"


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

//Définition des opérateurs d'insertion et d'extraction sur flux fichier pour les classes les plus utilisées

//1. Classe Statistiques

template <class charT, class Traits>
basic_ostream<charT, Traits> &operator<< (basic_ostream<charT, Traits> &flux, const Statistiques &stats)
{
	typename basic_ostream<charT, Traits>::sentry init(flux);
	if (init)
	{
		flux	<< stats["Vitalite"] << " "
				<< stats["Energie"] << " "
				<< stats["Recuperation"];
	}
	return flux;
}

template <class charT, class Traits>
basic_istream<charT, Traits> &operator>> (basic_istream<charT, Traits> &flux, Statistiques &stats)
{
	typename basic_istream<charT, Traits>::sentry init(flux);
	if (init)
	{
		flux >> stats["Vitalite"] >> stats["Energie"] >> stats["Recuperation"];
	}
	return flux;
}

//2. Classe Caracteristiques

template <class charT, class Traits>
basic_ostream<charT, Traits> &operator<< (basic_ostream<charT, Traits> &flux, const Caracteristiques &caracs)
{
	typename basic_ostream<charT, Traits>::sentry init(flux);
	if (init)
	{
		flux	<< caracs["Force"] << " "			<< caracs["Puissance"] << " "
				<< caracs["Agilite"] << " "		<< caracs["Intelligence"] << " "
				<< caracs["Constitution"] << " "	<< caracs["Esquive"] << " "
				<< caracs["Charisme"] << " "		<< caracs["RecuperationMoyenne"] << " "
				<< caracs.Vitesses.size();
		for (MapVitesses::const_iterator i = caracs.Vitesses.begin() ; i != caracs.Vitesses.end() ; ++i)
		{
			flux << " " << i->first << " " << i->second;
		}
	}
	return flux;
}

template <class charT, class Traits>
basic_istream<charT, Traits> &operator>> (basic_istream<charT, Traits> &flux, Caracteristiques &caracs)
{
	typename basic_istream<charT, Traits>::sentry init(flux);
	if (init)
	{
		flux 	>> caracs["Force"]			>> caracs["Puissance"]
				>> caracs["Agilite"]		>> caracs["Intelligence"]
				>> caracs["Constitution"]	>> caracs["Esquive"]
				>> caracs["Charisme"]		>> caracs["RecuperationMoyenne"];
		int size = 0; flux >> size;
		for (int i = 0 ; i < size ; ++i)
		{
			int first = 0, second = 0;
			flux >> first >> second;
			caracs.Vitesses.insert(MapVitesses::value_type(first, second));
		}
	}
	return flux;
}

//3. Classe Objects

template <class charT, class Traits>
basic_istream<charT, Traits> &operator>> (basic_istream<charT, Traits> &flux, Objects &obj)
{
	typename basic_istream<charT, Traits>::sentry init(flux);
	if (init)
	{
		int numberOfObjects = 0;
		flux >> numberOfObjects;
		string fileName = "", key = "", data = "";
		for (int i = 0 ; i < numberOfObjects ; ++i)
		{
			flux >> fileName >> key;
			getline(flux, data);
			obj.loadObjectFromSavedGame(fileName, key, data);
		}
	}
	return flux;
}

#endif
