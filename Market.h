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
	std::vector<rbnode*> mSearchCointainer;

public:
	Market() {};
	~Market() {};

	//void LookupForAProducts(const char* name, std::vector<const char*> &ret);

	Product getProduct(structio_t whereIs);
	void changeProduct(structio_t whereIs);
	void insertProduct(Product toInsert);
	void removeProduct(rbnode* whereIsToRemove);

	void SearchOn(const char* name)
	{
		mIndex.LookupAtIndexVec(name, mSearchCointainer);
	}

	std::vector<rbnode*> &LookupForAProducts(const char* name)
	{
		mSearchCointainer.clear();

		mIndex.LookupAtIndexVec(name, mSearchCointainer);

		return mSearchCointainer;
	}
};

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
void Market<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::
insertProduct(Product toInsert)
{
	structio_t lIOStructure;

	lIOStructure.mAmIonInventory = false;
	strcpy_s(lIOStructure.mName, NAME_SIZE, toInsert.getName());

	//long long int lOffset = InsertToInventory(toInsert);
	lIOStructure.mOffset = 25;

	lIOStructure.mLote = toInsert.getLote();

	lIOStructure.mManufacturingDate = toInsert.getManufacturingDate();

	mIndex.InsertToIndex(lIOStructure);

	mIndex.PrintIndex();

	std::cout << "TRACE RING 3 : INSERT TO INDEX AND FILE - PUSHED " << std::endl;
}

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
void Market<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::
removeProduct(rbnode* whereIsToRemove)
{
	mIndex.RemoveFromIndex(whereIsToRemove);
}

#endif // ! MARKET_H
