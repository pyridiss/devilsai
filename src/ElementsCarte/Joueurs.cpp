
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

#include <lua5.2/lua.hpp>

#include <cmath>

#include "../Bibliotheque/Bibliotheque.h"
#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/Templates.h"
#include "../Jeu/Jeu.h"
#include "ElementsCarte.h"


/** FONCTIONS DE LA CLASSE Joueur **/

Joueur::Joueur() : Individu_Unique()
{
	AjouterDansListeCollision = false;
}

int Joueur::TabToAct(int TabAppui)
{
	if (TabAppui == 0)		return PAUSE;
	if (TabAppui < 10000)	return COURSE;

	for (int c = 0 ; c < NOMBRE_COMPETENCES ; ++c)
		if (TabAppui/10000 == c+1 && skillLinks[c] != NULL) return getIntFromLUA(skillLinks[c], "getActivite");

	return PAUSE;
}

void Joueur::Repos()
{
	Set_Vitalite(1000);
	Set_Energie(1000);
	DureeEveil = 0;
}

void Joueur::Gestion_Equipement()
{
	//Diminue la durée de vie des objets utilisés
	for (mapObjects::iterator i = Get_Caracs()->objects.objects.begin() ; i != Get_Caracs()->objects.objects.end() ; ++i)
	{
		if (getStringFromLUA(i->second, "getIdEmplacement") == i->first && getDoubleFromLUA(i->second, "getDuree") > 0)
		{
			setDoubleToLUA(i->second, "setDuree", getDoubleFromLUA(i->second, "getDuree") - I(1));

			if (getDoubleFromLUA(i->second, "getDuree") <= 0)
			{
				lua_State *j = i->second;
				i = Get_Caracs()->objects.objects.erase(i);
				Get_Caracs()->objects.deleteObject(j);
				continue;
			}
			if (getStringFromLUA(i->second, "getCategorieObjet") == "amelioratif")
			{
				if (Get_Caracs()->objects.objects.find("regulier-" + getStringFromLUA(i->second, "getTypeObject")) == Get_Caracs()->objects.objects.end())
				{
					//L'équipement amélioré n'est plus présent ; on efface l'amélioration.
					lua_State *j = i->second;
					i = Get_Caracs()->objects.objects.erase(i);
					Get_Caracs()->objects.deleteObject(j);
					continue;
				}
			}
		}
	}
}

void Joueur::Gestion_Statistiques()
{
	//1. Régénération de la vitalité
	if (get("Vitalite") < 1000)
	{
		Lag_Vitalite(I(1/4.f*tan(get("Recuperation")/70.f)));
		if (Abs(get("Recuperation")) <= 95) Lag_Energie(-Abs(I(get("Recuperation")/25.f*(1000-get("Vitalite"))/1000.f)));
	}

	//2. Régénération ou Perte d'Énergie lors d'une Récupération forcée
	if (Abs(get("Recuperation")) > 95 && get("Energie") < ToSegment(get("Constitution"), 0, 100))
	{
		if (get("Recuperation") > 0) Lag_Energie(I(0.5));
		else Lag_Energie(-I(0.25));
	}

	//3. Perte d'énergie selon durée depuis repos
	if (DureeEveil > get("Constitution")) Lag_Energie(-I(DureeEveil-get("Constitution"))/10000);

	//4. Gain & Perte d'énergie par activité
	if (Get_Act() == PAUSE)
		Lag_Energie(I(get("Vitalite")/25000) + LieuVillage*I(get("Vitalite")/10000));
	if (Get_Act() == COURSE)
		Lag_Energie(-I(0.05/get("Constitution")));

	//5. Évolution du taux de récupération
	float RecupCible = get("RecuperationMoyenne") * ((get("Vitalite")-200)/740.f);
	if (RecupCible > 0) RecupCible *= Maximum(0, (get("Energie")-70.f)/1000.f);

	if (1.05*get("Recuperation") < RecupCible) Lag_Recuperation(I(0.1));
	if (0.95*get("Recuperation") > RecupCible) Lag_Recuperation(-I(0.1));

	if (Abs(get("RecuperationMoyenne")) >= 95) Set_Recuperation(get("RecuperationMoyenne"));

	//6. Durée d'éveil
	DureeEveil += I(0.0005);

	//7. Fatigue extrême
	if (get("Energie") < 70 && get("Energie") > 10)
	{
		//Agilite-
		if (Caracs["Agilite"] > 1 && !(rand()%(int)(get("Energie")*100)+1))
			--BufAgilite;

		//Intelligence-
		if (Caracs["Intelligence"] > 1 && !(rand()%(int)(get("Energie")*100)+1))
			--BufIntelligence;

		//Application
		if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_FATIGUE), Color(255, 128, 128, 255));
			Disp_Information(_FATIGUE, true);
	}
}

