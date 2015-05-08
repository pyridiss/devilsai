
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

#include <fstream>
#include <map>

#include "../Jeu/Jeu.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "Bibliotheque.h"
#include "Constantes.h"
#include "Templates.h"


/** VARIABLES GLOBALES **/

const int numberOfMessages = 109;

Language *languages;

short numberOfLanguages;


/** GESTION DES FICHIERS DE TYPE PHRASES **/

void loadAvailableLanguages()
{
	string fileName = Partie.DATA + "lng/langues";

	ifstream fileStream(fileName, ios_base::in);

	if (fileStream == NULL) Erreur("Le fichier suivant n'a pu être chargé :", fileName);
	if (fileStream != NULL) MESSAGE(" Fichier \"" + fileName + "\" ouvert", FICHIER)

	string dataType;
	int counter = 0;

	while (fileStream.rdstate() == 0)
	{
		fileStream >> dataType;

		if (dataType == "NOMBRE_LANGUES")
		{
			fileStream >> numberOfLanguages;
			languages = new Language[numberOfLanguages];
		}
		else if (dataType != "")
		{
			fileStream >> languages[counter].name;
			languages[counter].shortName = dataType;
			++counter;
		}
		      dataType = "";

		if (counter > numberOfLanguages)
		{
			Erreur("Il n'y a pas autant de langues que le précise le fichier", fileName);
			break;
		}
	}

	fileStream.close();
}

void deleteLanguagesList()
{
	if (languages != NULL) delete[] languages;
	languages = NULL;

	MESSAGE("Liste des Langues supprimée", LISTE)
}

void loadDevilsaiMessages()
{
	string fileName = Partie.DATA + "lng/devilsai.lng";

	ifstream fileStream(fileName, ios_base::in);

	if (fileStream == NULL) Erreur("Le fichier suivant n'a pu être chargé :", fileName);
	if (fileStream != NULL) MESSAGE(" Fichier \"" + fileName + "\" ouvert", FICHIER)

	string dataType;
	string buffer;
	int counter = 0;
	String32 message;

	int messageNumber;

	Jeu.Dico.clear();

	while (fileStream.rdstate() == 0)
	{
		fileStream >> dataType;

		if (dataType == "#") getline(fileStream, buffer); //Comments

		else if (dataType == "*") fileStream >> buffer >> messageNumber;
		else if (dataType == Options.Langue)
		{
			if (counter >= numberOfMessages)
			{
				Erreur("Le fichier de langue contient plus de phrases que le programme n'en a", "");
				break;
			}
			fileStream.get();
			fileStream >> message;
			Jeu.Dico.insert(Dictionnaire::value_type((enumPhrases)messageNumber, message));
			++counter;
		}
		else getline(fileStream, buffer);
		dataType = ""; 
        buffer = "";
		message.clear();
	}

	fileStream.close();
}

template <class T>
String32 getTranslatedName(string fichier, T Indice)
{
	//Cette fonction peut gérer les indices numériques ou alphanumériques
	ifstream fileStream(fichier, ios_base::in);

	if (fileStream == NULL) Erreur("Le fichier suivant n'a pu être chargé :", fichier);
	if (fileStream != NULL) MESSAGE(" Fichier \"" + fichier + "\" ouvert", FICHIER)

	String32 Nom;
	bool NomTrouve = false;
	T NumCourant = Indice;
	string TypeDonnee; string Buffer;

	while (fileStream.rdstate() == 0 && !NomTrouve)
	{
		fileStream >> TypeDonnee;

		if (TypeDonnee == "#") getline(fileStream, Buffer); //Comments

		else if (TypeDonnee == "*") fileStream >> NumCourant;
		else if (TypeDonnee == Options.Langue && Indice == NumCourant)
		{
			fileStream.get();
			fileStream >> Nom;
		}
		else getline(fileStream, Buffer);
		TypeDonnee = ""; Buffer = "";
	}

	fileStream.close();

	return Nom;
}

String32 getTranslatedNameOfElement(string Indice)
{
	return getTranslatedName(Partie.DATA + "lng/noms_elements.lng", Indice);
}
String32 getTranslatedNameOfPlace(int Indice)
{
	return getTranslatedName(Partie.DATA + "lng/noms_lieux.lng", Indice);
}
String32 getTranslatedNameOfObject(int Indice)
{
	return getTranslatedName(Partie.DATA + "lng/noms_objets.lng", Indice);
}
String32 getTranslatedNameOfObject(string Indice)
{
	return getTranslatedName(Partie.DATA + "lng/noms_objets.lng", Indice);
}
String32 getTranslatedNameOfSkill(string Indice)
{
	return getTranslatedName(Partie.DATA + "lng/noms_competences.lng", Indice);
}
String32 getTranslatedDescriptionOfObject(int Indice)
{
	return getTranslatedName(Partie.DATA + "lng/desc_objets.lng", Indice);
}

