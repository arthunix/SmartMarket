#pragma once
#ifndef IO_H
#define IO_H

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "Section.h"
#include "Product.h"
#include "File.h"

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
class IO {
private:
    std::string mFileName;
    std::fstream mIoFileStream;
    FileHeader mFileHeader;

    FileHeader ReadFileHeader();
    void WriteFileHeader(FileHeader fileheader);

    bool CheckForFileExistency();
public:
    IO(std::string filename);
    ~IO();

    void RemoveFromInventory();
    void InsertToInventory(Product product);
    void ModifyFromInventory();
    Product SeekOnInventory();

    void RemoveFromShelf();
    void InsertToShelf(Product product);
    void ModifyFromShelf();
    Product SeekOnShelf();

    void CleanFile();
    FileHeader getFileHeader() { return mFileHeader; };
};

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
inline IO<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::IO(std::string filename)
{
    /* I think I am creating a file for the first time?? This is an ethernal server
    but can we deal with energy issues but I will not deal with journaling here
    just I will verify if the file already exist */
    std::ifstream file(filename.c_str());
    if (file.good())
    {
        ReadFileHeader();
        file.close();
        mIoFileStream.open(filename.c_str());
        ReadFileHeader();
    }
    else
    {
        mIoFileStream.open(filename.c_str());
        std::cout << "We have no such file, writing one by the way" << std::endl;
        Section<TNumOfProductsOnShelf, TNumOfShelfsOnSection> writingsections;
        mIoFileStream.write((char*)&mFileHeader, sizeof(FileHeader));
        for (int i = 0; i < TNumOfSections; i++)
        {
            mIoFileStream.write((char*)&writingsections, sizeof(writingsections));
        }
        long long actual_byte_offset_for_end_of_maket = mIoFileStream.tellp();
        mFileHeader = {
            .NumberOfSections = TNumOfSections,
            .NumberOfShelfsOnSection = TNumOfShelfsOnSection,
            .NumberOfProductsOnShelf = TNumOfProductsOnShelf,
            .OffsetToShelfBlock = sizeof(mFileHeader),
            .OffsetToInventoryBlock = actual_byte_offset_for_end_of_maket
        };
        mIoFileStream.seekp(std::ios::beg);
        mIoFileStream.write((char*)&mFileHeader, sizeof(mFileHeader));
    }
}

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
inline IO<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::~IO()
{
    mIoFileStream.close();
}

#endif // !IO_H