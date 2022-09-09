#pragma once
#ifndef  MARKET_H
#define MARKET_H

#include "IO.h"
#include "Index.h"

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
class Market {
private:
	IO<TNumOfSections,TNumOfShelfsOnSection,TNumOfProductsOnShelf> mIOfs;
	Index mIndex;

public:
	Market();
	~Market();

	list* LookupForAProducts(char name[]);

	Product getProduct(structio_t whereIs);
	void changeProduct(structio_t whereIs);
	void insertProduct(Product toInsert);
	void removeProduct(structio_t whereIsToRemove);
};


#endif // ! MARKET_H