String32& getTranslatedMessage(enumPhrases msg)
{
	Dictionnaire::iterator i = Jeu.Dico.find(msg);
	if (i != Jeu.Dico.end()) return i->second;

	Erreur("La phrase suivante a été demandée sans avoir été chargée :", msg);
	return Jeu.Dico.find(_ERREUR)->second;
}

//Cette fonction est définie de manière template dans Templates.h
//Redéfinition dans le cas où arg est une String32 : on peut alors éviter les multiples conversions
String32 getFormatedTranslatedMessage(enumPhrases msg, String32 arg)
{
	String32 str32 = getTranslatedMessage(msg);

	String32::size_type posForm = str32.find(Options.CharForm);

	if (posForm != String32::npos)
	{
		str32.erase(posForm, Options.CharForm.size());
		str32.insert(posForm, arg);
	}
	return str32;
}

void changeLanguage()
{
	int i = 0;
	for ( ; i < numberOfLanguages ; ++i)
		if (languages[i].shortName == Options.Langue) break;

	if (i == numberOfLanguages -1) i = 0;
	else ++i;

	Options.Langue = languages[i].shortName;

	loadDevilsaiMessages();
}

String32 getNameOfLanguage()
{
	int i = 0;
	for ( ; i < numberOfLanguages ; ++i)
		if (languages[i].shortName == Options.Langue) break;

	return languages[i].name;
}

void cutParagraph(Paragraph* paragraph)
{
	Text text("", getDefaultFont(), 12);

	//Before cutting, detect the presence of formatting characters
	size_t formattingCharacterPosition = paragraph->characters.find(Options.CharForm);

	while (Partie.perso != NULL && formattingCharacterPosition != paragraph->characters.npos)
	{
		paragraph->characters.replace(formattingCharacterPosition, Options.CharForm.size(), Partie.perso->Nom);
		formattingCharacterPosition = paragraph->characters.find(Options.CharForm);
	}

	String32 buffer = paragraph->characters;
	size_t firstCut = 0, lastCut = 0;
	while (firstCut < paragraph->characters.size())
	{
		text.setString(buffer);
		lastCut = buffer.size() - 1;

		while (text.getLocalBounds().width > paragraph->rectangle.width)
		{
			lastCut = buffer.find_last_of(Options.CharSpace);
			if (lastCut == buffer.npos)
			{
				//Word too long to be displayed; exceed the frame rather than cutting the word
				lastCut = paragraph->characters.substr(firstCut).find_first_of(Options.CharSpace);
				break;
			}
			buffer = buffer.substr(0, lastCut);
			text.setString(buffer);
		}
		if (buffer.find(Options.CharEOL) != buffer.npos)
		{
			lastCut = buffer.find(Options.CharEOL);
			buffer = buffer.substr(0, lastCut);
			text.setString(buffer);
			++lastCut;
		}

		paragraph->lines.push_back(buffer);

		firstCut += lastCut + 1;
		buffer = paragraph->characters.substr(firstCut);
	}
}


/** Class 'Dialog' functions **/

Paragraph::Paragraph()
{
	rectangle.width = Options.ScreenW / 2;
}

void Dialog::load(string str)
{
	id = str;

	string fileName = Partie.DATA + "lng/" + str;

	ifstream fileStream(fileName, ios_base::in);

	if (fileStream == NULL) Erreur("Le fichier suivant n'a pu être chargé :", fileName);
	if (fileStream != NULL) MESSAGE(" Fichier \"" + fileName +"\" ouvert", FICHIER)

	paragraphs.clear();

	string dataType = "", dataType2 = "";
	string buffer;

	while (fileStream.rdstate() == 0)
	{
		fileStream >> dataType;

		if (dataType == "#") getline(fileStream, buffer);

		else if (dataType == "*")
		{
			Paragraph r;
			paragraphs.push_back(r);
			for (ListParagraph::iterator i = paragraphs.begin() ; i != paragraphs.end() ; ++i) paragraphNumber = i;
			fileStream >> dataType2;
			if (dataType2 == "POSITION") fileStream >> paragraphNumber->rectangle.left >> paragraphNumber->rectangle.top;
			else
			{
				Erreur ("Le fichier de langue suivant est corrompu :", fileName);
				break;
			}
		}
		else if (dataType == "LARGEUR")
		{
			fileStream >> paragraphNumber->rectangle.width;
		}
		else if (dataType == "NOM")
		{
			fileStream >> dataType2;
			if (dataType2 == Options.Langue || dataType2 == "TOUTES_LANGUES")
				fileStream >> paragraphNumber->name;
		}
		else if (dataType == Options.Langue)
		{
			fileStream >> paragraphNumber->characters;
		}
		else getline(fileStream, buffer);

		dataType = ""; 
		dataType2 = ""; 
		buffer = "";
	}

	paragraphNumber = paragraphs.begin();
	if (paragraphNumber != paragraphs.end()) cutParagraph(&*paragraphNumber);

	fileStream.close();
}

void Dialog::unload()
{
	paragraphs.clear();
}

