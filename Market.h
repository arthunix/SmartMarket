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
};


#endif // ! MARKET_H
