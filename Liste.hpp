// Auteurs: Leonard Pouliot (2150965) et Kamil Maarite (2152653)
// Date: 25 octobre 2022
// cours: INF1015
// Nom de la classe: Liste.hpp

#pragma once
#include <iostream>
#include <fstream>
#include <cstdint>
#include <cassert>
#include "cppitertools/range.hpp"
#include "gsl/span"
#include <functional>
#include <string>

using namespace std;
using namespace gsl;
using namespace iter;


template<typename T>
class Liste {
public:
	Liste() :
		nElements_(0),
		capacite_(0),
		elements_(make_unique<shared_ptr<T>[]>(capacite_))
	{};

	Liste(Liste<T>& autre)
	{
		*this = autre;
	}

	~Liste() = default;

	void changerCapacite(const size_t nouvelleCapacite);

	void ajouter(const shared_ptr<T> ptrT);

	span<shared_ptr<T>> enSpan() const { return { elements_.get(), nElements_ }; };

	void afficher(ostream& o) const
	{
		for (size_t i : range(nElements_)) { o << elements_[i]; }
	}

	shared_ptr<T>& operator[] (const int index) const { return elements_[index]; }

	shared_ptr<T>& operator[] (const size_t index) const { return elements_[int(index)]; } // Pour eliminer des warnings de conversions de size_t a int

	Liste& operator= (const Liste<T>& autre)
	{
		if (this != &autre) {
			nElements_ = autre.nElements_; capacite_ = autre.capacite_;
			elements_ = make_unique<shared_ptr<T>[]>(capacite_);

			for (size_t i : range(nElements_)) {
				elements_[i] = autre[i];
			}
		}
		return *this;
	}

	size_t const getnElements() const { return nElements_; }

	size_t const getCapacite() const { return capacite_; }

	template <typename PredicatUnaire>
	shared_ptr<T> trouverSi(const PredicatUnaire& critere); 

private:
	std::size_t nElements_, capacite_;
	unique_ptr<shared_ptr<T>[]> elements_;
};


template<typename T>
void  Liste<T>::changerCapacite(const size_t nouvelleCapacite)
{
	assert(nouvelleCapacite >= nElements_); 
	unique_ptr<shared_ptr<T>[]> nouvelleListe = make_unique<shared_ptr<T>[]>(nouvelleCapacite);

	for (size_t i : range(nElements_))
		nouvelleListe[i] = elements_[i];

	elements_ = move(nouvelleListe);
	capacite_ = nouvelleCapacite;
}


template<typename T>
void Liste<T>::ajouter(const shared_ptr<T> ptrT)
{
	if (nElements_ == capacite_)
		changerCapacite(max(size_t(1), capacite_ * 2));
	elements_[nElements_++] = ptrT;
}


template <typename T>
template <typename PredicatUnaire>
shared_ptr<T> Liste<T>::trouverSi(const PredicatUnaire& critere) {
	int i = 0;
	for (auto&& v : enSpan()) {
		if (critere(v))
			return v;
		i++;
	}
	return nullptr;
}