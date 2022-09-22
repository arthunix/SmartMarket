#pragma once
#ifndef IO_H
#define IO_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "Product.h"
#include "File.h"

#define FILENAME "SMARTMARKET"

struct product_storage_t {
    bool is_deleted = false;
    unsigned short int whereshelf;
    unsigned short int wheresection;
    Product product_itself;
};

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
class IO {
private:
    std::string mFileName;
    std::fstream mIoFileStream;
    FileHeader mFileHeader;
    product_controller_t mMtf[TNumOfSections][TNumOfShelfsOnSection][TNumOfProductsOnShelf];

    void ReadFileHeader()
    {
        mIoFileStream.seekg(std::ios::beg);
        mIoFileStream.read((char*)&mFileHeader, sizeof(FileHeader));
    }

    void WriteFileHeader()
    {
        mIoFileStream.seekp(std::ios::beg);
        mIoFileStream.write((char*)&mFileHeader, sizeof(FileHeader));
    }

    void ReadFileMtf()
    {
        mIoFileStream.seekg(mFileHeader.OffsetToMtf, std::ios::beg);
        mIoFileStream.read((char*)&mMtf, sizeof(mMtf));
    }

    void WriteFileMtf()
    {
        mIoFileStream.seekp(mFileHeader.OffsetToMtf, std::ios::beg);
        mIoFileStream.write((char*)&mMtf, sizeof(mMtf));
    }
public:
    IO();
    ~IO();

    void PrintFileHeader()
    {
        ReadFileHeader();
        mFileHeader.printFileHeader();
    }

    long long int InsertToShelf(Product product)
    {
        int idepth, ishelf, iproducts;
        ReadFileHeader();
        ReadFileMtf();

        for (idepth = 0; idepth < TNumOfSections; idepth++)
        {
            for (ishelf = 0; ishelf < TNumOfShelfsOnSection; ishelf++)
            {
                for (iproducts = 0; iproducts < TNumOfProductsOnShelf; iproducts++)
                {
                    product_controller_t loop_mtf = mMtf[idepth][ishelf][iproducts];
                    if (loop_mtf.is_occuped == false) break;
                }
            }
        }

        product_storage_t new_inserted_product = {
            false, idepth, ishelf, iproducts, product
        };

        mIoFileStream.seekp(mFileHeader.OffsetToShelfBlockNextFree, std::ios::beg);
        long long int ret = mIoFileStream.tellp();
        mIoFileStream.write((char*)&new_inserted_product, sizeof(product_storage_t));

        product_controller_t correct_mtf = { true , false , ret };
        mMtf[idepth][ishelf][iproducts] = correct_mtf;
        mFileHeader.OffsetToInventoryBlockNextFree = mIoFileStream.tellp();

        WriteFileHeader();
        WriteFileMtf();

        return ret;
    }

    long long int InsertToShelf(Product product, int iWantToPutInShelf, int iWantToPutInSection);

    void RemoveFromShelf();

    void ModifyFromShelf();

    Product SeekOnShelf();

    void CleanFile();

    FileHeader getFileHeader() { return mFileHeader; };
};

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
inline IO<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::IO()
{
    std::string filename = FILENAME;

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
        mIoFileStream.seekp(std::ios::beg);
        long long int actual_byte_offset_for_fileheader = mIoFileStream.tellp();
        mIoFileStream.write((char*)&mFileHeader, sizeof(FileHeader));
        long long int actual_byte_offset_for_mtf = mIoFileStream.tellp();
        mIoFileStream.write((char*)&mMtf, sizeof(mMtf));
        
        mFileHeader = {
            actual_byte_offset_for_fileheader,
            actual_byte_offset_for_mtf,
            actual_byte_offset_for_mtf,
            actual_byte_offset_for_mtf,
            actual_byte_offset_for_mtf,
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