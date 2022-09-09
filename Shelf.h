#pragma once
#ifndef SHELF_H
#define SHELF_H

#include "Product.h"

struct product_controller_t {
	bool is_occuped = false;
	bool is_deleted = false;
};

template<int TNumOfProductsOnShelf>
class Shelf {
	product_controller_t arr_of_products[TNumOfProductsOnShelf];
};

#endif // !SHELF_H
