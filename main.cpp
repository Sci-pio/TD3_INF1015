#include <iostream>
#include <fstream>
#include <cstdint>
#include <cassert>
#include "cppitertools/range.hpp"
#include "gsl/span"
#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp"
#include "Jeu.hpp"
#include "Concepteur.hpp"
#include "Liste.hpp"
#include <string>
#include "debogage_memoire.hpp"  //NOTE: Incompatible avec le "placement new", ne pas utiliser cette entête si vous utilisez ce type de "new" dans les lignes qui suivent cette inclusion.

using namespace std;
using namespace iter;
using namespace gsl;

#pragma region "Fonctions de base pour vous aider"
template <typename T>
T lireType(istream& fichier)
{
	T valeur{};
	fichier.read(reinterpret_cast<char*>(&valeur), sizeof(valeur));
	return valeur;
}
#define erreurFataleAssert(message) assert(false&&(message)),terminate()
static const uint8_t enteteTailleVariableDeBase = 0xA0;
size_t lireUintTailleVariable(istream& fichier)
{
	uint8_t entete = lireType<uint8_t>(fichier);
	switch (entete) {
	case enteteTailleVariableDeBase + 0: return lireType<uint8_t>(fichier);
	case enteteTailleVariableDeBase + 1: return lireType<uint16_t>(fichier);
	case enteteTailleVariableDeBase + 2: return lireType<uint32_t>(fichier);
	default:
		erreurFataleAssert("Tentative de lire un entier de taille variable alors que le fichier contient autre chose à cet emplacement.");
	}
}

string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUintTailleVariable(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}
#pragma endregion

//Kamil: jsp jsuis vrm cave, quand je uncomment le morceau de code que j'ai fait l'execution cree juste le premier jeu dans la listeJeux et arrete la
//Retourne le shared_ptr<Concepteur> dans la listeJeux qui a le nom "nom". If none found, return nullptr.
//Jeu::chercherConcepteur(const string& nomConcepteur) retourne le shared_ptr<Concepteur> d'un Jeu qui le nom "nomConcepteur"
shared_ptr<Concepteur> trouverConcepteur(const Liste<Jeu>& listeJeux, const string& nom)
{
	//FAUDRAIT METTRE CA AVEC DES SPAN PT: EN CE MOMENT C'EST PAS TRÈS CLEAN (LEO)
	for (size_t i : range(listeJeux.getnElements())) {
		shared_ptr<Jeu> jeuPtr = listeJeux[i];

		for (size_t j : range(jeuPtr->concepteurs.getnElements())) {
			shared_ptr concepteurPtr = jeuPtr->concepteurs[j];

			if (concepteurPtr->nom == nom) {
				return concepteurPtr;
			}
		}
	}
	return nullptr;
	//shared_ptr<Concepteur> ptrConcepteur;
	//bool concepteurTrouve = false;
	//while (concepteurTrouve == false) {
	//	for (shared_ptr<Jeu> ptrJeu : listeJeux.enSpan()) {
	//		ptrConcepteur = ptrJeu->chercherConcepteur(nom);
	//		if (ptrConcepteur != nullptr) { concepteurTrouve = true; }
	//	}
	//}
	//return ptrConcepteur;
}


shared_ptr<Concepteur> lireConcepteur(istream& fichier, Liste<Jeu>& listeJeux)
{
	Concepteur concepteur = {}; 
	concepteur.nom = lireString(fichier);
	concepteur.anneeNaissance = int(lireUintTailleVariable(fichier));
	concepteur.pays = lireString(fichier);


	shared_ptr<Concepteur> concepteurPtr = trouverConcepteur(listeJeux, concepteur.nom);
	if (concepteurPtr == nullptr)
		concepteurPtr = make_shared<Concepteur>(concepteur);

	cout << "\033[92m" << "Allocation en mémoire du concepteur " << concepteur.nom
		<< "\033[0m" << endl;
	return concepteurPtr; 
}


shared_ptr<Jeu> lireJeu(istream& fichier, Liste<Jeu>& listeJeux)
{
	Jeu jeu = {};
	jeu.titre = lireString(fichier);
	jeu.anneeSortie = int(lireUintTailleVariable(fichier));
	jeu.developpeur = lireString(fichier);

	size_t nConcepteur = lireUintTailleVariable(fichier);


	shared_ptr ptrJeu = make_shared<Jeu>(jeu);
	cout << "\033[96m" << "Allocation en mémoire du jeu " << jeu.titre
		<< "\033[0m" << endl;

	for (size_t i : range(nConcepteur)) {
		shared_ptr<Concepteur> concep = lireConcepteur(fichier, listeJeux);
		ptrJeu->concepteurs.ajouter(concep);
	}

	return ptrJeu;
};