void Joueur::CoupCritique(Individu* ennemi)
{
	//Agilité+, selon Agilité de l'ennemi
	if (!(rand()%(int)(5 + 10 + ToSegment(get("Agilite") - ennemi->get("Agilite"), 0, 10) + 1)))
		BufAgilite += ToSegment(1.1*ennemi->get("Agilite")/get("Agilite"), 0, 10);

	//Charisme+, selon Charisme de l'ennemi
	if (!(rand()%(int)(5 + 10 + ToSegment(get("Charisme") - ennemi->get("Charisme"), 0, 10) + 1)))
		BufCharisme += ToSegment(1.1*ennemi->get("Charisme")/get("Charisme"), 0, 10);

	//Application
	if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_CRITIQUE), Color(128, 255, 128, 255));
		Disp_Information(_CRITIQUE, true);
}

void Joueur::BlessureGrave(Individu* ennemi)
{
	//Constitution-, selon Puissance de l'ennemi
	if (!(rand()%(int)(5 + 10 + ToSegment(get("Constitution") - ennemi->get("Puissance"), 0, 10) + 1)))
		BufConstitution -= Minimum(2, ToSegment(1.1*ennemi->get("Puissance")/get("Constitution"), 0, 10));

	//Charisme-, selon Charisme de l'ennemi
	if (!(rand()%(int)(5 + 10 + ToSegment(get("Charisme") - ennemi->get("Charisme"), 0, 10) + 1)))
		BufCharisme -= Minimum(2, ToSegment(1.1*ennemi->get("Charisme")/get("Charisme"), 0, 10));

	//Application
	if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_BLESSURE), Color(255, 128, 128, 255));
		Disp_Information(_BLESSURE, true);
}

void Joueur::CoupEsquive(Individu* ennemi)
{
	//Esquive+, selon Agilité de l'ennemi
	if (!(rand()%(int)(5 + 10 + ToSegment(get("Esquive") - ennemi->get("Agilite"), 0, 10) + 1)))
		BufEsquive += ToSegment(1.1*ennemi->get("Agilite")/get("Esquive"), 0, 3);

	//Application
	if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_ESQUIVE), Color(128, 255, 128, 255));
		Disp_Information(_ESQUIVE, true);
}

void Joueur::GainExperience(Individu* ennemi, float Degats, int Exp)
{
	//Gain d'expérience
	if (ennemi != NULL)
	{
		if (ennemi->get("Vitalite") > 0) Exp += Degats/5000.*ennemi->Get_Experience();
		else Exp += ennemi->Get_Experience();

		//Intelligence+, Force+, Puissance+, et Constitution+, si peu fatigué
		for (int i = 0 ; i < 1 + Exp/100 ; ++i)
		{
			if (!(rand()%(int)(50 + ToSegment(-get("Energie")/100, 0, 30) + 1)))
			{
				short carac = rand()%4;
				switch(carac)
				{
					case 0 : BufIntelligence += ToSegment(2*get("Intelligence"), 0, 100)/100; break;
					case 1 : BufForce += ToSegment(10*get("Intelligence"), 0, 100)/100; break;
					case 2 : BufPuissance += ToSegment(10*get("Intelligence"), 0, 100)/100; break;
					case 3 : BufConstitution += ToSegment(5*get("Intelligence"), 0, 100)/100; break;
				}
			}
		}

		//Application
		ApplicationAmeliorations();
	}

	Experience += Exp;
}

