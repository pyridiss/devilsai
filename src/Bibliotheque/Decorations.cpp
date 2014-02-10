
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

#include "../Jeu/Jeu.h"
#include "../Carte/Carte.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "Bibliotheque.h"
#include "Constantes.h"


/** VARIABLES GLOBALES **/

LigneConsole ConsolePerso[10];
LigneConsole ConsoleAmeliorations[10];

Bouton *MenuSup_Pause;
Bouton *MenuSup_Sauvegarder;
Bouton *MenuSup_Quitter;
Bouton *MenuSup_Repos;

unsigned int NombreMesures = 0;
float SommeFPS = 0;

Shader *blurShader;


/** GESTION DES DÉCORATIONS **/

void Load_Decorations()
{
	AjouterImageDecoration("Fond");
	Set_ImageDecoration("Fond", Color(255,255,255,255), IntRect(0, 0, 400, 300), (float)Options.ScreenW/400.f, (float)Options.ScreenH/300.f);
	AjouterImageDecoration("Frange");
	AjouterImageDecoration("BarreVie");
	AjouterImageDecoration("BarreEnergie");
	AjouterImageDecoration("BarreRecup");
	AjouterImageDecoration("Art");
	AjouterImageDecoration("FondObjet_75_100");
	AjouterImageDecoration("FondObjet_75_75");
	AjouterImageDecoration("FondObjet_50_50");
	AjouterImageDecoration("FondCoffre");
	AjouterImageDecoration("FondInventaire");
	AjouterImageDecoration("FondMiniaturesCompetences");
	AjouterImageDecoration("Repos");
	AjouterImageDecoration("Bouton");
	AjouterImageDecoration("BoutonAppuye");
	AjouterImageDecoration("BoutonPrecedent");
	AjouterImageDecoration("BoutonPrecedentAppuye");
	AjouterImageDecoration("BoutonSuivant");
	AjouterImageDecoration("BoutonSuivantAppuye");
	AjouterImageDecoration("BoutonPersonnage");
	AjouterImageDecoration("BoutonEquipement");
	AjouterImageDecoration("BoutonCompetences");
	AjouterImageDecoration("BoutonJournal");

	Partie.screenCharacter.button.Creer(Options.ScreenW/2 - 2*32 - 4 - 2, 28, 32, 32, "BoutonPersonnage", "BoutonPersonnage");
	Partie.screenCharacter.key = Keyboard::Key::A;
	Partie.screenCharacter.dispFunction = Disp_Personnage;
	Partie.screenCharacter.manageFunction = nullptr;

	Partie.screenEquipment.button.Creer(Options.ScreenW/2 - 1*32 - 2, 28, 32, 32, "BoutonEquipement", "BoutonEquipement");
	Partie.screenEquipment.key = Keyboard::Key::E;
	Partie.screenEquipment.dispFunction = Disp_Equipement;
	Partie.screenEquipment.manageFunction = Gestion_Coffre;

	Partie.screenSkills.button.Creer(Options.ScreenW/2 + 2, 28, 32, 32, "BoutonCompetences", "BoutonCompetences");
	Partie.screenSkills.key = Keyboard::Key::K;
	Partie.screenSkills.dispFunction = Disp_Competences;
	Partie.screenSkills.manageFunction = Gestion_Competences;

	Partie.screenJournal.button.Creer(Options.ScreenW/2 + 32 + 4 + 2, 28, 32, 32, "BoutonJournal", "BoutonJournal");
	Partie.screenJournal.key = Keyboard::Key::J;
	Partie.screenJournal.dispFunction = Disp_Journal;
	Partie.screenJournal.manageFunction = nullptr;

	MenuSup_Pause = new Bouton;
	MenuSup_Pause->Creer(275, 2, 125, 18, "Bouton", "BoutonAppuye");
	MenuSup_Pause->AjouterTexte(Get_Phrase(_MENUJEU_PAUSE));

	MenuSup_Sauvegarder = new Bouton;
	MenuSup_Sauvegarder->Creer(410, 2, 125, 18, "Bouton", "BoutonAppuye");
	MenuSup_Sauvegarder->AjouterTexte(Get_Phrase(_MENUJEU_SAUVEG));

	MenuSup_Quitter = new Bouton;
	MenuSup_Quitter->Creer(Options.ScreenW - 255, 2, 125, 18, "Bouton", "BoutonAppuye");
	MenuSup_Quitter->AjouterTexte(Get_Phrase(_MENUJEU_QUITTER));

	MenuSup_Repos = new Bouton;
	MenuSup_Repos->Creer(31, 130, 125, 18, "Bouton", "BoutonAppuye");
	MenuSup_Repos->AjouterTexte(Get_Phrase(_MENUJEU_REPOS));

	blurShader = new Shader;
	blurShader->loadFromFile(Partie.DATA + "blurShader.frag", Shader::Type::Fragment);

	Load_Decorations_Objets();
}

