#pragma once
#ifndef  MARKET_H
#define MARKET_H

#include <cstring>
#include <list>
#include <vector>
#include <string>

#include "IO.h"
#include "Index.h"

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
class Market {
private:
	//IO<TNumOfSections,TNumOfShelfsOnSection,TNumOfProductsOnShelf> mIOfs;
	Index mIndex;

public:
	Market() {};
	~Market() {};

	std::vector<const char*> LookupForAProducts(const char* name);

	Product getProduct(structio_t whereIs);
	void changeProduct(structio_t whereIs);
	void insertProduct(Product toInsert);
	void removeProduct(structio_t whereIsToRemove);
};

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
inline void Market<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::insertProduct(Product toInsert)
{
	//long long int lOffset = InsertToInventory(toInsert);

	structio_t lIOStructure;

	lIOStructure.mAmIonInventory = false;
	strcpy_s(lIOStructure.mName, NAME_SIZE, toInsert.getName());
	lIOStructure.mOffset = 25;

	mIndex.InsertToIndex(lIOStructure);

	mIndex.PrintIndex();

	std::cout << "TRACE RING 3 : INSERT TO INDEX AND FILE - PUSHED " << std::endl;
}

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
std::vector<const char*> 
Market<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>
::LookupForAProducts(const char* name)
{
	std::vector<const char*> ret = mIndex.LookupAtIndexVec( name );

	return ret;
}


#endif // ! MARKET_H
