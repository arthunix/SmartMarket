#pragma once
#ifndef IO_H
#define IO_H

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "Section.h"
#include "Product.h"
#include "File.h"

template<int p_NumOfSections, int p_NumOfShelfsOnSection, int p_NumOfProductsOnShelf>
class IO {
private:
    std::string _fileName;
    std::fstream _ioFileStream;
    FileHeader _fileHeader;

    FileHeader ReadFileHeader();
    void WriteFileHeader(FileHeader write_header);

    bool checkForFileExistency();
public:
    IO(std::string p_Filename);
    ~IO();

    void RemoveFromInventory();
    void InsertToInventory(Product new_product);
    void ModifyFromInventory();
    Product SeekOnInventory();

    void RemoveFromShelf();
    void InsertToShelf(Product new_product);
    void ModifyFromShelf();
    Product SeekOnShelf();

    void CleanFile();
    FileHeader getFileHeader() { return _fileHeader; };
};

template<int p_NumOfSections, int p_NumOfShelfsOnSection, int p_NumOfProductsOnShelf>
inline IO<p_NumOfSections, p_NumOfShelfsOnSection, p_NumOfProductsOnShelf>::IO(std::string p_Filename)
{
    /* I think I am creating a file for the first time?? This is an ethernal server
    but can we deal with energy issues but I will not deal with journaling here
    just I will verify if the file already exist */
    std::ifstream file(p_Filename.c_str());
    if (file.good())
    {
        ReadFileHeader();
        file.close();
    }
    else
    {
        _ioFileStream.open(_fileName.c_str());
        std::cout << "We have no such file, writing one by the way" << std::endl;
        Section<p_NumOfProductsOnShelf, p_NumOfShelfsOnSection> writingsections;
        _ioFileStream.write((char*)&_fileHeader, sizeof(FileHeader));
        for (int i = 0; i < p_NumOfSections; i++)
        {
            _ioFileStream.write((char*)&writingsections, sizeof(writingsections));
        }
        long long actual_byte_offset_for_end_of_maket = _ioFileStream.tellp();
        _fileHeader = {
            .__numberOfSections = p_NumOfSections,
            .__numberOfShelfsOnSection = p_NumOfShelfsOnSection,
            .__numberOfProductsOnShelf = p_NumOfProductsOnShelf,
            .__OffsetToShelfBlock = sizeof(_fileHeader),
            .__OffsetToInventoryBlock = actual_byte_offset_for_end_of_maket
        };
        _ioFileStream.seekp(std::ios::beg);
        _ioFileStream.write((char*)&_fileHeader, sizeof(_fileHeader));
    }
}

template<int p_NumOfSections, int p_NumOfShelfsOnSection, int p_NumOfProductsOnShelf>
inline IO<p_NumOfSections, p_NumOfShelfsOnSection, p_NumOfProductsOnShelf>::~IO()
{
    _ioFileStream.close();
}

#endif // !IO_H