void Supprimer_Decorations()
{
	delete MenuSup_Pause;
	delete MenuSup_Sauvegarder;
	delete MenuSup_Quitter;
	delete MenuSup_Repos;

	delete blurShader;

	Supprimer_Decorations_Objets();
}

/** FONCTIONS DE LA CLASSE BOUTON **/

void Bouton::Creer(int posX, int posY, int width, int height, string imgRelache, string imgAppuye)
{
	PosX = posX;
	PosY = posY;
	Width = width;
	Height = height;

	strRelache = imgRelache;
	strAppuye = imgAppuye;
}

void Bouton::AjouterTexte(const String32 str)
{
	Texte.setString(str);
	Texte.setCharacterSize(10.);
	Texte.setFont(Font::getDefaultFont());
	FloatRect rect = Texte.getGlobalBounds();
	Texte.setPosition((int)(PosX + Width/2 - rect.width/2 - 1), (int)(PosY + Height/2 - rect.height/2 - 1));
}

void Bouton::AjouterTexte(enumPhrases str)
{
	Texte.setString(Get_Phrase(str));
	Texte.setCharacterSize(10.);
	Texte.setFont(Font::getDefaultFont());
	FloatRect rect = Texte.getGlobalBounds();
	Texte.setPosition((int)(PosX + Width/2 - rect.width/2 - 1), (int)(PosY + Height/2 - rect.height/2 - 1));
}

void Bouton::Creer(int posX, int posY, const String32 &str, float size, Font &font)
{
	Texte.setString(str);
	Texte.setCharacterSize(size);
	Texte.setFont(font);
	FloatRect rect = Texte.getGlobalBounds();
	Width = rect.width + 12;
	Height = rect.height + 6;
	PosX = posX - Width/2; PosY = posY - Height/2;
	Texte.setPosition((int)(PosX + Width/2 - rect.width/2 - 1), (int)(PosY + Height/2 - rect.height/2 - 1));
}

bool Bouton::TestActivation(Event::EventType event)
{
	if (TestSurvol())
	{
		if (event == Event::MouseButtonPressed || event == Event::MouseMoved)
		{
			Etat = true;
			return false;
		}
		if (event == Event::MouseButtonReleased)
		{
			if (Etat)
			{
				Etat = false;
				return true;
			}
			else return false;
		}
	}
	Etat = false;
	return false;
}

bool Bouton::TestSurvol()
{
	if (Mouse::getPosition(Jeu.App).x >= PosX && Mouse::getPosition(Jeu.App).x <= PosX + Width &&
		Mouse::getPosition(Jeu.App).y >= PosY && Mouse::getPosition(Jeu.App).y <= PosY + Height)
		return true;
	return false;
}

int Bouton::GetX()
{
	return PosX;
}
int Bouton::GetY()
{
	return PosY;
}

bool Bouton::Get_Etat()
{
	return Etat;
}

void Bouton::Disp()
{
	if (!Etat)
	{
		if (strRelache != "") Disp_ImageDecoration(strRelache, PosX, PosY);
		Texte.setColor(Color(200,170,0,255));
		Jeu.App.draw(Texte);
	}
	else
	{
		if (strAppuye != "") Disp_ImageDecoration(strAppuye, PosX, PosY);
		if (strAppuye != "" && strAppuye == strRelache) //Manually highlight button
		{
			RectangleShape highlight(Vector2f(Width, Height));
			highlight.setPosition(PosX, PosY);
			highlight.setFillColor(Color(255, 255, 255, 100));
			Jeu.App.draw(highlight);
		}
		Texte.setColor(Color(200,100,0,255));
		Texte.setPosition(Texte.getPosition().x + 1, Texte.getPosition().y + 1);
		Jeu.App.draw(Texte);
		Texte.setPosition(Texte.getPosition().x - 1, Texte.getPosition().y - 1);
	}
}

