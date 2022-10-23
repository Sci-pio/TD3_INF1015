#pragma once
#include <string>
#include "Concepteur.hpp"
#include "Liste.hpp"

struct Jeu
{
	std::string titre;
	int anneeSortie;
	std::string developpeur;
	Liste<Concepteur> concepteurs;
	shared_ptr<Concepteur> chercherConcepteur(const string& nomConcepteur) {
		return (concepteurs.trouverSi([&nomConcepteur](shared_ptr<Concepteur> c) {return c->nom == nomConcepteur; }));
	}
};

