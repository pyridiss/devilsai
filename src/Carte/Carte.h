
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

#ifndef header_carte
#define header_carte

#include <string>
#include <list>

#include <SFML/System.hpp>

using namespace std;

class lua_State;

class Classe_Commune;
class Classe_Paysage;
class Classe_Paysage_Mouvant;
class Element_Carte;
class Coffre;
class Cadavre;
class Element_Mouvant;
class Individu;
class Individu_Unique;
class Individu_Commun;
class Joueur;
class Paysage;
class Paysage_Mouvant;
class Projectile;
class Paysage_Lanceur;
class Actionneur;


class Carte
{
	//Eléments de la liste chainée :
	public:
		list <Element_Carte*> elements;

	//Objet :
	public:
		string Id = "";
		float PosFondX = 0, PosFondY = 0;

	public:
		bool AffichageFond = false;
		string FondCarte = "";

	//Constructeurs / Destructeurs :
	public:
		Carte();
		~Carte();

	//Gestion liste chainée :
	private:
		void AjouterElementEnListe(Element_Carte *elem);

	public:
		Individu_Unique* AjouterElement_Unique(string Type, string liste, int x, int y);
		Individu_Commun* AjouterElement_Commun(string Type, string liste, int x, int y);
		Joueur* AjouterJoueur(string Type, string liste, int x, int y);
		Paysage* AjouterPaysage(string Type, string liste, int x, int y);
		Paysage_Mouvant* AjouterPaysageMouvant(string Type, string liste, int x, int y);
		Paysage_Lanceur* AjouterPaysageLanceur(string Type, string liste, int x, int y);
		Projectile* AjouterProjectile(Projectile &prj);
		Actionneur* AjouterActionneur(string liste, int x, int y);
		Coffre* AjouterCoffre(string liste, int x, int y);
		Cadavre* AjouterCadavre(string liste, float x, float y);

		void AjouterListeEnCollision(string num);

		void SupprimerElement(Element_Carte* elem);

		void SupprimerListe(string num);

	//Gestion :
	public:
		int GestionElements();

	//Fond de la carte :
	public:
		void Set_FondCarte(string NumFond);
		void Disp_FondCarte();
};


void Load_Carte(string, bool);
Element_Carte* loadElementsFromStream(istream&, Carte*, string);
void ChangerCarte(Element_Carte*, string, string);
Element_Carte* Get_Element(int);
Individu_Unique* Get_IndividuUnique(string);

void CalculerRayonMax(int);
Element_Carte* Get_Current_Coll();
void RaZ_Coll();
bool Collision_cercle_cercle(int, int, int, int, int, int);
bool Collision_cercle_rectangle(int, int, int, int, int, int, int);
bool Collision_rectangle_rectangle(int, int, int, int, int, int, int, int);
int ParcoursCollisions(Individu*);
void TriCollision();

void Combat(Individu*, Individu*, lua_State* L = NULL);
void Blessure_Projectile(Projectile *Attaquant, Individu *Blesse);

void Lag_PosFondCartes(float, float);
void Set_PosCarte(float, float, bool);

void Ajouter_Carte(string);
void SupprimerListe_Carte();
Carte* Get_Carte(string);

void addCommonClass(string);
void deleteCommonClasses();
Classe_Commune* getCommonClass(string);

void addLandsClass(string);
void deleteLandsClasses();
Classe_Paysage* getLandsClass(string);

void addMovingLandsClass(string);
void deleteMovingLandsClasses();
Classe_Paysage_Mouvant* getMovingLandsClass(string);

void addCollider(Element_Carte*);
void removeCollider(Element_Carte*);
void removeColliders();

#endif
