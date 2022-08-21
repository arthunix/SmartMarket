#pragma once
#ifndef FILE_H
#define FILE_H

/* There is no bounding for this structure */
struct FileHeader {
    short int __SizeOfBlocks;
    long long int __OffsetToInventoryBlock;
    long long int __OffsetToShelfBlock;
    int __numberOfSections;
    int __numberOfShelfsOnSection;
    long int __SizeOfBuffer;
}fileHeader, FILEHEADER, fileheader;


#endif !FILE_H
