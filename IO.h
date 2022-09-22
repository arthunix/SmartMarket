#pragma once
#ifndef IO_H
#define IO_H

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
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
        std::streampos pos = mIoFileStream.tellg();
        mIoFileStream.seekg(std::ios::beg);
        mIoFileStream.read((char*)&mFileHeader, sizeof(FileHeader));
        mIoFileStream.seekg(pos);
    }

    void WriteFileHeader()
    {
        std::streampos pos = mIoFileStream.tellp();
        mIoFileStream.seekp(std::ios::beg);
        mIoFileStream.write((char*)&mFileHeader, sizeof(FileHeader));
        mIoFileStream.seekp(pos);
    }

    void ReadFileMtf()
    {
        std::streampos pos = mIoFileStream.tellg();
        mIoFileStream.seekg(mFileHeader.OffsetToMtf, std::ios::beg);
        mIoFileStream.read((char*)&mMtf, sizeof(mMtf));
        mIoFileStream.seekg(pos);
    }

    void WriteFileMtf()
    {
        std::streampos pos = mIoFileStream.tellp();
        mIoFileStream.seekp(mFileHeader.OffsetToMtf, std::ios::beg);
        mIoFileStream.write((char*)&mMtf, sizeof(mMtf));
        mIoFileStream.seekp(pos);
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
        unsigned short int idepth, ishelf, iproducts;
        product_controller_t loop_mtf;
        ReadFileHeader();
        ReadFileMtf();

        for (idepth = 0; idepth < TNumOfSections; idepth++)
        {
            for (ishelf = 0; ishelf < TNumOfShelfsOnSection; ishelf++)
            {
                for (iproducts = 0; iproducts < TNumOfProductsOnShelf; iproducts++)
                {
                    loop_mtf = mMtf[idepth][ishelf][iproducts];
                    if (loop_mtf.is_occuped == false) break;
                }
                if (loop_mtf.is_occuped == false) break;
            }
            if (loop_mtf.is_occuped == false) break;
        }

        product_storage_t new_inserted_product = {
            false, idepth, ishelf, product
        };

        mIoFileStream.seekp(0, std::ios::end);
        long long int ret = mIoFileStream.tellp();
#ifdef _DEBUG
        std::cout << "byte offset = " << ret << std::endl;
#endif // _DEBUG
        mIoFileStream.write((char*)&new_inserted_product, sizeof(new_inserted_product));

        product_controller_t correct_mtf = { true , false , ret };
        mMtf[idepth][ishelf][iproducts] = correct_mtf;
        mFileHeader.OffsetToInventoryBlockNextFree = mIoFileStream.tellp();

        WriteFileHeader();
        WriteFileMtf();

        mIoFileStream.clear();

        return ret;
    }

    long long int InsertToShelf(Product product, int iWantToPutInShelf, int iWantToPutInSection);

    void RemoveFromShelf();

    void ModifyFromShelf();

    Product SeekOnShelf();

    void CleanFile();

    FileHeader getFileHeader() { return mFileHeader; };

    void printProducts()
    {
        mIoFileStream.seekg(mFileHeader.OffsetToShelfBlock, std::ios::beg);
#ifdef _DEBUG
        std::cout << "byte offset = " << mIoFileStream.tellg();
#endif // _DEBUG
        product_storage_t lReadProductStructure;
        Product lReadProduct;
        lReadProduct = lReadProductStructure.product_itself;
        lReadProduct.printProduct();
        /*while (mIoFileStream.read((char*)&lReadProductStructure, sizeof(lReadProductStructure)));
        {
            lReadProduct = lReadProductStructure.product_itself;
            lReadProduct.printProduct();
        }*/
    }
};

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
inline IO<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::IO()
{
    /* I think I am creating a file for the first time?? This is an ethernal server
    but can we deal with energy issues but I will not deal with journaling here
    just I will verify if the file already exist */

    std::ifstream file(FILENAME);
    if (file)
    {
        ReadFileHeader();
        PrintFileHeader();
        file.close();
    }
    else
    {
#ifdef _DEBUG
        std::cout << "We have no such file, writing one by the way" << std::endl;
#endif // _DEBUG
        mIoFileStream.open(FILENAME, std::ios::out | std::ios::binary);
        mIoFileStream.close();

        mIoFileStream.open(FILENAME, std::ios::in | std::ios::out | std::ios::binary);
#ifdef _DEBUG
        if (!mIoFileStream) {
            std::cout << "Failed to open file for operations" << std::endl;
        }
#endif // _DEBUG
        mIoFileStream.seekp(std::ios::beg);
        long long int actual_byte_offset_for_fileheader = mIoFileStream.tellp();
        mIoFileStream.write((char*)&mFileHeader, sizeof(mFileHeader));
        long long int actual_byte_offset_for_mtf = mIoFileStream.tellp();
        mIoFileStream.write((char*)&mMtf, sizeof(mMtf));
        long long int actual_byte_offset_for_shelf = sizeof(mFileHeader) + sizeof(mMtf);

#ifdef _DEBUG
        std::cout << "sizeof(mFileHeader) = " << sizeof(mFileHeader) << std::endl;
        std::cout << "sizeof(mMtf) = " << sizeof(mMtf) << std::endl;
        std::cout << "actual_byte_offset_for_fileheader = " << actual_byte_offset_for_fileheader << std::endl;
        std::cout << "actual_byte_offset_for_mtf = " << actual_byte_offset_for_mtf << std::endl;
        std::cout << "actual_byte_offset_for_shelf = " << actual_byte_offset_for_shelf << std::endl;
#endif // _DEBUG
        
        mFileHeader = {
            actual_byte_offset_for_fileheader,
            actual_byte_offset_for_shelf,
            actual_byte_offset_for_shelf,
            actual_byte_offset_for_shelf,
            actual_byte_offset_for_shelf,
            TNumOfSections,
            TNumOfShelfsOnSection,
            TNumOfProductsOnShelf
        };
        std::streampos pos = mIoFileStream.tellp();
#ifdef _DEBUG
        std::cout << "byte offset on creation = " << (long long int)pos << std::endl;
#endif // _DEBUG
        mIoFileStream.seekp(std::ios::beg);
        mIoFileStream.write((char*)&mFileHeader, sizeof(FileHeader));
        mIoFileStream.seekp(pos);
#ifdef _DEBUG
        std::cout << "byte offset on creation = " << (long long int)pos << std::endl;
#endif // _DEBUG

#ifdef _DEBUG
        PrintFileHeader();
#endif // _DEBUG
    }
}

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
inline IO<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::~IO()
{
    mIoFileStream.close();
}

#endif // !IO_H