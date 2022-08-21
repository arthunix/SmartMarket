#pragma once
#ifndef  MARKET_H
#define MARKET_H

#include "IO.h"
#include "Index.h"

template<int NumOfSections, int NumOfShelfsOnSection, int NumOfProductsOnShelf>
class Market {
private:
	IO<NumOfSections,NumOfShelfsOnSection,NumOfProductsOnShelf> m_iofs;
	Index m_index;
};


#endif // ! MARKET_H