/** F.P.S. **/

void MoyenneFPS()
{
	cout << "Moyenne FPS : " << SommeFPS << endl;
}

void AjouterMesureFPS(float Mesure)
{
	if (Mesure > 100000) return;	//Évite les 'inf'
	SommeFPS = (NombreMesures * SommeFPS + Mesure)/(NombreMesures + 1.);
	++NombreMesures;
}

void Disp_FPS()
{
	static Clock FrameClock;

	Jeu.FrameTime = FrameClock.getElapsedTime();
	FrameClock.restart();

// 	#ifdef DEBOGAGE
	//Test de FPS trop bas :
// 	if (Jeu.FrameTime.asSeconds() > 5) Erreur("Un blocage a été détecté :", Jeu.FrameTime.asSeconds());
// 	#endif

	#ifdef DEBOGAGE
	static string Texte("");
	static short Compteur = 15;
	static float Framerate;
	static Text FPS("", Font::getDefaultFont(), 10);
	FPS.setPosition(Options.ScreenW - 100, 7);

	if (Arguments.LimitToFpsDisabled) Compteur = 15;

	if (Compteur == 15)
	{
		Framerate = 1000.f/Jeu.FrameTime.asMilliseconds();
		string Texte("FPS : " + intToString(Framerate));
		FPS.setString(Texte);
		Compteur = 0;
	}
	else ++Compteur;

	Jeu.App.draw(FPS);

	#endif

	if (Arguments.LimitToFpsDisabled) AjouterMesureFPS(1000.f/Jeu.FrameTime.asMilliseconds());
}

/** FOND POUR LES MENUS **/

void Disp_FondMenus()
{
	static float Animation = 350;
	static RectangleShape Masque(Vector2f(Options.ScreenW, Options.ScreenH));
	Masque.setFillColor(Color(0,0,0,0));

	Disp_ImageDecoration("Fond", 0, 0);

	Animation += I(0.75);

	if (Animation < 250) Masque.setFillColor(Color((255-Animation)/2,0,0,Animation));
	if (Animation >= 250 && Animation < 350) Masque.setFillColor(Color(0,0,0,250));
	if (Animation >= 350 && Animation < 600) Masque.setFillColor(Color(0,0,(Animation-350)/1.5,600-Animation));
	if (Animation >= 600 && Animation < 700) Masque.setFillColor(Color(0,0,165,0));
	if (Animation >= 700) Animation = 0;

	Jeu.App.draw(Masque);

	Disp_FPS();
}

/** MENU SUPÉRIEUR **/

int Gestion_Menu(Event &event)
{
	if (event.type == Event::MouseButtonPressed || event.type == Event::MouseButtonReleased)
	{
 		if (MenuSup_Pause->TestActivation(event.type)) return ACTION_PAUSE;
		if (!Arguments.SaveDisabled)
			if (MenuSup_Sauvegarder->TestActivation(event.type)) return ACTION_SAUVEG;
		if (MenuSup_Quitter->TestActivation(event.type)) return ACTION_QUITTER;
		if (Partie.perso->LieuVillage)
			if (MenuSup_Repos->TestActivation(event.type)) return ACTION_REPOS;

		if (Partie.screenCharacter.button.TestActivation(event.type))	Partie.changeCurrentUserScreen(&Partie.screenCharacter);
		if (Partie.screenEquipment.button.TestActivation(event.type))	Partie.changeCurrentUserScreen(&Partie.screenEquipment);
		if (Partie.screenSkills.button.TestActivation(event.type))		Partie.changeCurrentUserScreen(&Partie.screenSkills);
		if (Partie.screenJournal.button.TestActivation(event.type))		Partie.changeCurrentUserScreen(&Partie.screenJournal);
	}

	return ACTION_JEU;
}