Liste<Jeu> creerListeJeux(const string& nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);
	size_t nElements = lireUintTailleVariable(fichier);
	Liste<Jeu> lj;

	for ([[maybe_unused]] size_t n : iter::range(nElements))
	{
		shared_ptr<Jeu> ptrJeu = lireJeu(fichier, lj);
		lj.ajouter(ptrJeu);
	}

	return lj;
}

ostream& operator<< (ostream& o, const shared_ptr<Concepteur>& ptrConcepteur) {
	cout << "\t" << ptrConcepteur->nom << ", " << ptrConcepteur->anneeNaissance << ", " << ptrConcepteur->pays << endl;
	return o;
}

ostream& operator<< (ostream& o, const shared_ptr<Jeu>& ptrJeu) {
	static const string ligneSeparation = "\n\033[12m--------------------------------------------------\033[0m\n";

	o << ptrJeu->titre << ", " << ptrJeu->developpeur << ", " << ptrJeu->anneeSortie << endl;
	o << "Concepteurs: \n";
	
	ptrJeu->concepteurs.afficher(o);

	o << ligneSeparation;
	return o;
}

template<typename T>
ostream& operator<< (ostream& o, const Liste<T>& l) {
	l.afficher(o);
	return o;
}

template<typename T>
span<shared_ptr<T>> enSpan(Liste<T>& l)
{
	return span(l.getElements());
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
#pragma region "Bibliothèque du cours"
	bibliotheque_cours::activerCouleursAnsi();
#pragma endregion
	static const string ligneSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";

	// CREATION LISTE<JEU> (#1, #2, #3):
	Liste<Jeu> lj = creerListeJeux("jeux.bin");
	cout << "nElements: " << lj.getnElements() << " , capacite: " << lj.getCapacite() << endl; 
	cout << ligneSeparation << endl;


	//TEST OPERATOR [] (#4):
	cout << "Titre du jeu a l'indice 2: " << lj[2]->titre << endl;
	cout << "Nom de son concepteur a l'indice 1: " << lj[2]->concepteurs[1]->nom << endl;
	cout << ligneSeparation << endl;


	//TEST FONCTION LAMBDA (#5):
	shared_ptr<Concepteur> a = lj[0]->chercherConcepteur("Yoshinori Kitase");
	shared_ptr<Concepteur> b = lj[1]->chercherConcepteur("Yoshinori Kitase");
	
	if (a.get() == b.get()) { cout << "a et b pointent vers la meme adresse: " << a.get() << " == " << b.get() << endl; }
	else { cout << "a et b ne pointent pas vers la meme adresse: " << a.get() << " != " << b.get() << endl; }
	cout << "Date de naissance de Yoshinori Kitase: " << a->anneeNaissance << endl;

	//TEST OPERATOR << (#6):
	cout << ligneSeparation << lj << ligneSeparation << endl; 
	ofstream("sortie.txt") << lj;
	cout << ligneSeparation << endl;


	//TEST DE LA COPIE (#7):
	cout << "Jeu original: \n\n" << lj[2] << endl;
	Jeu copieJeu = *lj[2];
	copieJeu.concepteurs[1] = lj[0]->concepteurs[1];
	cout << "Copie du jeu, avec concepteurs differents: \n\n" << make_shared<Jeu>(copieJeu) << endl;
	if (copieJeu.concepteurs[0].get() == lj[2]->concepteurs[0].get()) { cout << "L'adresse du premier concepteur dans les deux jeux est la meme : " << copieJeu.concepteurs[0].get() << " == " << lj[2]->concepteurs[0].get() << endl; }
	else { cout << "L'adresse du premier concepteur dans les deux jeux n'est pas la meme: " << copieJeu.concepteurs[0].get() << " != " << lj[2]->concepteurs[0].get() << endl; }
	cout << ligneSeparation << endl;

	
	//TODO: Faire les appels à toutes vos fonctions/méthodes pour voir qu'elles fonctionnent et avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.

}
