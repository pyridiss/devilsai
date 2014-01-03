
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

#include "../Bibliotheque/Templates.h"
#include "../Carte/Carte.h"
#include "../ElementsCarte/ElementsCarte.h"

Bouton *BoutonsInventaire[24];
Bouton *BoutonsCoffre[8];
Bouton *BoutonsCompetences[4]; //Never displayed, but useful to test activation

int PosDescX = 0, PosDescY = 0;
int PosCoffreX = 0, PosCoffreY = 0;

void Load_Decorations_Objets()
{
	for (int a = 0 ; a < 24 ; ++a)
	{
		BoutonsInventaire[a] = new Bouton;
		BoutonsInventaire[a]->Creer(150 + 5 + 50*(a%12), Options.ScreenH - 110 + 50*(a/12), 50, 50, "FondObjet_50_50", "FondObjet_50_50");
	}

	if (Options.ScreenW > 150 + 610 + 250) //Assez de place à droite de l'inventaire
	{
		PosCoffreX = 790; PosCoffreY = Options.ScreenH - 130;
		PosDescX = Options.ScreenW - 145; PosDescY = Options.ScreenH - 290;
	}
	else
	{
		PosCoffreX = 550; PosCoffreY = Options.ScreenH - 260;
		PosDescX = Options.ScreenW - 145; PosDescY = 100;
	}

	for (int a = 0 ; a < 8 ; ++a)
	{
		BoutonsCoffre[a] = new Bouton;
		BoutonsCoffre[a]->Creer(PosCoffreX + 5 + 50*(a%4), PosCoffreY + 20 + 50*(a/4), 50, 50, "FondObjet_50_50", "FondObjet_50_50");
	}

	unsigned yCompetences[] = {Options.ScreenH - 55, Options.ScreenH - 105, Options.ScreenH - 171, Options.ScreenH - 55};
	for (int a = 0 ; a < 4 ; ++a)
	{
		BoutonsCompetences[a] = new Bouton;
		BoutonsCompetences[a]->Creer(5 + 66*(a/3), yCompetences[a], 50, 50, "FondObjet_50_50", "FondObjet_50_50");
	}
}

void Supprimer_Decorations_Objets()
{
	for (int a = 0 ; a < 24 ; ++a)
		delete BoutonsInventaire[a];

	for (int a = 0 ; a < 8 ; ++a)
		delete BoutonsCoffre[a];

	for (int a = 0 ; a < 4 ; ++a)
		delete BoutonsCompetences[a];
}

void Disp_Skill(lua_State* skill)
{
	static int LigneCourante = PosDescY;

	string internalNumber = getStringFromLUA(skill, "getInternalNumber");

	String32 nom = Get_NomCompetence(internalNumber);
	Disp_TexteCentre(nom, PosDescX, LigneCourante, Color(255, 220, 220, 255), 20., Jeu.DayRoman);
}

void Disp_Competences()
{
	Disp_ImageDecoration("FondInventaire", 150, Options.ScreenH - 130);
	Disp_TexteCentre(_PERSONNAGE, 455, Options.ScreenH - 120, Color(255, 255, 255, 255), 13.);

	mapSkills::iterator skill = Partie.perso->Get_Caracs()->skills.begin();

	for (int cmpt = 0 ; cmpt < 24 ; ++cmpt)
	{
		BoutonsInventaire[cmpt]->Disp();
		if (skill != Partie.perso->Get_Caracs()->skills.end())
		{
			string internalNumber = getStringFromLUA(skill->second, "getInternalNumber");
			Disp_ImageCompetence(internalNumber, BoutonsInventaire[cmpt]->GetX(), BoutonsInventaire[cmpt]->GetY());

			if (BoutonsInventaire[cmpt]->TestSurvol())
				Disp_Skill(skill->second);

			++skill;
		}
	}

	if (Partie.selectedSkill != nullptr)
		Disp_ImageCompetence(getStringFromLUA(Partie.selectedSkill, "getInternalNumber"), Mouse::getPosition(Jeu.App).x, Mouse::getPosition(Jeu.App).y);
}