void Disp_Menu()
{
	static bool Clign = false;
	static float Cmpt = 0;
	Cmpt += I(1);
	if (Cmpt >= 30)
	{
		Cmpt = 0;
		Clign = !Clign;
	}

	for (unsigned short a = 0 ; a < Options.ScreenW/100 + 1 ; ++a)
		Disp_ImageDecoration("Frange", 100*a, 0);

	MenuSup_Pause->Disp();
	if (!Arguments.SaveDisabled) MenuSup_Sauvegarder->Disp();
	MenuSup_Quitter->Disp();
	if (Partie.perso->LieuVillage) MenuSup_Repos->Disp();

	//Erreur éventuelle
	if (Jeu.ErreurDetectee)
	{
		if (Clign)
			Disp_TexteCentre(_ERREUR, Options.ScreenW/2, 32, Color(255,255,255,255), 10.);
		else
			Disp_TexteCentre(_ERREUR, Options.ScreenW/2, 32, Color(255,0,0,255), 10.);
	}

	Partie.screenCharacter.button.Disp();
	Partie.screenEquipment.button.Disp();
	Partie.screenSkills.button.Disp();
	Partie.screenJournal.button.Disp();

	if (Partie.journal.newEntryAdded && Clign)
	{
		RectangleShape mask(Vector2f(32, 32));
		mask.setPosition(Partie.screenJournal.button.GetX(), Partie.screenJournal.button.GetY());
		mask.setFillColor(Color(255, 0, 0, 64));
		Jeu.App.draw(mask);
	}
}


/** CONSOLES DU PERSONNAGE **/

