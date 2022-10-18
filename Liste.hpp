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
	Liste() = default;
	~Liste();

	void changerCapacite(size_t nouvelleCapacite) {
		assert(nouvelleCapacite >= nElements_);
		unique_ptr<T> nouvelleListe = shared_ptr<T>[nouvelleCapacite];

		for (size_t i : iter::range(nElements_))
			nouvelleListe[i] = elements_[i];

		elements_ = nouvelleListe;
		capacite_ = nouvelleCapacite;
	};

	void ajouter(shared_ptr<T> ptrT) {
		if (nElements_ == capacite_)
			changerCapacite(max(size_t(1), capacite_ * 2));
		elements_[nElements_++] = ptrT;
	};

	void retirer(const shared_ptr<T> aRetirer) {
		for (shared_ptr<T>& a : enSpan()) {
			if (a==aRetirer){
				if (nElements_ > 1)
					a = elements_[nElements_ - 1];
				nElements_--;
			}
		}
	};

	span<T*> enSpan() const { return span(elements_, nElements_); };

	shared_ptr<T> operator[] (const int index) const { return elements_[index]; }

private:
	std::size_t nElements_ = 0, capacite_ = 0;
	unique_ptr<shared_ptr<T>[]> elements_ = make_unique<shared_ptr<T>[]>(capacite_);
};