void Joueur::BlessuresMultiples(Individu* ennemi)
{
	//Force-, selon sa propre Constitution et Puissance de l'un des ennemis
	if (!(rand()%(int)(5 + 10 + ToSegment(get("Constitution") - ennemi->get("Puissance"), 0, 10) + 1)))
		BufForce -= Minimum(2, ToSegment(1.1*ennemi->get("Puissance")/get("Constitution"), 0, 10));

	//Puissance-, selon sa propre Constitution et Puissance de l'un des ennemis
	if (!(rand()%(int)(5 + 10 + ToSegment(get("Constitution") - ennemi->get("Puissance"), 0, 10) + 1)))
		BufPuissance -= Minimum(2, ToSegment(1.1*ennemi->get("Puissance")/get("Constitution"), 0, 10));

	//Esquive-, selon sa propre Constitution et Puissance de l'un des ennemis
	if (!(rand()%(int)(5 + 10 + ToSegment(get("Constitution") - ennemi->get("Puissance"), 0, 10) + 1)))
		BufEsquive -= Minimum(2, ToSegment(1.1*ennemi->get("Puissance")/get("Constitution"), 0, 10));

	//Application
	if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_BLESSURE), Color(255, 128, 128, 255));
		Disp_Information(_BLESSURE, true);
}

bool Joueur::ApplicationAmeliorations()
{
	//Affichage & enregistrement des améliorations proposées :

	String32 Final;
	Color Positif(128, 255, 128, 255); Color Negatif(255, 128, 128, 255);
	bool Retour = false;

	if ((int)BufForce != 0)
	{
		Final = Get_PhraseFormatee(_AMELIORATION_FORCE, (int)BufForce);
		(BufForce > 0) ? Ajouter_LigneAmelioration(Final, Positif) : Ajouter_LigneAmelioration(Final, Negatif);
		Caracs["Force"] += (int)BufForce; BufForce -= (int)BufForce; Retour = true;
	}
	if ((int)BufPuissance != 0)
	{
		Final = Get_PhraseFormatee(_AMELIORATION_PUISS, (int)BufPuissance);
		(BufPuissance > 0) ? Ajouter_LigneAmelioration(Final, Positif) : Ajouter_LigneAmelioration(Final, Negatif);
		Caracs["Puissance"] += (int)BufPuissance; BufPuissance -= (int)BufPuissance; Retour = true;
	}
	if ((int)BufAgilite != 0)
	{
		Final = Get_PhraseFormatee(_AMELIORATION_AGILITE, (int)BufAgilite);
		(BufAgilite > 0) ? Ajouter_LigneAmelioration(Final, Positif) : Ajouter_LigneAmelioration(Final, Negatif);
		Caracs["Agilite"] += (int)BufAgilite; BufAgilite -= (int)BufAgilite; Retour = true;
	}
	if ((int)BufIntelligence != 0)
	{
		Final = Get_PhraseFormatee(_AMELIORATION_INTELLI, (int)BufIntelligence);
		(BufIntelligence > 0) ? Ajouter_LigneAmelioration(Final, Positif) : Ajouter_LigneAmelioration(Final, Negatif);
		Caracs["Intelligence"] += (int)BufIntelligence; BufIntelligence -= (int)BufIntelligence; Retour = true;
	}
	if ((int)BufConstitution != 0)
	{
		Final = Get_PhraseFormatee(_AMELIORATION_CONSTIT, (int)BufConstitution);
		(BufConstitution > 0) ? Ajouter_LigneAmelioration(Final, Positif) : Ajouter_LigneAmelioration(Final, Negatif);
		Caracs["Constitution"] += (int)BufConstitution; BufConstitution -= (int)BufConstitution; Retour = true;
	}
	if ((int)BufEsquive != 0)
	{
		Final = Get_PhraseFormatee(_AMELIORATION_ESQUIVE, (int)BufEsquive);
		(BufEsquive > 0) ? Ajouter_LigneAmelioration(Final, Positif) : Ajouter_LigneAmelioration(Final, Negatif);
		Caracs["Esquive"] += (int)BufEsquive; BufEsquive -= (int)BufEsquive; Retour = true;
	}
	if ((int)BufCharisme != 0)
	{
		Final = Get_PhraseFormatee(_AMELIORATION_CHARISM, (int)BufCharisme);
		(BufCharisme > 0) ? Ajouter_LigneAmelioration(Final, Positif) : Ajouter_LigneAmelioration(Final, Negatif);
		Caracs["Charisme"] += (int)BufCharisme; BufCharisme -= (int)BufCharisme; Retour = true;
	}

	return Retour;
}