void Gestion_Competences(Event &event)
{
	mapSkills::iterator skill = Partie.perso->Get_Caracs()->skills.begin();

	for (int cmpt = 0 ; cmpt < 24 ; ++cmpt)
	{
		if (skill != Partie.perso->Get_Caracs()->skills.end())
		{
			if (BoutonsInventaire[cmpt]->TestActivation(event.type))
				Partie.selectedSkill = skill->second;

			++skill;
		}
	}

	if (Partie.selectedSkill != nullptr)
	{
		for (int i : {COMPETENCE_CTRL, COMPETENCE_SHIFT, COMPETENCE_TAB, COMPETENCE_SPACE})
		if (BoutonsCompetences[i]->TestActivation(event.type))
		{
			Partie.perso->skillLinks[i] = Partie.selectedSkill;
			//We must remove duplicates of this skill
			for (int j : {COMPETENCE_CTRL, COMPETENCE_SHIFT, COMPETENCE_TAB, COMPETENCE_SPACE})
			{
				if (i != j && Partie.perso->skillLinks[j] == Partie.selectedSkill)
					Partie.perso->skillLinks[j] = NULL;
			}
			Partie.selectedSkill = nullptr;
		}
	}
}

void Disp_EmplacementVide(string TypeObjet)
{
	Disp_TexteCentre(Get_PhraseFormatee(_EQUIP_VIDE, Get_NomObjet(TypeObjet)), PosDescX, PosDescY, Color(255, 220, 220, 255), 20., Jeu.DayRoman);
}

void Disp_Caracs_Objet(lua_State* obj, bool MaJ)
{
	static int LigneCourante = PosDescY;
	if (MaJ) LigneCourante = PosDescY;

	int internalNumber = getIntFromLUA(obj, "getInternalNumber");

	String32 nom = Get_NomObjet(internalNumber);
	Disp_TexteCentre(nom, PosDescX, LigneCourante, Color(255, 220, 220, 255), 20., Jeu.DayRoman);

	LigneCourante += 26;

	if (getBoolFromLUA(obj, "getDescriptionManuelle") && MaJ)
	{
		//Pour ne pas surcharger, on désactive les descriptions manuelles des objets supplémentaires
		Replique replique;
		replique.Chaine = Get_DescObjet(internalNumber);
		replique.Rectangle = IntRect(0, 0, 2*(Options.ScreenW - PosDescX) - 20, 0);
		DecoupageReplique(&replique);
		for (ListString32::iterator i = replique.Lignes.begin() ; i != replique.Lignes.end() ; ++i)
		{
			Disp_TexteCentre(*i, PosDescX, LigneCourante-4, Color(255, 220, 220), 12.);
			LigneCourante += 14;
		}
	}
	if (getDoubleFromLUA(obj, "getDuree") > 0)
	{
		Disp_TexteCentre(Get_PhraseFormatee(_EQUIP_DUREE, (int)(getDoubleFromLUA(obj, "getDuree")/60.f)), PosDescX, LigneCourante-4, Color(255, 220, 220), 12.);
		LigneCourante += 16;
	}

	if (getBoolFromLUA(obj, "getDescriptionAutomatique")) for (int Carac = 0 ; Carac < 19 ; ++Carac)
	{
		enumPhrases phrase; string functionLUA = "";

		switch(Carac)
		{
			case 0 :	functionLUA = "getAbsoluteForce";				phrase = _EQUIP_FORCE;			break;
			case 1 :	functionLUA = "getAbsoluteMultForce";			phrase = _EQUIP_MULT_FORCE;		break;
			case 2 :	functionLUA = "getAbsolutePuissance";			phrase = _EQUIP_PUISS;			break;
			case 3 :	functionLUA = "getAbsoluteMultPuissance";		phrase = _EQUIP_MULT_PUISS;		break;
			case 4 :	functionLUA = "getAbsoluteAgilite";				phrase = _EQUIP_AGILITE;		break;
			case 5 :	functionLUA = "getAbsoluteMultAgilite";			phrase = _EQUIP_MULT_AGILITE;	break;
			case 6 :	functionLUA = "getAbsoluteIntelligence";		phrase = _EQUIP_INTELLI;		break;
			case 7 :	functionLUA = "getAbsoluteMultIntelligence";	phrase = _EQUIP_MULT_INTELLI;	break;
			case 8 :	functionLUA = "getAbsoluteConstitution";		phrase = _EQUIP_CONSTIT;		break;
			case 9 :	functionLUA = "getAbsoluteMultConstitution";	phrase = _EQUIP_MULT_CONSTIT;	break;
			case 10 :	functionLUA = "getAbsoluteEsquive";				phrase = _EQUIP_ESQUIVE;		break;
			case 11 :	functionLUA = "getAbsoluteMultEsquive";			phrase = _EQUIP_MULT_ESQUIVE;	break;
			case 12 :	functionLUA = "getAbsoluteCharisme";			phrase = _EQUIP_CHARISM;		break;
			case 13 :	functionLUA = "getAbsoluteMultCharisme";		phrase = _EQUIP_MULT_CHARISM;	break;
			case 14 :	functionLUA = "getAbsoluteRecuperation";		phrase = _EQUIP_RECUP;			break;
			case 15 :	functionLUA = "getAbsoluteMultRecuperation";	phrase = _EQUIP_MULT_RECUP;		break;
			case 16 :	functionLUA = "getAbsoluteVitesseCourse";		phrase = _EQUIP_VIT_COURSE;		break;
			case 17 :	functionLUA = "getAbsoluteVitesseAttaque";		phrase = _EQUIP_VIT_ATTAQUE;	break;
			case 18 :	functionLUA = "getAbsoluteVitesseBlesse";		phrase = _EQUIP_VIT_BLESSE;		break;
		}

		int value = getIntFromLUA(obj, functionLUA);
		if (value != 0)
		{
			Disp_TexteCentre(Get_PhraseFormatee(phrase, value), PosDescX, LigneCourante, Color(255, 255, 255), 11.);
			LigneCourante += 14;
		}
	}
	LigneCourante += 26;
}

