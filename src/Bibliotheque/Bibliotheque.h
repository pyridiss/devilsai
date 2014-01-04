
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

#ifndef header_bibliotheque
#define header_bibliotheque

#include <iostream>
#include <string>
#include <fstream>
#include <list>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class lua_State;
class Element_Carte;
struct Replique;

//Type de base pour les string contenant des textes à afficher
typedef basic_string<Uint32> String32;
typedef list<String32> ListString32;
typedef list<Replique> ListRepliques;

enum enumPhrases{
	_TITRE = 100,
	_ERREUR,
	_CHARGEMENT,
	_SAISIE_NOM,
	_CHOIX_SAUVEGARDE,
	_LANCER_PARTIE,
	_SUPPRIMER_SAUVEGARDE,
	_SAUVEGARDE_VERSION,
	_VERSION_INCOMPATIBLE,
	_AFFICHAGE_PAUSE,
	_SAUVEGARDE,
	_AFFICHAGE_MORT,
	_RETOUR_MENU_ENTREE,
	_RETOUR,
	_RETOUR_MENU,
	_LANCER_DIDACTICIEL,
	_LANCER_CHAP1,
	_MENUPRINCIPAL_NOUVEAU = 200,
	_MENUPRINCIPAL_CHARGER,
	_MENUPRINCIPAL_OPTIONS,
	_MENUPRINCIPAL_QUITTER,
	_MENUJEU_PERSONNAGE,
	_MENUJEU_EQUIPEMENT,
	_MENUJEU_PAUSE,
	_MENUJEU_SAUVEG,
	_MENUJEU_QUITTER,
	_MENUJEU_REPOS,
	_PAS_DE_SAUVEG,
	_ACTIVE = 300,
	_DESACTIVE,
	_SUIVANT,
	_PRECEDENT,
	_LANGUE,
	_PLEINECRAN,
	_AFFICHAGE_DEGATS,
	_RESOLUTION,
	_DEMANDE_REDEMARRAGE,
	_RESOLUTION_IMCOMPATIBLE,
	_PERSONNAGE = 400,
	_SANTE1,
	_SANTE2,
	_SANTE3,
	_SANTE4,
	_SANTE5,
	_MORT,
	_SANTE_FATIGUE,
	_MONSTRES_RESTANTS,
	_CADAVRE,
	_ENERGIE_INSUFFISANTE,
	_SOUS_EFFET,
	_ESQUIVE = 500,
	_CRITIQUE,
	_BLESSURE,
	_FATIGUE,
	_DEGATS_INFLIGES,
	_DEGATS_RECUS,
	_ECHEC,
	_AMELIORATION_FORCE = 600,
	_AMELIORATION_PUISS,
	_AMELIORATION_AGILITE,
	_AMELIORATION_INTELLI,
	_AMELIORATION_CONSTIT,
	_AMELIORATION_ESQUIVE,
	_AMELIORATION_CHARISM,
	_AMELIORATION_RECUP,
	_AMELIORATION_VIT_COURSE,
	_AMELIORATION_VIT_ATTAQUE,
	_AMELIORATION_VIT_BLESSE,
	_PERSO_NIVEAU = 700,
	_PERSO_VITALITE,
	_PERSO_ENERGIE,
	_PERSO_RECUP,
	_PERSO_EXP,
	_PERSO_DEGATS,
	_PERSO_FORCE,
	_PERSO_PUISS,
	_PERSO_AGILITE,
	_PERSO_INTELLI,
	_PERSO_CONSTIT,
	_PERSO_ESQUIVE,
	_PERSO_CHARISM,
	_PERSO_RECUPMOY,
	_EQUIP_VIDE = 800,
	_EQUIP_ARME_VIDE,
	_EQUIP_BOUCLIER_VIDE,
	_EQUIP_CASQUE_VIDE,
	_EQUIP_ARMURE_VIDE,
	_EQUIP_BOTTES_VIDE,
	_EQUIP_BAGUE_VIDE,
	_INVENTAIRE,
	_EQUIP_FORCE,
	_EQUIP_PUISS,
	_EQUIP_AGILITE,
	_EQUIP_INTELLI,
	_EQUIP_CONSTIT,
	_EQUIP_ESQUIVE,
	_EQUIP_CHARISM,
	_EQUIP_RECUP,
	_EQUIP_VIT_COURSE,
	_EQUIP_VIT_ATTAQUE,
	_EQUIP_VIT_BLESSE,
	_EQUIP_MULT_FORCE,
	_EQUIP_MULT_PUISS,
	_EQUIP_MULT_AGILITE,
	_EQUIP_MULT_INTELLI,
	_EQUIP_MULT_CONSTIT,
	_EQUIP_MULT_ESQUIVE,
	_EQUIP_MULT_CHARISM,
	_EQUIP_MULT_RECUP,
	_EQUIP_DUREE
};


/** CLASSES **/

struct LigneConsole
{
	String32 Ligne;
	Color Couleur;
	float Temps		= 0;
	short NumLigne	= 0;
	bool Affichage	= false;
};

struct Langage
{
	string Indice;
	String32 Nom;
};

struct Replique
{
	public:
		String32 Nom;
		String32 Chaine;
		ListString32 Lignes;
		IntRect Rectangle;
		bool Centre = false;

	public:
		Replique();
};

class JournalEntry : public Replique
{
	public:
		string reference = "";
		bool done = false;

	public:
		JournalEntry();
};

class Journal
{
	public:
		list<JournalEntry> journal;