void Disp_JaugesVie()
{
	static int PersoViePrec = Partie.perso->get("Vitalite") - 10;
	static int PersoEnePrec = Partie.perso->get("Energie");
	static unsigned short ClignVie = 0;
	static unsigned short ClignEne = 0;
	static int AncienInteraction = 0;

	static Element_Carte *Elem; static Individu* Ind;

	Disp_TexteCentre(Partie.perso->Nom, 92, 60, Color(128, 255, 128, 255), 12.f);

	//1. Jauges de vitalité, d'énergie, de récupération

	static RectangleShape MasquePerso(Vector2f(100, 7));
	MasquePerso.setFillColor(Color(0, 0, 0, 175));
	MasquePerso.setPosition(42, 70);
	Jeu.App.draw(MasquePerso);
	MasquePerso.setPosition(42, 79);
	Jeu.App.draw(MasquePerso);
	
	if ((signed)Partie.perso->get("Vitalite") != PersoViePrec) PersoViePrec += ((int)Partie.perso->get("Vitalite") - PersoViePrec)/10;

	Set_ImageDecoration("BarreVie", Color(255,255,255,255), IntRect(0, 0, PersoViePrec/10 + 1, 7));
	Set_ImageDecoration("BarreEnergie", Color(255,255,255,255), IntRect(0, 0, Partie.perso->get("Energie")/10, 7));

	int Recup = Partie.perso->get("Recuperation");

	if (Recup > 0)
	{
		Set_ImageDecoration("BarreRecup", Color(255,255,255,255), IntRect(51, 0, (Recup-(Recup/98)*Recup%98)/2, 7));
		Disp_ImageDecoration("BarreRecup", 93, 88);
	}
	if (Recup < 0)
	{
		Set_ImageDecoration("BarreRecup", Color(255,255,255,255), IntRect(51+Recup/2, 0, - Recup/2, 7));
		Disp_ImageDecoration("BarreRecup", 92+Recup/2, 88);
	}

	Disp_ImageDecoration("BarreVie", 42, 70);
	Disp_ImageDecoration("BarreEnergie", 42, 79);

	if (PersoViePrec < 75)
	{
		Set_ImageDecoration("BarreVie", Color(255,255,255,(ClignVie > 255) ? 510-ClignVie : ClignVie), IntRect(0, 0, 102, 7));
		Disp_ImageDecoration("BarreVie", 42, 70);
		Set_ImageDecoration("BarreVie", Color(255,255,255,255), IntRect(0, 0, 100, 7));
		if (PersoViePrec <= 35) ClignVie += 24;
		else ClignVie += 16;
		if (ClignVie > 510) ClignVie = 0;
	}
	if (Partie.perso->get("Energie") < 75)
	{
		Set_ImageDecoration("BarreEnergie", Color(255,255,255,(ClignEne > 255) ? 510-ClignEne : ClignEne), IntRect(0, 0, 102, 7));
		Disp_ImageDecoration("BarreEnergie", 42, 79);
		Set_ImageDecoration("BarreEnergie", Color(255,255,255,255), IntRect(0, 0, 100, 7));
		if (Partie.perso->get("Energie") <= 35) ClignEne += 24;
		else ClignEne += 16;
		if (ClignEne > 510) ClignEne = 0;
	}

	//2. État général, fatigue si nécessaire, effet d'une potion
	if		(Partie.perso->get("Vitalite") == 0)				Disp_TexteCentre(_MORT, 92, 105, Color(168, 168, 168, 255), 11.f);
	else if	(Partie.perso->get("Vitalite") + Recup * 10 >= 900)	Disp_TexteCentre(_SANTE1, 92, 105, Color(128, 255, 128, 255), 11.f);
	else if (Partie.perso->get("Vitalite") + Recup * 10 >= 650)	Disp_TexteCentre(_SANTE2, 92, 105, Color(255, 220, 30, 255), 11.f);
	else if (Partie.perso->get("Vitalite") + Recup * 10 >= 300)	Disp_TexteCentre(_SANTE3, 92, 105, Color(255, 190, 10, 255), 11.f);
	else if (Partie.perso->get("Vitalite") + Recup * 10 >= 100)	Disp_TexteCentre(_SANTE4, 92, 105, Color(255, 80, 10, 255), 11.f);
	else 														Disp_TexteCentre(_SANTE5, 92, 105, Color(255, 0, 0, 255), 11.f);

	if (Partie.perso->get("Energie") < 140)
	{
		if (PersoEnePrec >= 140) Disp_Information(_FATIGUE, true);
		Disp_TexteCentre(_SANTE_FATIGUE, 92, 120, Color(255, 255, 128, 255), 11.f);
	}
	PersoEnePrec = Partie.perso->get("Energie");

	//Effets dûs aux objets temporaires
	int y = 70;
	for (mapObjects::iterator i = Partie.perso->Get_Caracs()->objects.objects.begin() ; i != Partie.perso->Get_Caracs()->objects.objects.end() ; ++i)
	{
		if (getStringFromLUA(i->second, "getIdEmplacement") == i->first)
		{
			if (getStringFromLUA(i->second, "getCategorieObjet") == "temporaire")
			{
				String32 name = Get_NomObjet(getIntFromLUA(i->second, "getInternalNumber"));
				Disp_Texte(Get_PhraseFormatee(_SOUS_EFFET, name), 160, y, Color(255, 255, 128, 255), 11.f);
				y += 12;
			}
		}
	}

	Disp_Information(_TITRE, false);

	//3. Individu en interaction

	static unsigned char TransInd = 0;
	static int sIVit = 0, sIRec = 0;
	static String32 NomInd;
	static Color CouleurNom = Color(255, 255, 255, 255);
	static RectangleShape MasqueInter(Vector2f(100, 7));
	MasqueInter.setPosition(Options.ScreenW - 114, Options.ScreenH/2 - 25);

	if (TransInd != 0)
	{
		Disp_TexteCentre(NomInd, Options.ScreenW - 64, Options.ScreenH/2 - 35, CouleurNom, 12.f);

		MasqueInter.setFillColor(Color(0, 0, 0, (TransInd > 80) ? TransInd-80 : 0));
		Set_ImageDecoration("BarreVie", Color(255,255,255,TransInd), IntRect(0, 0, sIVit/10, 7));

		Jeu.App.draw(MasqueInter);
		Disp_ImageDecoration("BarreVie", Options.ScreenW - 114, Options.ScreenH/2 - 25);

		Set_ImageDecoration("BarreVie", Color(255,255,255,255), IntRect(0,0,0,0));

		if		(sIVit == 0)				Disp_TexteCentre(_MORT, Options.ScreenW - 64, Options.ScreenH/2 - 10, Color(168, 168, 168, TransInd), 11.f);
		else if	(sIVit/10 + sIRec >= 90)	Disp_TexteCentre(_SANTE1, Options.ScreenW - 64, Options.ScreenH/2 - 10, Color(128, 255, 128, TransInd), 11.f);
		else if (sIVit/10 + sIRec >= 65)	Disp_TexteCentre(_SANTE2, Options.ScreenW - 64, Options.ScreenH/2 - 10, Color(255, 220, 30, TransInd), 11.f);
		else if (sIVit/10 + sIRec >= 30)	Disp_TexteCentre(_SANTE3, Options.ScreenW - 64, Options.ScreenH/2 - 10, Color(255, 190, 10, TransInd), 11.f);
		else if (sIVit/10 + sIRec >= 10)	Disp_TexteCentre(_SANTE4, Options.ScreenW - 64, Options.ScreenH/2 - 10, Color(255, 80, 10, TransInd), 11.f);
		else 								Disp_TexteCentre(_SANTE5, Options.ScreenW - 64, Options.ScreenH/2 - 10, Color(255, 0, 0, TransInd), 11.f);
	}

	if (Partie.perso->ElementInteraction != -1 && Partie.perso->ElementInteraction != AncienInteraction)
	{
		Elem = Get_Element(Partie.perso->ElementInteraction);
		if (Elem == NULL) return;

		if (Elem->Diplomatie != 0)
		{
			TransInd = 255;
			Ind = dynamic_cast<Individu*>(Elem);

			if (Ind == NULL)
			{
				Erreur("Disp_JaugesVie() tente d'afficher les statistiques d'un élément non individu", "");
				return;
			}

			NomInd = Ind->Get_Nom();
			if (Elem->Diplomatie == DIPLOM_ALLIE) CouleurNom = Color(128, 255, 128, 255);
			if (Elem->Diplomatie == DIPLOM_ENNEMI) CouleurNom = Color(255, 255, 255, 255);
		}
		else Ind = NULL;
		AncienInteraction = Partie.perso->ElementInteraction;
	}
	if (Ind != NULL && Partie.perso->ElementInteraction != -1)
	{
		sIVit = Ind->get("Vitalite");
		sIRec = Ind->get("Recuperation");
	}

	if ((Ind == NULL || Partie.perso->ElementInteraction == -1) && TransInd > 0)
	{
		Elem = NULL; Ind = NULL;
		TransInd -= 5;
		if (CouleurNom.r == 128) CouleurNom = Color(128, 255, 128, TransInd);
		if (CouleurNom.r == 255) CouleurNom = Color(255, 255, 255, TransInd);
		AncienInteraction = 0;
	}
}