void Disp_Equipement()
{
	//Just a pointer to simplify source code
	mapObjects* objects = &(Partie.perso->Get_Caracs()->objects.objects);

	//Affichage des emplacements et des objets s'y trouvant
	for (auto& emp : Partie.perso->EmplacementsEquip)
	{
		emp.BoutonEquipement->Disp();
		mapObjects::iterator obj = objects->find(emp.Get_IdEmplacement());

		if (obj != objects->end())
		{
			int internalNumber = getIntFromLUA(obj->second, "getInternalNumber");
			Disp_ImageObjet(internalNumber, OBJET_IMG, emp.BoutonEquipement->GetX(), emp.BoutonEquipement->GetY());
			mapObjects::iterator equip2 = objects->find("amelioratif-" + emp.TypeObjet);
			if (equip2 != objects->end())
			{
				int internalNumber2 = getIntFromLUA(equip2->second, "getInternalNumber");
				Disp_ImageObjet(internalNumber2, OBJET_IMG, emp.BoutonEquipement->GetX(), emp.BoutonEquipement->GetY());
			}
		}
	}

	//Si un des objets est survolé par la souris, affichage de ses caractéristiques
	for (auto& emp : Partie.perso->EmplacementsEquip)
	{
		if (emp.BoutonEquipement->TestSurvol())
		{
			mapObjects::iterator equip = objects->find(emp.Get_IdEmplacement());
			if (equip == objects->end()) Disp_EmplacementVide(emp.Get_IdEmplacement());
			else
			{
				Disp_Caracs_Objet(equip->second, true);
				mapObjects::iterator equip2 = objects->find("amelioratif-" + emp.TypeObjet);
				if (equip2 != objects->end()) Disp_Caracs_Objet(equip2->second, false);
			}
			break;
		}
	}

	//Inventaire :
	Disp_ImageDecoration("FondInventaire", 150, Options.ScreenH - 130);
	Disp_TexteCentre(_INVENTAIRE, 455, Options.ScreenH - 120, Color(255, 255, 255, 255), 13.);

	mapObjects::iterator inv;

	for (int cmpt = 0 ; cmpt < 24 ; ++cmpt)
	{
		BoutonsInventaire[cmpt]->Disp();
		inv = objects->find(intToString(CLEF_INVENTAIRE + cmpt));
		if (inv != objects->end())
		{
			int internalNumber = getIntFromLUA(inv->second, "getInternalNumber");
			Disp_ImageObjet(internalNumber, OBJET_MINIATURE, BoutonsInventaire[cmpt]->GetX(), BoutonsInventaire[cmpt]->GetY());
			if (getBoolFromLUA(inv->second, "getCumul"))
			{
				string StrNombre = intToString(getIntFromLUA(inv->second, "getQuantite"));
				Disp_Texte(StrNombre, BoutonsInventaire[cmpt]->GetX() + 30, BoutonsInventaire[cmpt]->GetY() + 30, Color(255,255,255), 12.);
			}
		}
	}

	for (int a = 0 ; a < 24 ; ++a)
	{
		if (BoutonsInventaire[a]->TestSurvol())
		{
			inv = objects->find(intToString(CLEF_INVENTAIRE + a));
			if (inv != objects->end()) Disp_Caracs_Objet(inv->second, true);
		}
	}

	//Affichage de l'équipement sélectionné :
	if (Partie.selectedObject != nullptr)
		Disp_ImageObjet(getIntFromLUA(Partie.selectedObject, "getInternalNumber"), OBJET_IMG, Mouse::getPosition(Jeu.App).x, Mouse::getPosition(Jeu.App).y, true);
}

