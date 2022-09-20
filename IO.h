#pragma once
#ifndef IO_H
#define IO_H

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "Section.h"
#include "Product.h"
#include "File.h"

#define FILENAME "SMARTMARKET"

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
class IO {
private:
    std::string mFileName;
    std::fstream mIoFileStream;
    FileHeader mFileHeader;
    product_controller_t mMtf[TNumOfSections][TNumOfShelfsOnSection][TNumOfProductsOnShelf];

    void ReadFileHeader() {
        mIoFileStream.seekg(std::ios::beg);
        mIoFileStream.read((char*)&mFileHeader, sizeof(FileHeader));
    }

    void WriteFileHeader() {
        mIoFileStream.seekp(std::ios::beg);
        mIoFileStream.write((char*)&mFileHeader, sizeof(FileHeader));
    }

public:
    IO();
    ~IO();

    void PrintFileHeader() {
        ReadFileHeader();
        mFileHeader.printFileHeader();
    }

    void RemoveFromInventory();

    long long int InsertToInventory(Product product) {
        ReadFileHeader();
        mIoFileStream.seekp(mFileHeader.OffsetToShelfBlock, std::ios::beg);
        long long int ret = mIoFileStream.tellp();
        mIoFileStream.write((char*)&product, sizeof(Product));

        mFileHeader.

        return ret;
    }

    void ModifyFromInventory();
    Product SeekOnInventory();

    void RemoveFromShelf();
    long long int InsertToShelf(Product product);
    void ModifyFromShelf();
    Product SeekOnShelf();

    void CleanFile();
    FileHeader getFileHeader() { return mFileHeader; };
};

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
inline IO<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::IO()
{
    std::string filename = "SMARTMARKET";

    /* I think I am creating a file for the first time?? This is an ethernal server
    but can we deal with energy issues but I will not deal with journaling here
    just I will verify if the file already exist */

    std::ifstream file(filename.c_str());
    if (file)
    {
        ReadFileHeader();
        PrintFileHeader();
        file.close();
    }
    else
    {
        std::cout << "We have no such file, writing one by the way" << std::endl;
        mIoFileStream.open(filename.c_str(), std::ios::out);
        mIoFileStream.close();

        mIoFileStream.open(filename.c_str(), std::ios::in | std::ios::out);
        if (!mIoFileStream) {
            std::cout << "Failed to open file for operations" << std::endl;
        }
        
        Section<TNumOfProductsOnShelf, TNumOfShelfsOnSection> writingsections;
        mIoFileStream.seekp(std::ios::beg);
        mIoFileStream.write((char*)&mFileHeader, sizeof(FileHeader));
        mIoFileStream.write((char*)&mMtf, sizeof(writingsections));
        long long int actual_byte_offset_for_end_of_maket = mIoFileStream.tellp();
        mFileHeader = {
            sizeof(FileHeader),
            actual_byte_offset_for_end_of_maket,
            actual_byte_offset_for_end_of_maket,
            TNumOfSections,
            TNumOfShelfsOnSection,
            TNumOfProductsOnShelf
        };
        mIoFileStream.seekp(std::ios::beg);
        mIoFileStream.write((char*)&mFileHeader, sizeof(FileHeader));
        PrintFileHeader();
    }
}

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
inline IO<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::~IO()
{
    mIoFileStream.close();
}

#endif // !IO_H