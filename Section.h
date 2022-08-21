#pragma once
#ifndef SECTION_H
#define SECTION_H

#include "Shelf.h"

/*
I IMAGINE HERE WE HAVE: 5 shelfs (height) at each side of a hall then we have 10
*/
template<int num_of_products, int num_of_shelfs>
struct Section {
	Shelf<num_of_products> arr_of_shelf[num_of_shelfs];
};


#endif // !SECTION_H