void Ajouter_LignePerso(String32 ligne, Color couleur)
{
	if (!Options.AffichageDegats) return;

	for(int a = 0 ; a < 10 ; ++a)
	{
		if (ConsolePerso[a].Affichage) --ConsolePerso[a].NumLigne;
	}

	short NouvelleLigne = -1;

	for (int b = 0 ; b < 10 ; ++b)
	{
		if (!ConsolePerso[b].Affichage) NouvelleLigne = b;
	}

	if (NouvelleLigne == -1)
	{
		int TempsMax = 0; int Remplacement = 0;
		for (int c = 0 ; c < 10 ; ++c)
		{
			if (ConsolePerso[c].Temps > TempsMax)
			{
				Remplacement = c;
				TempsMax = ConsolePerso[c].Temps;
			}
		}
		NouvelleLigne = Remplacement;
	}

	ConsolePerso[NouvelleLigne].Ligne = ligne;
	ConsolePerso[NouvelleLigne].Couleur = couleur;
	ConsolePerso[NouvelleLigne].Temps = 0;
	ConsolePerso[NouvelleLigne].NumLigne = 10;
	ConsolePerso[NouvelleLigne].Affichage = true;
}

void Ajouter_LigneAmelioration(String32 ligne, Color couleur)
{
	for(int a = 0 ; a < 10 ; ++a)
	{
		if (ConsoleAmeliorations[a].Affichage) --ConsoleAmeliorations[a].NumLigne;
	}

	short NouvelleLigne = -1;

	for (int b = 0 ; b < 10 ; ++b)
	{
		if (!ConsoleAmeliorations[b].Affichage) NouvelleLigne = b;
	}

	if (NouvelleLigne == -1)
	{
		int TempsMax = 0; int Remplacement = 0;
		for (int c = 0 ; c < 10 ; ++c)
		{
			if (ConsoleAmeliorations[c].Temps > TempsMax)
			{
				Remplacement = c;
				TempsMax = ConsoleAmeliorations[c].Temps;
			}
		}
		NouvelleLigne = Remplacement;
	}

	ConsoleAmeliorations[NouvelleLigne].Ligne = ligne;
	ConsoleAmeliorations[NouvelleLigne].Couleur = couleur;
	ConsoleAmeliorations[NouvelleLigne].Temps = 0;
	ConsoleAmeliorations[NouvelleLigne].NumLigne = 10;
	ConsoleAmeliorations[NouvelleLigne].Affichage = true;
}

