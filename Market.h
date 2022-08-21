#pragma once
#ifndef  MARKET_H
#define MARKET_H

#include "IO.h"
#include "Index.h"

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
class Market {
private:
	IO<TNumOfSections,TNumOfShelfsOnSection,TNumOfProductsOnShelf> m_iofs;
	Index m_index;
};


#endif // ! MARKET_H
