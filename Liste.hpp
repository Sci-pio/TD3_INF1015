

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cassert>
#include "cppitertools/range.hpp"
#include "gsl/span"
#include <functional>

using namespace std;
using namespace gsl;
using namespace iter;



#pragma once
template<typename T> 
class Liste {
public:

	Liste() :
		nElements_(0),
		capacite_(0),
		elements_(make_unique<shared_ptr<T>[]>(capacite_))
	{}

	Liste(Liste<T>&& autre) noexcept
	{
		*this = move(autre);
	}

	~Liste() = default;

	void changerCapacite(const size_t nouvelleCapacite)
	{
		assert(nouvelleCapacite >= nElements_); //Est ce que la bonne prog. ? Pcq si la condition est respecter �a termine le programme... (LEO)
		auto nouvelleListe = make_unique<shared_ptr<T>[]>(nouvelleCapacite);

		for (size_t i : range(nElements_))
			nouvelleListe[i] = elements_[i];

		elements_ = move(nouvelleListe);
		capacite_ = nouvelleCapacite;
	}

	void ajouter(const shared_ptr<T> ptrT)
	{
		if (nElements_ == capacite_)
			changerCapacite(max(size_t(1), capacite_ * 2));
		elements_[nElements_++] = ptrT;
	}

	/*void retirer(const shared_ptr<T> aRetirer)
	{
		for (shared_ptr<T> a : enSpan()) {
			if (a == aRetirer) {
				if (nElements_ > 1)
					a = elements_[nElements_ - 1];
				nElements_--;
			}
		}
	}*/

	span<shared_ptr<T>> enSpan() const { return gsl::span(elements_, nElements_); };

	shared_ptr<T> operator[] (const int index) const { return elements_[index]; }

	template<typename U>
	friend ostream& operator<< (ostream& o, const Liste<U>& l);

	Liste& operator= (Liste&& autre) noexcept
	{
		elements_ = move(autre.elements_); capacite_ = autre.capacite_; nElements_ = autre.nElements_;
		autre.elements_ = nullptr;
		return *this;
	}

	size_t const getnElements() const { return nElements_; }

	unique_ptr<shared_ptr<T>[]> const getElements() const{ return elements_; }

	size_t const getCapacite() const { return capacite_; }

	//Kamil : fonction qui retourne l'indexe du premier element dans une Liste qui satisfait a un critere, ca marche pas
	//template <typename PredicatUnaire>
	//int trouverSi(const PredicatUnaire& critere) {
	//	int i = 0;
	//	for (auto&& v : enSpan()) {
	//		if (critere(v))
	//			return i;
	//		i++;
	//	}
	//	return -1;
	//}

private:
	std::size_t nElements_, capacite_;
	unique_ptr<shared_ptr<T>[]> elements_;
};

template<typename T>
ostream& operator<< (ostream& o, const Liste<T>& l) {

	for (size_t i : range(l.getnElements()))
		o << l[i];

	return o;
}