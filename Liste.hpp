// Kamil : ceci est un nouveau fichier, le code ici est entierement ecrit par moi avec des morceaux venant de la correction du TD2

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cassert>
#include "cppitertools/range.hpp"
#include "gsl/span"

using namespace std;
using namespace gsl;
using namespace iter;

//devrait avoir un unique_ptr elements_ rempli de shared_ptr qui sont des pointeurs vers des jeux
template<typename T>
class Liste {
public:
	Liste();
	//~Liste();

	void ajouter(shared_ptr<T> ptrT); 



private:
	std::size_t nElements_, capacite_;
	unique_ptr<T> elements_[nElements_];

	void changerCapacite(size_t nouvelleCapacite);
};


template<typename T>
Liste<T>::Liste()
{
	nElements_ = 0; capacite_ = 0; elements_ = {};
}

template<typename T>
void Liste<T>:: ajouter(shared_ptr<T> ptrT) {
	if (nElements_ == capacite_)
		changerCapacite(max(size_t(1), capacite_ * 2));
	elements_[nElements_++] = ptrT;

	// code temporaire de test:
	//for (size_t i : range(nElements_)) { cout << elements_[i]; }
	//cout << endl;
	//cout << capacite_ << endl;
	//cout << nElements_ << endl;
};


template<typename T>
void Liste<T>::changerCapacite(size_t nouvelleCapacite) {
	assert(nouvelleCapacite >= nElements_); 
	unique_ptr<T> nouvelleListe = shared_ptr<T>[nouvelleCapacite];

	for (size_t i : iter::range(nElements_))
		nouvelleListe[i] = elements_[i];

	elements_ = nouvelleListe;
	capacite_ = nouvelleCapacite;
}