	public:
		void addEntry(string _ref);
		void setDone(string _ref);
		void disp();
};

class Dialogue
{
	private:
		ListRepliques::iterator NumeroReplique;
		float Duree = 0;
		ListRepliques Repliques;

	public:
		string id = "";

	public:
		void Load(string str);
		bool Afficher();
		void Set_Position(int x, int y, bool c);

	private:
		void Unload();
};

class Bouton
{
	private:
		int PosX = 0, PosY = 0;
		int Width = 0, Height = 0;

		string strAppuye;
		string strRelache;

		Text Texte;

		bool Etat = false;

	public:
		void Creer(int posX, int posY, int width, int height, string imgRelache, string imgAppuye);
		void Creer(int posX, int posY, const String32 &str, float size, Font &font);

		void AjouterTexte(const String32);
		void AjouterTexte(enumPhrases str);

		bool TestActivation(Event::EventType event);
		bool TestSurvol();

		int GetX();
		int GetY();

		bool Get_Etat();

		void Disp();
};


/** DÉCLARATION DES FONCTIONS **/

/* Fonctions définies dans le fichier Decorations.cpp */

void Load_Decorations();
void Supprimer_Decorations();
void MoyenneFPS();
void Disp_FPS();
void Disp_FondMenus();
int Gestion_Menu(Event &event);
void Disp_Menu();
void Disp_JaugesVie();
void Ajouter_LignePerso(String32, Color);
void Ajouter_LigneAmelioration(String32, Color);
void Disp_Consoles();
void SupprimerLignesConsoles();
void Disp_Pause();
void Disp_Information(enumPhrases, bool);
void Disp_Mort();
void Disp_FonduNoir(int);
float Disp_Chargement(float);
bool Disp_Repos();

/* Fonctions définies dans le fichier Images.cpp */

void AjouterImagesIndividu(string, short, short, short, float[3][3]);
void AjouterImagePaysage(string, int, int);
void AjouterImagesPaysageMouvant(string, short, short, int, int);
void AjouterImageProjectile(string, short);
void AjouterImagesObjet(unsigned);
void AjouterImageDecoration(string, int = 0, int = 0, float = 1);
void AjouterImageCompetence(string);
void Disp_ImageIndividu(string, short, short, short, float, float, bool = false);
void Disp_ImageCadavre(string, short, short, float, float, bool = false);
void Disp_ImagePaysage(string, float, float, bool = false);
void Disp_ImagePaysageMouvant(string, short, short, float, float, bool = false);
void Disp_ImageProjectile(string, short, float, float, bool = false);
void Disp_ImageObjet(unsigned, short, float, float, bool = false);
void Set_ImageDecoration(string, Color, IntRect, float = 1, float = 1);
void Disp_ImageDecoration(string, float, float, bool = false);
void Disp_ImageCompetence(string, float, float);
void Supprimer_Images();

/* Fonctions définies dans le fichier Langues.cpp */

void Load_LanguesDisponibles();
void SupprimerListe_Langues();
void Load_LangueDevilsai();
String32 Get_NomElement(string);
String32 Get_NomLieu(int);
String32 Get_NomObjet(int);
String32 Get_NomObjet(string);
String32 Get_NomCompetence(string);
String32 Get_DescObjet(int);
String32& Get_Phrase(enumPhrases);
String32 Get_PhraseFormatee(enumPhrases, String32);
void ChangerLangue();
String32 Get_NomLangue();
void DecoupageReplique(Replique*);

/* Fonctions définies dans le fichier Utilitaires.cpp */

bool CreerDossier(string);
bool SupprimerDossier(string);
bool SupprimerFichier(string);
string Get_DossierSauvegardes();
bool getBoolFromLUA(lua_State*, string);
int getIntFromLUA(lua_State*, string);
double getDoubleFromLUA(lua_State*, string);
string getStringFromLUA(lua_State*, string);
void setBoolToLUA(lua_State*, string, bool);
void setIntToLUA(lua_State*, string, int);
void setDoubleToLUA(lua_State*, string, double);
void setStringToLUA(lua_State*, string, string);
void DialogueEtGestionEvenements(string);
int SaisieNom(String32&);
float I(float);
float Minimum(float, float);
float Maximum(float, float);
float Abs(float);
int Abs(int);
long int Abs(long int);
double ToSegment(double, int, int);
int NouveauId();
void Verbose(const string&, const short&);
void Erreur(const string&, const string&);
void Erreur(const string&, const float&);
float DirToCoeff_X(short, float Devn=0);
float DirToCoeff_Y(short, float Devn=0);
bool TestAngle(float&, float&, short, float&, float&, const short&);
void Disp_Texte(const String32&, int, int, Color couleur=Color(255,255,255,255), float Size=16.f, const Font &CharFont=Font::getDefaultFont());
void Disp_Texte(enumPhrases, int, int, Color couleur=Color(255,255,255,255), float Size=16.f, const Font &CharFont=Font::getDefaultFont());
void Disp_TexteCentre(enumPhrases, int, int, Color couleur=Color(255,255,255,255), float Size=16.f, const Font &CharFont=Font::getDefaultFont());
void Disp_TexteCentre(const String32&, int, int, Color couleur=Color(255,255,255,255), float Size=16.f, const Font &CharFont=Font::getDefaultFont());
void Disp_Texte(const string&, int, int, Color couleur=Color(255,255,255,255), float Size=16.f, const Font &CharFont=Font::getDefaultFont());
void Disp_TitrePage(enumPhrases);
string intToString(double);

#endif
