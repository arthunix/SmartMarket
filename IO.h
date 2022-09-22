#pragma once
#ifndef IO_H
#define IO_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <io.h>
#include "Product.h"
#include "File.h"

#define FILENAME "SMARTMARKET"

#define log_file_operations_error(error) {                                  \
if (err != 0)                                                               \
    std::cout << "Fail in file operations -> ";                             \
    std::cout << "error code = " << error << std::endl;                     \
}

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
    FILE* mIoFileStream;
    FileHeader mFileHeader;
    product_controller_t mMtf[TNumOfSections][TNumOfShelfsOnSection][TNumOfProductsOnShelf];
    errno_t err;

    int gNum = 0;

    void ReadFileHeader()
    {
        long long int pos = ftell(mIoFileStream);
        err = fseek(mIoFileStream, 0, SEEK_SET);
        log_file_operations_error(err);
        err = fread((char*)&mFileHeader, sizeof(FileHeader), 1, mIoFileStream);
        log_file_operations_error(err);
        err = fseek(mIoFileStream, pos, SEEK_SET);
        log_file_operations_error(err);
    }

    void WriteFileHeader()
    {
        long long int pos = ftell(mIoFileStream);
        err = fseek(mIoFileStream, 0, SEEK_SET);
        log_file_operations_error(err);
        err = fwrite((char*)&mFileHeader, sizeof(FileHeader), 1, mIoFileStream);
        log_file_operations_error(err);
        err = fseek(mIoFileStream, pos, SEEK_SET);
        log_file_operations_error(err);
    }

    void ReadFileMtf()
    {
        long long int pos = ftell(mIoFileStream);
        err = fseek(mIoFileStream, mFileHeader.OffsetToMtf, SEEK_SET);
        log_file_operations_error(err);
        err = fread((char*)&mMtf, sizeof(mMtf), 1, mIoFileStream);
        log_file_operations_error(err);
        err = fseek(mIoFileStream, pos, SEEK_SET);
        log_file_operations_error(err);
    }

    void WriteFileMtf()
    {
        long long int pos = ftell(mIoFileStream);
        err = fseek(mIoFileStream, mFileHeader.OffsetToMtf, SEEK_SET);
        log_file_operations_error(err);
        err = fwrite((char*)&mMtf, sizeof(mMtf), 1, mIoFileStream);
        log_file_operations_error(err);
        err = fseek(mIoFileStream, pos, SEEK_SET);
        log_file_operations_error(err);
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
        long long int ret = -1; product_controller_t loop_mtf;
        unsigned short int idepth, ishelf, iproducts;
        ReadFileHeader(); ReadFileMtf();

        gNum += 1;

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

        product_storage_t new_inserted_product = { false, idepth, ishelf, product };

        ret = ftell(mIoFileStream);
#ifdef _DEBUG
        std::cout << "byte I will write the product = " << ret << std::endl;
#endif // _DEBUG

        err = fwrite((char*)&new_inserted_product, sizeof(product_storage_t), 1, mIoFileStream);

        log_file_operations_error(err);
        product_controller_t correct_mtf = { true , false , ret };
        mMtf[idepth][ishelf][iproducts] = correct_mtf;

        mFileHeader.OffsetToShelfBlockNextFree = ftell(mIoFileStream);

        WriteFileHeader(); WriteFileMtf();
        return ret;
    }

    void RemoveFromShelf(long byteoffset_to_product)
    {
        long whereWasI = ftell(mIoFileStream);
        product_storage_t lReadProductStructure;

        err = fseek(mIoFileStream, byteoffset_to_product, SEEK_SET);
        log_file_operations_error(err);
        fread((char*)&lReadProductStructure, sizeof(product_storage_t), 1, mIoFileStream);

        lReadProductStructure.is_deleted = true;

        err = fseek(mIoFileStream, byteoffset_to_product, SEEK_SET);
        log_file_operations_error(err);
        err = fwrite((char*)&lReadProductStructure, sizeof(product_storage_t), 1, mIoFileStream);
        log_file_operations_error(err);

        err = fseek(mIoFileStream, whereWasI, SEEK_SET);
        log_file_operations_error(err);
    }

    long long int ModifyFromShelf(long byteoffset_to_product, Product modified_product)
    {
        RemoveFromShelf(byteoffset_to_product);
        return InsertToShelf(modified_product);
    }

    Product SeekOnShelf();

    void CleanFile();

    FileHeader getFileHeader() { return mFileHeader; };

