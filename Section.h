#pragma once
#ifndef SECTION_H
#define SECTION_H

#include "Shelf.h"

/*
I IMAGINE HERE WE HAVE: 5 shelfs (height) at each side of a hall then we have 10
*/
template<int NumOfProductsOnShelf, int NumOfShelfsOnSection>
struct Section {
	Shelf<NumOfProductsOnShelf> arr_of_shelf[NumOfShelfsOnSection];
};


#endif // !SECTION_H
