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
}


shared_ptr<Concepteur> lireConcepteur(istream& fichier, Liste<Jeu>& listeJeux)
{
	Concepteur concepteur = {}; // On initialise une structure vide de type Concepteur.
	concepteur.nom = lireString(fichier);
	concepteur.anneeNaissance = int(lireUintTailleVariable(fichier));
	concepteur.pays = lireString(fichier);


	shared_ptr<Concepteur> concepteurPtr = trouverConcepteur(listeJeux, concepteur.nom);
	if (concepteurPtr == nullptr)
		concepteurPtr = make_shared<Concepteur>(concepteur);

	cout << "\033[92m" << "Allocation en mémoire du concepteur " << concepteur.nom
		<< "\033[0m" << endl;
	return concepteurPtr; //TODO: Retourner le pointeur vers le concepteur crée.
}

//TODO: Fonction qui change la taille du tableau de jeux de ListeJeux.
// Cette fonction doit recevoir en paramètre la nouvelle capacité du nouveau
// tableau. Il faut allouer un nouveau tableau assez grand, copier ce qu'il y
// avait dans l'ancien, et éliminer l'ancien trop petit. N'oubliez pas, on copie
// des pointeurs de jeux. Il n'y a donc aucune nouvelle allocation de jeu ici !
//void changerTailleListeJeux(ListeJeux& liste, size_t nouvelleCapacite)
//{
//	assert(nouvelleCapacite >= liste.nElements); // On ne demande pas de supporter les réductions de nombre d'éléments.
//	Jeu** nouvelleListeJeux = new Jeu* [nouvelleCapacite];
//	// Pas nécessaire de tester si liste.elements est nullptr puisque si c'est le cas, nElements est nécessairement 0.
//	for (size_t i : iter::range(liste.nElements))
//		nouvelleListeJeux[i] = liste.elements[i];
//	delete[] liste.elements;
//
//	liste.elements = nouvelleListeJeux;
//	liste.capacite = nouvelleCapacite;
//}

//TODO: Fonction pour ajouter un Jeu à ListeJeux.
// Le jeu existant déjà en mémoire, on veut uniquement ajouter le pointeur vers
// le jeu existant. De plus, en cas de saturation du tableau elements, cette
// fonction doit doubler la taille du tableau elements de ListeJeux.
// Utilisez la fonction pour changer la taille du tableau écrite plus haut.
//void ajouterJeu(ListeJeux& liste, Jeu* jeu)
//{
//	if (liste.nElements == liste.capacite)
//		changerTailleListeJeux(liste, max(size_t(1), liste.capacite * 2));  // En C++23, on peut utiliser 1uz au lieu du cast.
//	liste.elements[liste.nElements++] = jeu;
//};

//TODO: Fonction qui enlève un jeu de ListeJeux.
// Attention, ici il n'a pas de désallocation de mémoire. Elle enlève le
// pointeur de la ListeJeux, mais le jeu pointé existe encore en mémoire.
// Puisque l'ordre de la ListeJeux n'a pas être conservé, on peut remplacer le
// jeu à être retiré par celui présent en fin de liste et décrémenter la taille
// de celle-ci.
//void enleverJeu(ListeJeux& liste, const Jeu* jeu)
//{
//	for (Jeu*& elem : spanListeJeux(liste)) {
//		if (elem == jeu) {
//			if (liste.nElements > 1)
//				elem = liste.elements[liste.nElements - 1];
//			liste.nElements--;
//		}
//	}
//};

shared_ptr<Jeu> lireJeu(istream& fichier, Liste<Jeu>& listeJeux)
{
	Jeu jeu = {};
	jeu.titre = lireString(fichier);
	jeu.anneeSortie = int(lireUintTailleVariable(fichier));
	jeu.developpeur = lireString(fichier);
	size_t nConcepteur = lireUintTailleVariable(fichier);
	// Rendu ici, les champs précédents de la structure jeu sont remplis avec la
	// bonne information.


	shared_ptr ptrJeu = make_shared<Jeu>();
	cout << "\033[96m" << "Allocation en mémoire du jeu " << jeu.titre
		<< "\033[0m" << endl;

	for (size_t i : range(nConcepteur)) {
		shared_ptr<Concepteur> concep = lireConcepteur(fichier, listeJeux);
		ptrJeu->concepteurs.ajouter(move(concep));
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
		lj.ajouter(move(ptrJeu));
	}

	return lj;
}
//TODO: Fonction pour détruire un concepteur (libération de mémoire allouée).
// Lorsqu'on détruit un concepteur, on affiche son nom pour fins de débogage.
//void detruireConcepteur(Concepteur* concepteur)
//{
//	cout << "\033[91m" << "Destruction du concepteur " << concepteur->nom << "\033[0m"
//			  << endl;
//	delete[] concepteur->jeuxConcus.elements;
//	delete concepteur;
//}

//TODO: Fonction qui détermine si un concepteur participe encore à un jeu.
//bool encorePresentDansUnJeu(const shared_ptr<Concepteur> concepteur)
//{
//	return concepteur->jeuxConcus.nElements != 0;
//}