void Dialog::setPosition(int x, int y, bool c)
{
	for (auto& i : paragraphs)
	{
		i.rectangle.left = x;
		i.rectangle.top = y;
		i.centered = c;
	}
}

bool Dialog::display()
{
	if (paragraphs.empty()) return true;

	bindBlurShader(paragraphs.begin()->rectangle.left - 20, paragraphs.begin()->rectangle.top, Options.ScreenW / 2, 26 + paragraphNumber->lines.size() * 16);

	Disp_Texte(paragraphNumber->name, paragraphNumber->rectangle.left, paragraphNumber->rectangle.top, Color(255,220,220,255), 20, Jeu.DayRoman);

	Text text("", getDefaultFont(), 11);

	int counter = 0;
	for (auto& i : paragraphNumber->lines)
	{
		text.setString(i);
		if (paragraphNumber->centered) text.setPosition(Vector2f(paragraphNumber->rectangle.left - (int)(text.getLocalBounds().width/2), 26 + paragraphNumber->rectangle.top + 16*counter));
		else text.setPosition(paragraphNumber->rectangle.left, 26 + paragraphNumber->rectangle.top + 16*counter);
		++counter;
		Jeu.App.draw(text);
	}

	duration += I(1);

	if (duration > 200 + 2.5*paragraphNumber->characters.size())
	{
		++paragraphNumber;
		duration = 0;

		if (paragraphNumber == paragraphs.end())
		{
			unload();
			return true;
		}
		else cutParagraph(&*paragraphNumber);
	}

	return false;
}

JournalEntry::JournalEntry()
{
	rectangle.left = 20;
	rectangle.top = Options.ScreenH - 10;
	rectangle.width = Options.ScreenW / 2;
}

void Journal::addEntry(string _ref)
{
	string fileName = Partie.DATA + "lng/journal.lng";

	ifstream fileStream(fileName, ios_base::in);

	if (fileStream == NULL) Erreur("Le fichier suivant n'a pu être chargé :", fileName);
	if (fileStream != NULL) MESSAGE(" Fichier \"" + fileName +"\" ouvert", FICHIER)

	JournalEntry newEntry;
	entries.push_back(newEntry);
	JournalEntry& entry = entries.back();
	entry.reference = _ref;

	string dataType = "";
	string buffer;
	string readRef = "";

	while (fileStream.rdstate() == 0)
	{
		fileStream >> dataType;

		if (dataType == "#") getline(fileStream, buffer); //Comments

		else if (dataType == "*")
		{
			fileStream >> readRef;
			fileStream.get();
		}
		else if (dataType == "NOM")
		{
			fileStream >> dataType;
			if (dataType == Options.Langue && readRef == _ref)
				fileStream >> entry.name;
		}
		else if (dataType == Options.Langue && readRef == _ref)
		{
			fileStream.get();
			fileStream >> entry.characters;
		}
		else getline(fileStream, buffer);
		dataType = ""; 
        buffer = "";
	}

	cutParagraph(&entry);

	newEntryAdded = true;

	fileStream.close();
}

void Journal::setDone(string _ref)
{
	for (auto& i : entries)
		if (i.reference == _ref)
		{
			i.done = true;
			break;
		}
}

void displayJournal()
{
	if (Partie.journal.entries.empty()) return;

	Partie.journal.newEntryAdded = false;

	bindBlurShader(10, 170, Options.ScreenW / 2 - 20, Options.ScreenH - 170 - 10);

	int numberOfLine = 0;
	int opacity = 255;

	for (auto entry = Partie.journal.entries.rbegin() ; entry != Partie.journal.entries.rend() ; ++entry)
	{
		Text Texte("", getDefaultFont(), 11);

		for (auto i = entry->lines.rbegin() ; i != entry->lines.rend() ; ++i)
		{
			numberOfLine += 16;
			Texte.setString(*i);
			Texte.setPosition(entry->rectangle.left, entry->rectangle.top - numberOfLine);

			if (numberOfLine >= (int)Options.ScreenH - 170 - 20 - 64) opacity = 255 - 4*(numberOfLine - (Options.ScreenH - 170 - 20 - 64));
			if (numberOfLine >= (int)Options.ScreenH - 170 - 20) return;

			if (entry->done) Texte.setColor(Color(180, 180, 180, opacity));
			else Texte.setColor(Color(255, 255, 255, opacity));
			Jeu.App.draw(Texte);

			if (entry->done)
			{
				RectangleShape strikeLine;
				strikeLine.setSize(Vector2f(Texte.getLocalBounds().width, 1));
				strikeLine.setFillColor(Color(180, 180, 180, opacity));
				strikeLine.setPosition(Texte.getGlobalBounds().left, Texte.getGlobalBounds().top + 6);
				Jeu.App.draw(strikeLine);
			}
		}

		numberOfLine += 22;
		Disp_Texte(entry->name, entry->rectangle.left, entry->rectangle.top - numberOfLine, Color(255,220,220,opacity), 16, Jeu.DayRoman);
		numberOfLine += 10;
	}
}