bool Disp_Coffre()
{
	if (Get_Element(Partie.perso->ElementInteraction) == NULL) Partie.CoffreOuvert = NULL;

	if (Partie.CoffreOuvert == NULL) return false;

	Disp_ImageDecoration("FondCoffre", PosCoffreX, PosCoffreY);
	Disp_TexteCentre(Partie.CoffreOuvert->Nom, PosCoffreX + 105, PosCoffreY + 10, Color(255, 255, 255, 255), 13.);

	mapObjects::iterator i;

	for (int cmpt = 0 ; cmpt < 8 ; ++cmpt)
	{
		BoutonsCoffre[cmpt]->Disp();
		i = Partie.CoffreOuvert->objects.objects.find(intToString(CLEF_COFFRE + cmpt));
		if (i != Partie.CoffreOuvert->objects.objects.end())
		{
			Disp_ImageObjet(getIntFromLUA(i->second, "getInternalNumber"), OBJET_MINIATURE, BoutonsCoffre[cmpt]->GetX(), BoutonsCoffre[cmpt]->GetY());
			if (getBoolFromLUA(i->second, "getCumul"))
			{
				string StrNombre = intToString(getIntFromLUA(i->second, "getQuantite"));
				Disp_Texte(StrNombre, BoutonsCoffre[cmpt]->GetX() + 30, BoutonsCoffre[cmpt]->GetY() + 30, Color(255,255,255), 12.);
			}
		}
	}

	for (int a = 0 ; a < 8 ; ++a)
	{
		if (BoutonsCoffre[a]->TestSurvol())
		{
			i = Partie.CoffreOuvert->objects.objects.find(intToString(CLEF_COFFRE + a));
			if (i != Partie.CoffreOuvert->objects.objects.end()) Disp_Caracs_Objet(i->second, true);
		}
	}

	return true;
}

