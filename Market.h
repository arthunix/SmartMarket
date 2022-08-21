#pragma once
#ifndef  MARKET_H
#define MARKET_H

#include "IO.h"
#include "Index.h"

class Market {
private:
	// <num_sec, num_slf, num_prd>
	IO<50, 10, 200> iofs;
	Index index;
};


#endif // ! MARKET_H
