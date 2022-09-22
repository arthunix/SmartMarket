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

	char modName[NAME_SIZE] = "carro";
	char modDescription[DESCRIPTION_SIZE] = "boneca eletrico";
	char modBrand[BRAND_SIZE] = "tesla";

	Product new_prod(Name, Description, Brand,
		currTime, currTime, 12345, 100.00);

	Product new_mod(modName, modDescription, modBrand,
		currTime, currTime, 12345, 250.00);

	for (int k = 0; k < 5; k++)
	{
		io_system.InsertToShelf(new_prod);
	}

	for (int k = 0; k < 5; k++)
	{
		io_system.InsertToShelf(new_mod);
	}

	//io_system.printProducts();

	io_system.RemoveFromShelf(16056);

	io_system.ModifyFromShelf(16240, new_mod);

	io_system.printProducts();
	
	return 0;
}