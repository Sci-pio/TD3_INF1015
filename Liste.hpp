#include <iostream>
#include <fstream>
#include <cstdint>
#include <cassert>
#include "cppitertools/range.hpp"
#include "gsl/span"

using namespace std;
using namespace gsl;
using namespace iter;

template<typename T>
class Liste {
public:
	Liste() = default;
	~Liste();

	//void ajouter(T* element);


private:
	std::size_t nElements_ = 0, capacite_ = 0;
	shared_ptr<T> elements_ = nullptr;
};

//template<typename T>
//void Liste::ajouter(T* element)
//{
//
//}