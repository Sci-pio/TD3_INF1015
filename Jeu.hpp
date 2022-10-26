// Auteurs: Leonard Pouliot (2150965) et Kamil Maarite (2152653)
// Date: 25 octobre 2022
// cours: INF1015
// Nom de la classe: Jeu.hpp

#pragma once
#include <string>
#include "Concepteur.hpp"
#include "Liste.hpp"

struct Jeu
{
	string titre;
	int anneeSortie;
	string developpeur;
	Liste<Concepteur> concepteurs;
	shared_ptr<Concepteur> chercherConcepteur(const string& nomConcepteur) {
		return (concepteurs.trouverSi([&nomConcepteur](shared_ptr<Concepteur> c) {return c->nom == nomConcepteur; }));
	}
};