void Joueur::Disp(float RefX, float RefY)
{
	if (Controle == AI_IMG_HORSCHAMP) return;

	#ifdef DEBOGAGE
	if (Arguments.Masks)
		if (Abs(RefX - PosX) <= Options.ScreenW/2 && Abs(RefY - PosY) <= Options.ScreenH/2)
			Disp_Masks(RefX, RefY);
	#endif

	Disp_ImageIndividu(Type, Act, Dir, Num, Options.ScreenW/2 - (RefX - PosX), Options.ScreenH/2 + 12 - (RefY - PosY), true);
/*
	for (int i = 0 ; i < NOMBRE_EQUIPEMENTS ; ++i)
		Equips[i].Disp_Port(400 - (RefX - PosX), 310 - (RefY - PosY), Act, Dir, Num);*/
}

void Disp_Personnage()
{
	Disp_Texte(Partie.perso->Nom, 50, Options.ScreenH - 220, Color(255, 220, 220, 255), 35., Jeu.DayRoman);

	Disp_Texte(_PERSO_VITALITE, 50, Options.ScreenH - 170, Color(255, 255, 255, 255), 12.);
	Disp_Texte(intToString((int)Partie.perso->get("Vitalite")), 180, Options.ScreenH - 170, Color(255, 64, 64, 255), 12.);

	Disp_Texte(_PERSO_ENERGIE, 50, Options.ScreenH - 155, Color(255, 255, 255, 255), 12.);
	Disp_Texte(intToString((int)Partie.perso->get("Energie")), 180, Options.ScreenH - 155, Color(64, 160, 255, 255), 12.);

	Disp_Texte(_PERSO_RECUP, 50, Options.ScreenH - 140, Color(255, 255, 255, 255), 12.);
	if (Partie.perso->get("Recuperation") >= 0)
		Disp_Texte(intToString((int)Partie.perso->get("Recuperation")), 180, Options.ScreenH - 140, Color(64, 255, 64, 255), 12.);
	if (Partie.perso->get("Recuperation") < 0)
		Disp_Texte(intToString((int)Partie.perso->get("Recuperation")), 180, Options.ScreenH - 140, Color(255, 64, 255, 255), 12.);

	Disp_Texte(_PERSO_EXP, 50, Options.ScreenH - 125, Color(255, 255, 255, 255), 12.);
	Disp_Texte(intToString((int)Partie.perso->Experience), 180, Options.ScreenH - 125, Color(255, 255, 255, 255), 12.);

	//Affichage des dégâts de la compétence Arme :
	lua_State* weapon = NULL;
/*	for (int i = 0 ; i < NOMBRE_COMPETENCES ; ++i)
		if (Partie.perso->CompetencesRapides[i] != NULL && Partie.perso->CompetencesRapides[i]->Id/100 == TYPE_COMP_ARME)
 */ //			cpt_arme = dynamic_cast<Competence_Arme*>(Partie.perso->CompetencesRapides[/*i*/COMPETENCE_CTRL]);

	weapon = Partie.perso->skillLinks[COMPETENCE_CTRL];

	if (weapon != NULL)
	{
		Disp_Texte(_PERSO_DEGATS, 50, Options.ScreenH - 110, Color(255, 255, 255, 255), 12.);
		string StrNombre = intToString(getDoubleFromLUA(weapon, "getDegats") - getDoubleFromLUA(weapon, "getAmplitude") + Partie.perso->get("Force")/2)
						+ " - " + intToString(getDoubleFromLUA(weapon, "getDegats") + getDoubleFromLUA(weapon, "getAmplitude") + Partie.perso->get("Force")/2);
		Disp_Texte(StrNombre, 180, Options.ScreenH - 110, Color(255, 255, 255, 255), 12.);
	}

	for (int Carac = 0 ; Carac < 8 ; ++Carac)
	{
		enumPhrases Numero; int Nombre = 0; int Diff = 0;
		switch(Carac)
		{
			case 0 :	Numero = _PERSO_FORCE;	//Nombre = Partie.perso->Caracs.Force;
						//Diff = Partie.perso->Get_Force() - Partie.perso->Caracs.Force;
						break;
			case 1 :	Numero = _PERSO_PUISS;	//Nombre = Partie.perso->Caracs.Puissance;
						//Diff = Partie.perso->Get_Puissance() - Partie.perso->Caracs.Puissance;
						break;
			case 2 :	Numero = _PERSO_AGILITE;	//Nombre = Partie.perso->Caracs.Agilite;
						//Diff = Partie.perso->Get_Agilite() - Partie.perso->Caracs.Agilite;
						break;
			case 3 :	Numero = _PERSO_INTELLI;	//Nombre = Partie.perso->Caracs.Intelligence;
						//Diff = Partie.perso->Get_Intelligence() - Partie.perso->Caracs.Intelligence;
						break;
			case 4 :	Numero = _PERSO_CONSTIT;	//Nombre = Partie.perso->Caracs.Constitution;
						//Diff = Partie.perso->Get_Constitution() - Partie.perso->Caracs.Constitution;
						break;
			case 5 :	Numero = _PERSO_ESQUIVE;	//Nombre = Partie.perso->Caracs.Esquive;
						//Diff = Partie.perso->Get_Esquive() - Partie.perso->Caracs.Esquive;
						break;
			case 6 :	Numero = _PERSO_CHARISM;	//Nombre = Partie.perso->Caracs.Charisme;
						//Diff = Partie.perso->Get_Charisme() - Partie.perso->Caracs.Charisme;
						break;
			case 7 :	Numero = _PERSO_RECUPMOY;	//Nombre = Partie.perso->Caracs.RecuperationMoyenne;
						//Diff = Partie.perso->Get_RecuperationMoyenne() - Partie.perso->Caracs.RecuperationMoyenne;
						break;
		}

		Disp_Texte(Numero, 280, Options.ScreenH - 180 + 15*Carac, Color(255, 255, 255, 255), 12.);

		Disp_Texte(intToString(Nombre), 430, Options.ScreenH - 180 + 15*Carac, Color(255, 255, 255, 255), 12.);

		if (Diff > 0)
			Disp_Texte("+" + intToString(Diff), 460, Options.ScreenH - 180 + 15*Carac, Color(64, 255, 64, 255), 12.);
		if (Diff < 0)
			Disp_Texte(intToString(Diff), 460, Options.ScreenH - 180 + 15*Carac, Color(255, 64, 255, 255), 12.);
	}
}

void Disp_MiniaturesCompetences()
{
	Disp_ImageDecoration("FondMiniaturesCompetences", 5, Options.ScreenH - 171);

	unsigned y[] = {Options.ScreenH - 55, Options.ScreenH - 105, Options.ScreenH - 171, Options.ScreenH - 55};

	for (int i : {COMPETENCE_CTRL, COMPETENCE_SHIFT, COMPETENCE_TAB, COMPETENCE_SPACE})
		if (Partie.perso->skillLinks[i] != NULL)
		{
			lua_getglobal(Partie.perso->skillLinks[i], "dispImage");
			lua_pushnumber(Partie.perso->skillLinks[i], 5 + 66*(i/3));
			lua_pushnumber(Partie.perso->skillLinks[i], y[i]);
			lua_call(Partie.perso->skillLinks[i], 2, 0);
		}
}
