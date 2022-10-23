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
//gsl::span<Jeu*> spanListeJeux(const Liste<Jeu>& liste)
//{
//	return gsl::span(liste.getElements().get(), liste.getnElements());
//}
//gsl::span<Concepteur*> spanListeConcepteurs(const ListeConcepteurs& liste)
//{
//	return gsl::span(liste.elements, liste.nElements);
//}
#pragma endregion


shared_ptr<Concepteur> trouverConcepteur(const Liste<Jeu>& listeJeux, string nom)
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
	
	// on veut retourner le shared_ptr de Concepteur qui le nom "nom" dans une listeJeux
	// Jeu::chercherConcepteur() retourne le concepteur qui la le nom "nom" dans un Jeu
	//shared_ptr<Concepteur> ptrConcepteur;
	//for (shared_ptr<Jeu> ptrJeu : listeJeux.enSpan()) {
	//	if (ptrJeu->chercherConcepteur(nom) != nullptr) {
	//		ptrConcepteur = ptrJeu->chercherConcepteur(nom);
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
	// Rendu ici, les champs précédents de la structure jeu sont remplis avec la
	// bonne information.


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
	// Permet sous Windows les "ANSI escape code" pour changer de couleur
	// https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac
	// les supportent normalement par défaut.
	bibliotheque_cours::activerCouleursAnsi();
#pragma endregion

	//int* fuite = new int;  // Pour vérifier que la détection de fuites fonctionne; un message devrait dire qu'il y a une fuite à cette ligne.

	Liste<Jeu> lj = creerListeJeux("jeux.bin"); //TODO: Appeler correctement votre fonction de création de la liste de jeux.
	cout << "nElements: " << lj.getnElements() << " , capacite: " << lj.getCapacite() << endl; 
	static const string ligneSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";

	//Test operator<<
	cout << ligneSeparation << lj << ligneSeparation << endl; 
	ofstream("sortie.txt") << lj;
	
	/*cout << "Premier jeu de la liste :" << endl;*/
	//TODO: Afficher le premier jeu de la liste (en utilisant la fonction).  Devrait être Chrono Trigger.
	//afficherJeu(*lj.elements[0]);
	/*cout << ligneSeparation << endl;*/

	//TODO: Appel à votre fonction d'affichage de votre liste de jeux.
	//afficherListeJeux(lj);

	//TODO: Faire les appels à toutes vos fonctions/méthodes pour voir qu'elles fonctionnent et avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.



	//TODO: Détruire tout avant de terminer le programme.  Devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.
	//detruireListeJeux(lj);

	// Kamil: section de tests pour Liste.hpp
	Liste<int> listeInt;
	shared_ptr ptrInt = make_shared<int>(1);
	shared_ptr ptrInt2 = make_shared<int>(3);
	shared_ptr ptrInt3 = make_shared<int>(5);
	shared_ptr ptrInt4 = make_shared<int>(8);

	//Tests methode ajouter()
	listeInt.ajouter(ptrInt);
	listeInt.ajouter(ptrInt2);
	listeInt.ajouter(ptrInt3);
	listeInt.ajouter(ptrInt4);

	Liste<Jeu> listeJeu;
	shared_ptr ptrJeu = make_shared<Jeu>("Pierre");
	shared_ptr ptrJeu2 = make_shared<Jeu>("Luc");


	listeJeu.ajouter(ptrJeu);
	listeJeu.ajouter(ptrJeu2);

	

	//Tests operator[]
	cout << "Titre du jeu a l'indice 2: " << lj[2]->titre << endl; 
	cout << "Nom de son concepteur a l'indice 1: " << lj[2]->concepteurs[1]->nom << endl;
	cout << ligneSeparation << endl;
		 
	//Tests methode trouverSi()
	shared_ptr<int> ptrInt5 = listeInt.trouverSi([&](shared_ptr<int> v) {return *v > 6; });
	cout << *ptrInt5 << endl; // affiche 5
	cout << ptrInt5.use_count() << endl;

	Liste<int> ListeInt2;
	ListeInt2.ajouter(ptrInt5);
	cout << ptrInt5.use_count() << endl;

	//Tests methode Jeu::chercherConcepteur()
	shared_ptr<Concepteur> a = lj[0]->chercherConcepteur("Yoshinori Kitase");
	shared_ptr<Concepteur> b = lj[1]->chercherConcepteur("Yoshinori Kitase");
	if (a == b) { cout << "a et b pointent vers la meme adresse" << endl; } // Kamil: je crois que c'est une bonne maniere de verifier si deux shared_ptr pointent au meme endroit, mais jsuis pas sur
	else { cout << "a et b ne pointent pas vers la meme adresse" << endl; }
	cout << "Date de naissance de Yoshinori Kitase: "<<a->anneeNaissance << endl;

	cout << ligneSeparation << endl; 

	//Test de la copie
	//Jeu copieJeu = *lj[2];
	//copieJeu.concepteurs[1] = (*lj[0]).concepteurs[3]; //devrait etre remplace par Hironobu Sakaguchi
	//cout << lj[2] << endl;
	//cout << make_shared<Jeu>(copieJeu) << endl;


	
}
