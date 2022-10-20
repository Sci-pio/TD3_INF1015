#pragma once
#include <string>
#include "ListeConcepteurs.hpp"
//#include "Liste.hpp"

//consignes dit que ils veulent attributs publiques
struct Jeu
{
	std::string titre;
	int anneeSortie;
	std::string developpeur;
	Liste<Concepteur> concepteurs;
};
