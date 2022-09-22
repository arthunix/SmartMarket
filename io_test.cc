#include <ctime>
#include <iostream>
#include <vector>

#include "IO.h"
#include "Product.h"

int main()
{
	IO<10, 10, 10> io_system;
	time_t currTime;
	time(&currTime);
	char Name[NAME_SIZE] = "boneca";
	char Description[DESCRIPTION_SIZE] = "boneca feia";
	char Brand[BRAND_SIZE] = "jhonson";

	Product new_prod(Name, Description, Brand,
		currTime, currTime, 12345, 100.00);

	for (int k = 0; k < 10; k++)
	{
		io_system.InsertToShelf(new_prod);
	}

	io_system.printProducts();
	
	return 0;
}