//TODO: Fonction pour détruire un jeu (libération de mémoire allouée).
// Attention, ici il faut relâcher toute les cases mémoires occupées par un jeu.
// Par conséquent, il va falloir gérer le cas des concepteurs (un jeu contenant
// une ListeConcepteurs). On doit commencer par enlever le jeu à détruire des jeux
// qu'un concepteur a participé (jeuxConcus). Si le concepteur n'a plus de
// jeux présents dans sa liste de jeux participés, il faut le supprimer.  Pour
// fins de débogage, affichez le nom du jeu lors de sa destruction.
//void detruireJeu(Jeu* jeu)
//{
//	for (Concepteur* c : spanListeConcepteurs(jeu->concepteurs)) {
//		enleverJeu(c->jeuxConcus, jeu);
//		if (!encorePresentDansUnJeu(c))
//			detruireConcepteur(c);
//	}
//	cout << "\033[31m" << "Destruction du jeu " << jeu->titre << "\033[0m"
//			  << endl;
//	delete[] jeu->concepteurs.elements;
//	delete jeu;
//}

//TODO: Fonction pour détruire une ListeJeux et tous ses jeux.
//void detruireListeJeux(ListeJeux& liste)
//{
//	for(Jeu* j : spanListeJeux(liste))
//		detruireJeu(j);
//	delete[] liste.elements;
//}

//void afficherConcepteur(const Concepteur& d)
//{
//	cout << "\t" << d.nom << ", " << d.anneeNaissance << ", " << d.pays
//			  << endl;
//}

//TODO: Fonction pour afficher les infos d'un jeu ainsi que ses concepteurs.
// Servez-vous de la fonction afficherConcepteur ci-dessus.
//void afficherJeu(const Jeu& j)
//{
//	cout << "Titre : " << "\033[94m" << j.titre << "\033[0m" << endl;
//	cout << "Parution : " << "\033[94m" << j.anneeSortie << "\033[0m"
//			  << endl;
//	cout << "Développeur :  " << "\033[94m" << j.developpeur << "\033[0m"
//			  << endl;
//	cout << "Concepteurs du jeu :" << "\033[94m" << endl;
//	for(const Concepteur* c : spanListeConcepteurs(j.concepteurs))
//		afficherConcepteur(*c);
//	cout << "\033[0m";
//}

//TODO: Fonction pour afficher tous les jeux de ListeJeux, séparés par un ligne.
// Servez-vous de la fonction d'affichage d'un jeu crée ci-dessus. Votre ligne
// de séparation doit être différent de celle utilisée dans le main.
//void afficherListeJeux(const ListeJeux& listeJeux)
//{
//	static const string ligneSeparation = "\n\033[95m"
//	"══════════════════════════════════════════════════════════════════════════"
//	"\033[0m\n";
//	cout << ligneSeparation << endl;
//	for(const Jeu* j : spanListeJeux(listeJeux))
//	{
//		afficherJeu(*j);
//		cout << ligneSeparation << endl;
//	}
//}

template<typename T>
ostream& operator<< (ostream& o, shared_ptr< Jeu> ptrJeu) {
	cout << "here";
	return o << ptrJeu->titre << endl;
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
	cout << lj.getnElements();
	static const string ligneSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";
	cout << ligneSeparation << endl;
	cout << "Premier jeu de la liste :" << endl;
	//TODO: Afficher le premier jeu de la liste (en utilisant la fonction).  Devrait être Chrono Trigger.
	//afficherJeu(*lj.elements[0]);
	cout << ligneSeparation << endl;

	//TODO: Appel à votre fonction d'affichage de votre liste de jeux.
	//afficherListeJeux(lj);

	//TODO: Faire les appels à toutes vos fonctions/méthodes pour voir qu'elles fonctionnent et avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.



	//TODO: Détruire tout avant de terminer le programme.  Devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.
	//detruireListeJeux(lj);

	// Kamil: section de tests pour Liste.hpp
	Liste<int> listeInt;
	shared_ptr ptrInt = make_shared<int>(1);
	shared_ptr ptrInt2 = make_shared<int>(2);
	shared_ptr ptrInt3 = make_shared<int>(3);
	shared_ptr ptrInt4 = make_shared<int>(4);

	//Tests methode ajouter()
	listeInt.ajouter(ptrInt);
	listeInt.ajouter(ptrInt2);
	listeInt.ajouter(ptrInt3);
	listeInt.ajouter(ptrInt4);

	cout << *listeInt[0] << endl;
	cout << *listeInt[1] << endl;
	cout << *listeInt[2] << endl;
	cout << *listeInt[3] << endl;



	Liste<Jeu> listeJeu;
	shared_ptr ptrJeu = make_shared<Jeu>("Pierre");
	shared_ptr ptrJeu2 = make_shared<Jeu>("Luc");

	listeJeu.ajouter(ptrJeu);
	listeJeu.ajouter(ptrJeu2);

	Liste<Jeu> listeJeu2;
	listeJeu2 = move(listeJeu);


	cout << listeJeu2[0]->titre << endl;
	cout << listeJeu2[1]->titre << endl;

	//cout << listeInt; FONTIONNE PAS ENCORE
}
