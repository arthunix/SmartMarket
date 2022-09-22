#pragma once
#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <cstdlib>

struct product_controller_t {
    bool is_occuped = false;
    bool is_deleted = false;
    long long int offset = 0;
};

/* There is no bounding for this structure */
struct FileHeader {
    //char MAGIC_SING[11] = ".AACMarket";
    long long int OffsetToMtf = 0;
    long long int OffsetToShelfBlock = 0;
    long long int OffsetToShelfBlockNextFree = 0;
    long long int OffsetToInventoryBlock = 0;
    long long int OffsetToInventoryBlockNextFree = 0;
    int NumberOfSections = 0;
    int NumberOfShelfsOnSection = 0;
    int NumberOfProductsOnShelf = 0;

    void printFileHeader() {
        std::cout << "OffsetToMtf = " << OffsetToMtf << std::endl;
        std::cout << "OffsetToShelfBlock = " << OffsetToShelfBlock << std::endl;
        //std::cout << "OffsetToInventoryBlock = " << OffsetToInventoryBlock << std::endl;
        std::cout << "NumberOfSections = " << NumberOfSections << std::endl;
        std::cout << "NumberOfShelfsOnSection = " << NumberOfShelfsOnSection << std::endl;
        std::cout << "NumberOfProductsOnShelf = " << NumberOfProductsOnShelf << std::endl;
    }
};

#endif // !FILE_H