#ifdef _DEBUG
    void printProducts()
    {
        std::cout << " \n\n--- PRINTING PRODUCTS --- " << gNum << std::endl;
        ReadFileHeader();
        err = fseek(mIoFileStream, mFileHeader.OffsetToShelfBlock, SEEK_SET);
        log_file_operations_error(err);
        long tell = ftell(mIoFileStream);
        std::cout << "fteel in print products = " << tell << std::endl;
        product_storage_t lReadProductStructure;
        Product lReadProduct;

        for (int k = 0; k < gNum; k++)
        {
            std::cout << "val = " << k;
            long tell = ftell(mIoFileStream);
            std::cout << " fteel in print products = " << tell << std::endl;
            fread((char*)&lReadProductStructure, sizeof(product_storage_t), 1, mIoFileStream);
            lReadProduct = lReadProductStructure.product_itself;
            std::cout << "IS PRODUCT DELETED ??? = " << lReadProductStructure.is_deleted << std::endl;
            lReadProduct.printProduct();
            std::cout << "\n";
        }    
        std::cout << " --- END OF PRINTING PRODUCTS ---\n\n " << std::endl;
    }
#endif // _DEBUG
};

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
inline IO<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::IO()
{
    /* I think I am creating a file for the first time?? This is an ethernal server
    but can we deal with energy issues but I will not deal with journaling here
    just I will verify if the file already exist */
    if ((err = _access_s(FILENAME, 0)) == 0)
    {

    }
    else
    {
        std::cout << "We have no such file, writing one by the way" << std::endl;
        err = fopen_s(&mIoFileStream, FILENAME, "w+");
        log_file_operations_error(err);
        err = fseek(mIoFileStream, 0, SEEK_SET);
        log_file_operations_error(err);
        long long int actual_byte_offset_for_fileheader = ftell(mIoFileStream);
        err = fwrite((char*)&mFileHeader, sizeof(mFileHeader), 1, mIoFileStream);
        log_file_operations_error(err);
        long long int actual_byte_offset_for_mtf = ftell(mIoFileStream);
        err = fwrite((char*)&mMtf, sizeof(mMtf), 1, mIoFileStream);
        log_file_operations_error(err);
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

        long long int pos = ftell(mIoFileStream);
#ifdef _DEBUG
        std::cout << "byte offset on creation = " << (long long int)pos << std::endl;
#endif // _DEBUG
        err = fseek(mIoFileStream, 0, SEEK_SET);
        log_file_operations_error(err);
        err = fwrite((char*)&mFileHeader, sizeof(mFileHeader), 1, mIoFileStream);
        log_file_operations_error(err);
        err = fseek(mIoFileStream, pos,  SEEK_SET);
        log_file_operations_error(err);
#ifdef _DEBUG
        std::cout << "byte offset on creation = " << (long long int)pos << std::endl;
#endif // _DEBUG

#ifdef _DEBUG
        PrintFileHeader();
#endif // _DEBUG
        fclose(mIoFileStream);
        std::cout << " --- Opening file both for reading and writing --- " << std::endl;
        err = fopen_s(&mIoFileStream, FILENAME, "r+");
        log_file_operations_error(err);
#ifdef _DEBUG
        err = fseek(mIoFileStream, 0, SEEK_END);
        long tell = ftell(mIoFileStream);
        log_file_operations_error(err);
        std::cout << "fteel in at final of create = " << tell << std::endl;
#endif // _DEBUG
    }
}

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
inline IO<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::~IO()
{
    fclose(mIoFileStream);
}

#endif // !IO_H