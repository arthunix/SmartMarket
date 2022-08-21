#pragma once
#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <cstdlib>

/* There is no bounding for this structure */
struct FileHeader {
    char MAGIC_SING[11] = ".AACMarket";
    long long int __OffsetToShelfBlock;
    long long int __OffsetToInventoryBlock;
    int __numberOfSections = 0;
    int __numberOfShelfsOnSection = 0;
    int __numberOfProductsOnShelf = 0;
};


#endif // !FILE_H