void Disp_Consoles()
{
	for (int a = 0 ; a < 10 ; ++a)
	{
		if (ConsolePerso[a].Affichage)
		{
			ConsolePerso[a].Temps += I(1);
			if (ConsolePerso[a].Temps < 300)
				Disp_Texte(ConsolePerso[a].Ligne, Options.ScreenW - 115, 90+15*ConsolePerso[a].NumLigne, ConsolePerso[a].Couleur, 13.f);

			if (ConsolePerso[a].Temps == 300) ConsolePerso[a].Affichage = false;

			//Animation de dégradé :
			if (ConsolePerso[a].Temps > 175 && (int)ConsolePerso[a].Temps % 5 == 0)
				ConsolePerso[a].Couleur = Color(ConsolePerso[a].Couleur.r, ConsolePerso[a].Couleur.g, ConsolePerso[a].Couleur.b, 600-ConsolePerso[a].Temps*2);
		}
		if (ConsoleAmeliorations[a].Affichage)
		{
			ConsoleAmeliorations[a].Temps += I(1);
			if (ConsoleAmeliorations[a].Temps < 345)
				Disp_Texte(ConsoleAmeliorations[a].Ligne, 210, 400-16*ConsoleAmeliorations[a].NumLigne, ConsoleAmeliorations[a].Couleur, 13.f);

			if (ConsoleAmeliorations[a].Temps == 300) ConsoleAmeliorations[a].Affichage = false;
		}
	}
}

void SupprimerLignesConsoles()
{
	for (int a = 0 ; a < 10 ; ++a)
	{
		ConsolePerso[a].Affichage = false;
		ConsoleAmeliorations[a].Affichage = false;
	}
}

/** SITUATIONS PARTICULIÈRES **/

void Disp_Pause()
{
	Disp_TexteCentre(_AFFICHAGE_PAUSE, Options.ScreenW/2, Options.ScreenH/2, Color(255,255,255,255), 40., Jeu.DayRoman);
}

void Disp_Information(enumPhrases info, bool reactiver)
{
	static float Temps = 0;
	static enumPhrases Information;
	if (reactiver)
	{
		Temps = 250;
		Information = info;
		return;
	}
	if (Temps > 0)
	{
		Disp_TexteCentre(Information, Options.ScreenW/2, Options.ScreenH/2 - 100, Color(255,255,255,Temps), 20., Jeu.DayRoman);
		Temps -= I((256-Temps)/24);
	}
}

void Disp_Mort()
{
	static RectangleShape Fond(Vector2f(Options.ScreenW, Options.ScreenH));
	//Peut-être moduler la couleur…
	Fond.setFillColor(Color(200, 0, 0, 128));
	Jeu.App.draw(Fond);
	Disp_TexteCentre(_AFFICHAGE_MORT, Options.ScreenW/2, 280, Color(255,255,255,255), 40., Jeu.DayRoman);
	Disp_TexteCentre(_RETOUR_MENU_ENTREE, Options.ScreenW/2, 320, Color(255,255,255,255), 18.);
}

