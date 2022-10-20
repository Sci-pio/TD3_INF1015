#pragma once
#include <string>
#include "ListeJeux.hpp"
//#include "Liste.hpp"

//consignes dit que ils veulent attributs publiques
struct Concepteur
{
	std::string nom;
	int anneeNaissance;
	std::string pays;
	Liste<Jeu> jeuxConcus;
};
