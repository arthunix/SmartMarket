#pragma once
#ifndef  MARKET_H
#define MARKET_H

#include <cstring>
#include <list>
#include <vector>
#include <string>
#include "rbtree.h"
#include "Product.h"
#include "IO.h"
#include "Index.h"

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
class Market {
private:
	IO<TNumOfSections,TNumOfShelfsOnSection,TNumOfProductsOnShelf> mIOfs;
	Index mIndex;
	std::vector<rbnode*> mSearchCointainer;
public:
	Market() {};
	~Market() {};

	//void LookupForAProducts(const char* name, std::vector<const char*> &ret);

	Product getProduct(structio_t whereIs);
	void changeProduct(void* whereIsToChange, int index, Product productToChange);
	void insertProduct(Product toInsert);
	void removeProduct(void* whereIsToRemove, int index);

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

	lIOStructure.mOffset = mIOfs.InsertToShelf(toInsert);

	lIOStructure.mLote = toInsert.getLote();

	lIOStructure.mManufacturingDate = toInsert.getManufacturingDate();

	mIndex.InsertToIndex(lIOStructure);

#ifdef _DEBUG
	mIndex.PrintIndex();
	std::cout << "TRACE RING 3 : INSERT TO INDEX AND FILE - PUSHED " << std::endl;
#endif // _DEBUG
}

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
void Market<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::
removeProduct(void* whereIsToRemove, int index)
{
	rbnode* *str_io = (rbnode**)whereIsToRemove;
	rbnode& crr_str_io = *str_io[index];
	structio_t sio = *(structio_t*)crr_str_io.data;

	mIndex.RemoveFromIndex(str_io[index]);
	mIOfs.RemoveFromShelf(sio.mOffset);
}

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
void Market<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::
changeProduct(void* whereIsToChange, int index, Product productToChange)
{
	removeProduct(whereIsToChange, index);
	insertProduct(productToChange);
}

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
inline Product Market<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::getProduct(structio_t whereIs)
{
	mIOfs.SeekOnShelf(whereIs.mOffset);
}

#endif // ! MARKET_H
