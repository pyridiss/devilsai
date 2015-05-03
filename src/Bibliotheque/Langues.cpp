
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

const int NombrePhrases = 109;

Langage *Langues;

short NombreLangues;


/** GESTION DES FICHIERS DE TYPE PHRASES **/

void Load_LanguesDisponibles()
{
	string fileName = Partie.DATA + "lng/langues";

	ifstream fileStream(fileName, ios_base::in);

	if (fileStream == NULL) Erreur("Le fichier suivant n'a pu être chargé :", fileName);
	if (fileStream != NULL) MESSAGE(" Fichier \"" + fileName + "\" ouvert", FICHIER)

	string TypeDonnee;
	string Buffer;
	int NbLangue = 0;

	while (fileStream.rdstate() == 0)
	{
		fileStream >> TypeDonnee;

		if (TypeDonnee == "NOMBRE_LANGUES")
		{
			fileStream >> NombreLangues;
			Langues = new Langage[NombreLangues];
		}
		else if (TypeDonnee != "")
		{
			fileStream >> Langues[NbLangue].Nom;
			Langues[NbLangue].Indice = TypeDonnee;
			++NbLangue;
		}
		TypeDonnee = "";

		if (NbLangue > NombreLangues)
		{
			Erreur("Il n'y a pas autant de langues que le précise le fichier", fileName);
			break;
		}
	}

	fileStream.close();
}

void SupprimerListe_Langues()
{
	if (Langues != NULL) delete[] Langues;
	Langues = NULL;

	MESSAGE("Liste des Langues supprimée", LISTE)
}

void Load_LangueDevilsai()
{
	string fileName = Partie.DATA + "lng/devilsai.lng";

	ifstream fileStream(fileName, ios_base::in);

	if (fileStream == NULL) Erreur("Le fichier suivant n'a pu être chargé :", fileName);
	if (fileStream != NULL) MESSAGE(" Fichier \"" + fileName + "\" ouvert", FICHIER)

	string TypeDonnee;
	string Buffer;
	int NbPhrase = 0;
	String32 phrase;

	int Numero;

	Jeu.Dico.clear();

	while (fileStream.rdstate() == 0)
	{
		fileStream >> TypeDonnee;

		if (TypeDonnee == "#") getline(fileStream, Buffer); //Comments

		else if (TypeDonnee == "*") fileStream >> Buffer >> Numero;
		else if (TypeDonnee == Options.Langue)
		{
			if (NbPhrase >= NombrePhrases)
			{
				Erreur("Le fichier de langue contient plus de phrases que le programme n'en a", "");
				break;
			}
			fileStream.get();
			fileStream >> phrase;
			Jeu.Dico.insert(Dictionnaire::value_type((enumPhrases)Numero, phrase));
			++NbPhrase;
		}
		else getline(fileStream, Buffer);
		TypeDonnee = ""; Buffer = "";
		phrase.clear();
	}

	fileStream.close();
}

template <class T>
String32 Get_Nom(string fichier, T Indice)
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

String32 Get_NomElement(string Indice)
{
	return Get_Nom(Partie.DATA + "lng/noms_elements.lng", Indice);
}
String32 Get_NomLieu(int Indice)
{
	return Get_Nom(Partie.DATA + "lng/noms_lieux.lng", Indice);
}
String32 Get_NomObjet(int Indice)
{
	return Get_Nom(Partie.DATA + "lng/noms_objets.lng", Indice);
}
String32 Get_NomObjet(string Indice)
{
	return Get_Nom(Partie.DATA + "lng/noms_objets.lng", Indice);
}
String32 Get_NomCompetence(string Indice)
{
	return Get_Nom(Partie.DATA + "lng/noms_competences.lng", Indice);
}
String32 Get_DescObjet(int Indice)
{
	return Get_Nom(Partie.DATA + "lng/desc_objets.lng", Indice);
}

String32& Get_Phrase(enumPhrases phr)
{
	Dictionnaire::iterator i = Jeu.Dico.find(phr);
	if (i != Jeu.Dico.end()) return i->second;

	Erreur("La phrase suivante a été demandée sans avoir été chargée :", phr);
	return Jeu.Dico.find(_ERREUR)->second;
}

//Cette fonction est définie de manière template dans Templates.h
//Redéfinition dans le cas où arg est une String32 : on peut alors éviter les multiples conversions
String32 Get_PhraseFormatee(enumPhrases phr, String32 arg)
{
	String32 str32 = Get_Phrase(phr);

	String32::size_type posForm = str32.find(Options.CharForm);

	if (posForm != String32::npos)
	{
		str32.erase(posForm, Options.CharForm.size());
		str32.insert(posForm, arg);
	}
	return str32;
}

