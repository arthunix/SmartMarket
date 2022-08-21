#pragma once
#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <cstdlib>

/* There is no bounding for this structure */
struct FileHeader {
    char MAGIC_SING[11] = ".AACMarket";
    long long int OffsetToShelfBlock;
    long long int OffsetToInventoryBlock;
    int NumberOfSections = 0;
    int NumberOfShelfsOnSection = 0;
    int NumberOfProductsOnShelf = 0;
};


#endif // !FILE_H