void Gestion_Coffre(Event &event)
{
	//Just a pointer to simplify source code
	mapObjects* objects = &(Partie.perso->Get_Caracs()->objects.objects);

	//Contrôle des clics :
	bool ClicGauche = !(event.mouseButton.button == Mouse::Right);

	//1. Équipement du Joueur
	mapObjects::iterator currentObject = objects->end(), sourceObject = objects->end(), tmpObject = objects->end();
	ListEmplacementsEquipements::iterator emp;
	bool EquipClic = false;

	for (emp = Partie.perso->EmplacementsEquip.begin() ; emp != Partie.perso->EmplacementsEquip.end() ; ++emp)
	{
		if (emp->BoutonEquipement->TestActivation(event.type))
		{
			currentObject = objects->find(emp->Get_IdEmplacement());
			EquipClic = true;
			break;
		}
	}

	if (Partie.selectedObject == nullptr)
	{
		if (currentObject != objects->end()) //We pick currentObject
		{
			Partie.selectedObject = currentObject->second;
			setStringToLUA(currentObject->second, "setKey", "0");
			objects->erase(currentObject);
		}
	}
	else if (EquipClic) //We try to put selectedObject in our equipment
	{
		if (emp->TypeObjet == getStringFromLUA(Partie.selectedObject, "getTypeObject") &&
			((currentObject == objects->end() && getStringFromLUA(Partie.selectedObject, "getCategorieObjet") != "amelioratif") ||
			 (currentObject != objects->end() && getStringFromLUA(Partie.selectedObject, "getCategorieObjet") == "amelioratif") ))
		{
			pair<mapObjects::iterator, bool> result;

			if (getBoolFromLUA(Partie.selectedObject, "getCumul") && getIntFromLUA(Partie.selectedObject, "getQuantite") > 1)
			{
				//We need to load a new object
				result = Partie.perso->Get_Caracs()->objects.addObject(getStringFromLUA(Partie.selectedObject, "getFileName"), getStringFromLUA(Partie.selectedObject, "getIdEmplacement"));
				if (result.second)
				{
					setStringToLUA(result.first->second, "setKey", getStringFromLUA(result.first->second, "getIdEmplacement"));
					setIntToLUA(Partie.selectedObject, "setQuantite", getIntFromLUA(Partie.selectedObject, "getQuantite") - 1);
				}
			}
			else
			{
				//We can just move the object
				result = objects->insert(mapObjects::value_type(getStringFromLUA(Partie.selectedObject, "getIdEmplacement"), Partie.selectedObject));
				if (result.second)
				{
					setStringToLUA(result.first->second, "setKey", getStringFromLUA(result.first->second, "getIdEmplacement"));
					Partie.selectedObject = nullptr;
				}
			}
		}
	}

	//2. Inventaire du Joueur

	for (int Case = 0 ; Case < 24 ; ++Case)
	{
		if (!BoutonsInventaire[Case]->TestActivation(event.type)) continue;

		sourceObject = objects->find(intToString(CLEF_INVENTAIRE + Case));

		if (Partie.selectedObject == nullptr) //We pick (left click) or use (right click) sourceObject
		{
			if (sourceObject != objects->end())
			{
				if (ClicGauche) //Sélection d'un objet dans l'inventaire
				{
					Partie.selectedObject = sourceObject->second;
					setStringToLUA(sourceObject->second, "setKey", "0");
					objects->erase(sourceObject);
				}
				else //Utilisation d'un objet se trouvant dans l'inventaire
				{
					if (getStringFromLUA(sourceObject->second, "getCategorieObjet") != "charme") //Charmes must stay in the inventory
					{
						currentObject = objects->find(getStringFromLUA(sourceObject->second, "getIdEmplacement"));

						if (currentObject != objects->end() &&
							(getStringFromLUA(currentObject->second, "getCategorieObjet") == "temporaire" ||
							getStringFromLUA(currentObject->second, "getCategorieObjet") == "amelioratif" ))
						{
							Partie.perso->Get_Caracs()->objects.deleteObject(currentObject->second);
							objects->erase(currentObject);
							sourceObject = objects->find(intToString(CLEF_INVENTAIRE + Case));
						}

						pair<mapObjects::iterator, bool> result;

						if (getBoolFromLUA(sourceObject->second, "getCumul") && getIntFromLUA(sourceObject->second, "getQuantite") > 1)
						{
							//We need to load a new object
							result = Partie.perso->Get_Caracs()->objects.addObject(getStringFromLUA(sourceObject->second, "getFileName"), getStringFromLUA(sourceObject->second, "getIdEmplacement"));
							sourceObject = objects->find(intToString(CLEF_INVENTAIRE + Case));
							if (result.second)
							{
								setStringToLUA(result.first->second, "setKey", getStringFromLUA(sourceObject->second, "getIdEmplacement"));
								setIntToLUA(sourceObject->second, "setQuantite", getIntFromLUA(sourceObject->second, "getQuantite") - 1);
							}
						}
						else
						{
							//We can just move the object
							result = objects->insert(mapObjects::value_type(getStringFromLUA(sourceObject->second, "getIdEmplacement"), sourceObject->second));
							if (result.second)
							{
								setStringToLUA(result.first->second, "setKey", getStringFromLUA(result.first->second, "getIdEmplacement"));
								objects->erase(sourceObject);
							}
						}
					}
				}
			}
		}
		else // We put selectedObject in our inventory
		{
			bool Cumuler = false;
			if (sourceObject == objects->end()) //Pas d'objet ; on pose celui porté
			{
				if (getBoolFromLUA(Partie.selectedObject, "getCumul")) //Avant, on vérifie qu'il n'y a pas déjà un objet identique cumulable
				{
					string key = intToString(CLEF_INVENTAIRE);
					for (tmpObject = objects->begin() ; tmpObject != objects->end() ; ++tmpObject)
						if (tmpObject->first[0] == key[0] && getIntFromLUA(tmpObject->second, "getInternalNumber") == getIntFromLUA(Partie.selectedObject, "getInternalNumber"))
						{
							Cumuler = true;
							sourceObject = tmpObject;
							break;
						}
				}
			}
			else if (getBoolFromLUA(sourceObject->second, "getCumul") && getIntFromLUA(sourceObject->second, "getInternalNumber") == getIntFromLUA(Partie.selectedObject, "getInternalNumber"))
			{
				Cumuler = true;
			}

			if (Cumuler)
			{
				setIntToLUA(sourceObject->second, "setQuantite", getIntFromLUA(sourceObject->second, "getQuantite") + getIntFromLUA(Partie.selectedObject, "getQuantite"));
				Partie.selectedObject = nullptr;
			}
			else if (sourceObject == objects->end())
			{
				string clef = intToString(CLEF_INVENTAIRE + Case);
				objects->insert(mapObjects::value_type(clef, Partie.selectedObject));
				sourceObject = objects->find(clef);
				setStringToLUA(sourceObject->second, "setKey", clef);
				Partie.selectedObject = nullptr;
			}
		}
	}

	//3. Coffre (si ouvert)

	if (Partie.CoffreOuvert == NULL) return;

	for (int Case = 0 ; Case < 8 ; ++Case)
	{
		if (BoutonsCoffre[Case]->TestActivation(event.type) && ClicGauche)
		{
			mapObjects::iterator i = Partie.CoffreOuvert->objects.objects.find(intToString(CLEF_COFFRE + Case));

			if (Partie.selectedObject == nullptr)
			{
				if (i != Partie.CoffreOuvert->objects.objects.end())
				{
					Partie.selectedObject = i->second;
					setStringToLUA(Partie.selectedObject, "setKey", "0");
					Partie.CoffreOuvert->objects.objects.erase(i);
				}
			}
			else
			{
				if (i == Partie.CoffreOuvert->objects.objects.end())
				{
					string clef = intToString(CLEF_COFFRE + Case);
					Partie.CoffreOuvert->objects.objects.insert(mapObjects::value_type(clef, Partie.selectedObject));
					i = Partie.CoffreOuvert->objects.objects.find(clef);
					setStringToLUA(i->second, "setKey", clef);
					Partie.selectedObject = nullptr;
				}
				else if (getBoolFromLUA(i->second, "getCumul") && getIntFromLUA(i->second, "getInternalNumber") == getIntFromLUA(Partie.selectedObject, "getInternalNumber"))
				{
					//Il s'agit du même objet, qui peut être cumulé
					setIntToLUA(i->second, "setQuantite", getIntFromLUA(i->second, "getQuantite") + getIntFromLUA(Partie.selectedObject, "getQuantite"));
					Partie.selectedObject = nullptr;
				}
			}
		}
	}
}