void ChangerLangue()
{
	int i = 0;
	for ( ; i < NombreLangues ; ++i)
		if (Langues[i].Indice == Options.Langue) break;

	if (i == NombreLangues -1) i = 0;
	else ++i;

	Options.Langue = Langues[i].Indice;

	Load_LangueDevilsai();
}

String32 Get_NomLangue()
{
	int i = 0;
	for ( ; i < NombreLangues ; ++i)
		if (Langues[i].Indice == Options.Langue) break;

	return Langues[i].Nom;
}

void DecoupageReplique(Replique* Replique)
{
	Text Texte("", getDefaultFont(), 12);

	//Avant le découpage, on va détecter la présence de caractères de formatage
	size_t Formatage = Replique->Chaine.find(Options.CharForm);

	while (Partie.perso != NULL && Formatage != Replique->Chaine.npos)
	{
		Replique->Chaine.replace(Formatage, Options.CharForm.size(), Partie.perso->Nom);
		Formatage = Replique->Chaine.find(Options.CharForm);
	}

	String32 buf = Replique->Chaine;
	size_t CoupureInitiale = 0, CoupureBuf = 0;
	while (CoupureInitiale < Replique->Chaine.size())
	{
		Texte.setString(buf);
		CoupureBuf = buf.size() - 1;

		while (Texte.getLocalBounds().width > Replique->Rectangle.width)
		{
			CoupureBuf = buf.find_last_of(Options.CharSpace);
			if (CoupureBuf == buf.npos)
			{
				//Le mot est trop long pour être affiché dans le cadre ; on le fait dépasser plutôt que de le couper
				CoupureBuf = Replique->Chaine.substr(CoupureInitiale).find_first_of(Options.CharSpace);
				break;
			}
			buf = buf.substr(0, CoupureBuf);
			Texte.setString(buf);
		}
		if (buf.find(Options.CharEOL) != buf.npos)
		{
			CoupureBuf = buf.find(Options.CharEOL);
			buf = buf.substr(0, CoupureBuf);
			Texte.setString(buf);
			++CoupureBuf;
		}

		Replique->Lignes.push_back(buf);

		CoupureInitiale += CoupureBuf + 1;
		buf = Replique->Chaine.substr(CoupureInitiale);
	}
}


/** FONCTIONS DE LA CLASSE Dialogue **/

Replique::Replique()
{
	Rectangle.width = Options.ScreenW / 2;
}

void Dialogue::Load(string str)
{
	id = str;

	string fileName = Partie.DATA + "lng/" + str;

	ifstream fileStream(fileName, ios_base::in);

	if (fileStream == NULL) Erreur("Le fichier suivant n'a pu être chargé :", fileName);
	if (fileStream != NULL) MESSAGE(" Fichier \"" + fileName +"\" ouvert", FICHIER)

	Repliques.clear();

	string TypeDonnee = "", TypeDonnee2 = "";
	string Buffer;

	while (fileStream.rdstate() == 0)
	{
		fileStream >> TypeDonnee;

		if (TypeDonnee == "#") getline(fileStream, Buffer);

		else if (TypeDonnee == "*")
		{
			Replique r;
			Repliques.push_back(r);
			for (ListRepliques::iterator i = Repliques.begin() ; i != Repliques.end() ; ++i) NumeroReplique = i;
			fileStream >> TypeDonnee2;
			if (TypeDonnee2 == "POSITION") fileStream >> NumeroReplique->Rectangle.left >> NumeroReplique->Rectangle.top;
			else
			{
				Erreur ("Le fichier de langue suivant est corrompu :", fileName);
				break;
			}
		}
		else if (TypeDonnee == "LARGEUR")
		{
			fileStream >> NumeroReplique->Rectangle.width;
		}
		else if (TypeDonnee == "NOM")
		{
			fileStream >> TypeDonnee2;
			if (TypeDonnee2 == Options.Langue || TypeDonnee2 == "TOUTES_LANGUES")
				fileStream >> NumeroReplique->Nom;
		}
		else if (TypeDonnee == Options.Langue)
		{
			fileStream >> NumeroReplique->Chaine;
		}
		else getline(fileStream, Buffer);

		TypeDonnee = ""; TypeDonnee2 = ""; Buffer = "";
	}

	NumeroReplique = Repliques.begin();
	if (NumeroReplique != Repliques.end()) DecoupageReplique(&*NumeroReplique);

	fileStream.close();
}