void Disp_FonduNoir(int Commande)
{
	static float Avancement = 0;

	if (Commande > 0 && Avancement == 0)
	{
		Avancement = 1;
	}
	if (Commande < 0) Avancement = 0;
	if (Avancement > 0 && Avancement <= 255)
	{
		static RectangleShape Fond(Vector2f(Options.ScreenW, Options.ScreenH));

		Fond.setFillColor(Color(0, 0, 0, Avancement));
		Jeu.App.draw(Fond);
		Avancement += I(1);
		if (Avancement > 255) Avancement = 255;
	}
}

float Disp_Chargement(float Transparence)
{
	static float OldTransparence = 255;
	if (Transparence <= 0)
	{
		if (OldTransparence != 255) return Transparence;
		else Transparence = 255;
	}
	OldTransparence = Transparence;

	sf::Vertex Degrade[] =
	{
		sf::Vertex(sf::Vector2f(0, 0), Color(240, 240, 240, (int)Transparence)),
		sf::Vertex(sf::Vector2f(0, Options.ScreenH), Color(195, 195, 195, (int)Transparence)),
		sf::Vertex(sf::Vector2f(Options.ScreenW, Options.ScreenH), Color(10, 10, 10, (int)Transparence)),
		sf::Vertex(sf::Vector2f(Options.ScreenW, 0), Color(230, 230, 230, (int)Transparence))
	};

 	Jeu.App.draw(Degrade, 4, sf::Quads);

	Disp_TexteCentre(_CHARGEMENT, Options.ScreenW - 150, Options.ScreenH - 100, Color(0,0,0,255), 45., Jeu.Cardinal);

	Disp_ImageDecoration("Art", Options.ScreenW/2-390, Options.ScreenH/2 - 252);
	Jeu.App.display();

	Transparence -= I((256-Transparence)/2);
	if (Transparence < 0) Transparence = 0;
	return Transparence;
}

bool Disp_Repos()
{
	static float Animation = 0;
	static float T1 = 255, T2 = 255, T3 = 255, T4 = 255;

	Jeu.App.clear(Color::Black);

	Animation += I(1);

	if (Animation < 255)
	{
		if (T1 > 5) T1 -= I(1);
		if (T2 > 5) T2 -= I(0.75);
		if (T3 > 5) T3 -= I(0.5);
	}
	else if (Animation < 510)
	{
		if (T1 < 250) T1 += I(0.25);
		if (T2 < 250) T2 += I(0.5);
		if (T3 > 5) T3 -= I(0.25);
		if (T4 > 5) T4 -= I(1);
	}
	else
	{
		if (T1 < 250) T1 += I(1);
		if (T2 < 250) T2 += I(1);
		if (T3 < 250) T3 += I(1);
		if (T4 < 250) T4 += I(1);
	}
	if (Animation > 800)
	{
		Animation = 0;
		T1 = 255; T2 = 255; T3 = 255; T4 = 255;
		return true;
	}

	sf::Vertex Degrade[] =
	{
		sf::Vertex(sf::Vector2f(0, 0), Color(0, 0, 0, T1)),
		sf::Vertex(sf::Vector2f(0, Options.ScreenH), Color(0, 0, 0, T2)),
		sf::Vertex(sf::Vector2f(Options.ScreenW, Options.ScreenH), Color(0, 0, 0, T3)),
		sf::Vertex(sf::Vector2f(Options.ScreenW, 0), Color(0, 0, 0, T4))
	};

	Disp_ImageDecoration("Repos", Options.ScreenW/2, Options.ScreenH/2, true);
 	Jeu.App.draw(Degrade, 4, sf::Quads);
	return false;
}

void bindBlurShader(int x, int y, int w, int h)
{
	Texture tex;
	tex.create(Options.ScreenW, Options.ScreenH);
	tex.update(Jeu.App);
	RectangleShape rect;
	rect.setSize(Vector2f(w, h));
	rect.setTexture(&tex, true);
	rect.setTextureRect(IntRect(x, y, w, h));
	rect.setPosition(x, y);
	RenderStates states;
	states.shader = blurShader;
	blurShader->setParameter("texture", tex);
	blurShader->setParameter("textureSize", tex.getSize().x, tex.getSize().y);
	Jeu.App.draw(rect, states);
}
