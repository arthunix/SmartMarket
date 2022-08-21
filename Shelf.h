#pragma once
#ifndef SHELF_H
#define SHELF_H

#include "Product.h"

struct product_t {
	bool is_deleted = true;
	Product product;
};

template<int num_of_products>
class Shelf {
	product_t arr_of_products[num_of_products];
};

#endif // !SHELF_H