void Dialogue::Unload()
{
	Repliques.clear();
}

void Dialogue::Set_Position(int x, int y, bool c)
{
	for (auto& i : Repliques)
	{
		i.Rectangle.left = x;
		i.Rectangle.top = y;
		i.Centre = c;
	}
}

bool Dialogue::Afficher()
{
	if (Repliques.empty()) return true;

	bindBlurShader(Repliques.begin()->Rectangle.left - 20, Repliques.begin()->Rectangle.top, Options.ScreenW / 2, 26 + NumeroReplique->Lignes.size() * 16);

	Disp_Texte(NumeroReplique->Nom, NumeroReplique->Rectangle.left, NumeroReplique->Rectangle.top, Color(255,220,220,255), 20, Jeu.DayRoman);

	Text Texte("", getDefaultFont(), 11);

	int Numero = 0;
	for (auto& i : NumeroReplique->Lignes)
	{
		Texte.setString(i);
		if (NumeroReplique->Centre) Texte.setPosition(Vector2f(NumeroReplique->Rectangle.left - (int)(Texte.getLocalBounds().width/2), 26 + NumeroReplique->Rectangle.top + 16*Numero));
		else Texte.setPosition(NumeroReplique->Rectangle.left, 26 + NumeroReplique->Rectangle.top + 16*Numero);
		++Numero;
		Jeu.App.draw(Texte);
	}

	Duree += I(1);

	if (Duree > 200 + 2.5*NumeroReplique->Chaine.size())
	{
		++NumeroReplique;
		Duree = 0;

		if (NumeroReplique == Repliques.end())
		{
			Unload();
			return true;
		}
		else DecoupageReplique(&*NumeroReplique);
	}

	return false;
}

JournalEntry::JournalEntry()
{
	Rectangle.left = 20;
	Rectangle.top = Options.ScreenH - 10;
	Rectangle.width = Options.ScreenW / 2;
}

void Journal::addEntry(string _ref)
{
	string fileName = Partie.DATA + "lng/journal.lng";

	ifstream fileStream(fileName, ios_base::in);

	if (fileStream == NULL) Erreur("Le fichier suivant n'a pu être chargé :", fileName);
	if (fileStream != NULL) MESSAGE(" Fichier \"" + fileName +"\" ouvert", FICHIER)

	JournalEntry newEntry;
	journal.push_back(newEntry);
	JournalEntry& entry = journal.back();
	entry.reference = _ref;

	string TypeDonnee = "", TypeDonnee2 = "";
	string Buffer;
	string readRef = "";

	while (fileStream.rdstate() == 0)
	{
		fileStream >> TypeDonnee;

		if (TypeDonnee == "#") getline(fileStream, Buffer); //Comments

		else if (TypeDonnee == "*")
		{
			fileStream >> readRef;
			fileStream.get();
		}
		else if (TypeDonnee == "NOM")
		{
			fileStream >> TypeDonnee;
			if (TypeDonnee == Options.Langue && readRef == _ref)
				fileStream >> entry.Nom;
		}
		else if (TypeDonnee == Options.Langue && readRef == _ref)
		{
			fileStream.get();
			fileStream >> entry.Chaine;
		}
		else getline(fileStream, Buffer);
		TypeDonnee = ""; Buffer = "";
	}

	DecoupageReplique(&entry);

	newEntryAdded = true;

	fileStream.close();
}

void Journal::setDone(string _ref)
{
	for (auto& i : journal)
		if (i.reference == _ref)
		{
			i.done = true;
			break;
		}
}

void Disp_Journal()
{
	if (Partie.journal.journal.empty()) return;

	Partie.journal.newEntryAdded = false;

	bindBlurShader(10, 170, Options.ScreenW / 2 - 20, Options.ScreenH - 170 - 10);

	int numberOfLine = 0;
	int opacity = 255;

	for (auto entry = Partie.journal.journal.rbegin() ; entry != Partie.journal.journal.rend() ; ++entry)
	{
		Text Texte("", getDefaultFont(), 11);

		for (auto i = entry->Lignes.rbegin() ; i != entry->Lignes.rend() ; ++i)
		{
			numberOfLine += 16;
			Texte.setString(*i);
			Texte.setPosition(entry->Rectangle.left, entry->Rectangle.top - numberOfLine);

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
		Disp_Texte(entry->Nom, entry->Rectangle.left, entry->Rectangle.top - numberOfLine, Color(255,220,220,opacity), 16, Jeu.DayRoman);
		numberOfLine += 10;
	}
}