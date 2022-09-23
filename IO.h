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

#ifndef _DEBUG
#define LOG_fseek_call(x) x;
#define LOG_fwrite_call(x) x;
#define LOG_fread_call(x) x;
#define LOG_fopen_call(x) x;
#else

#define LOG_fopen_call(x) {                                                                                                     \
    err = x;                                                                                                                    \
    if(err == NULL) fprintf(stderr, "ERROR at: %s on line: %i \n", #x, __LINE__);                                               \
}

#define LOG_fseek_call(x) {                                                                                                     \
    err = x;                                                                                                                    \
    if(err != 0) fprintf(stderr, "ERROR at: %s on line: %i \n", #x, __LINE__);                                                  \
}

#define LOG_fwrite_call(x) {                                                                                                    \
    err = x;                                                                                                                    \
    if(err == 0) fprintf(stderr, "ERROR writing at: %s, %i bytes written, on line: %li \n", #x, __LINE__);                      \
    else fprintf(stderr, "Writing at: %s, %i bytes written, on line: %i\n", #x, __LINE__);                                      \
}

#define LOG_fread_call(x) {                                                                                                     \
    err = x;                                                                                                                    \
    if(err == 0) fprintf(stderr, "ERROR reading at: %s, %i bytes read, on line: %li \n", #x, __LINE__);                         \
    else fprintf(stderr, "Reading at: %s, %i bytes read, on line: %li \n", #x, __LINE__);                                    \
}

#endif

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
        long int pos = ftell(mIoFileStream);
        LOG_fseek_call(fseek(mIoFileStream, 0, SEEK_SET));
        LOG_fread_call(fread((char*)&mFileHeader, sizeof(FileHeader), 1, mIoFileStream));
        LOG_fseek_call(fseek(mIoFileStream, pos, SEEK_SET));
    }

    void WriteFileHeader()
    {
        long int pos = ftell(mIoFileStream);
        LOG_fseek_call(fseek(mIoFileStream, 0, SEEK_SET));
        LOG_fwrite_call(fwrite((char*)&mFileHeader, sizeof(FileHeader), 1, mIoFileStream));
        LOG_fseek_call(fseek(mIoFileStream, pos, SEEK_SET));
    }

    void ReadFileMtf()
    {
        long int pos = ftell(mIoFileStream);
        LOG_fseek_call(fseek(mIoFileStream, mFileHeader.OffsetToMtf, SEEK_SET));
        LOG_fread_call(fread((char*)&mMtf, sizeof(mMtf), 1, mIoFileStream));
        LOG_fseek_call(fseek(mIoFileStream, pos, SEEK_SET));
    }

    void WriteFileMtf()
    {
        long int pos = ftell(mIoFileStream);
        LOG_fseek_call(fseek(mIoFileStream, mFileHeader.OffsetToMtf, SEEK_SET));
        LOG_fwrite_call(fwrite((char*)&mMtf, sizeof(mMtf), 1, mIoFileStream));
        LOG_fseek_call(fseek(mIoFileStream, pos, SEEK_SET));
    }
public:
    IO();
    ~IO();

    long long int InsertToShelf(Product product)
    {
        long long int ret = -1;
        product_controller_t loop_mtf;
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
        LOG_fwrite_call(fwrite((char*)&new_inserted_product, sizeof(product_storage_t), 1, mIoFileStream));

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

        LOG_fseek_call(fseek(mIoFileStream, byteoffset_to_product, SEEK_SET));
        LOG_fread_call(fread((char*)&lReadProductStructure, sizeof(product_storage_t), 1, mIoFileStream));

        lReadProductStructure.is_deleted = true;

        LOG_fseek_call(fseek(mIoFileStream, byteoffset_to_product, SEEK_SET));
        LOG_fwrite_call(fwrite((char*)&lReadProductStructure, sizeof(product_storage_t), 1, mIoFileStream));
        LOG_fseek_call(fseek(mIoFileStream, whereWasI, SEEK_SET));
    }

    long long int ModifyFromShelf(long byteoffset_to_product, Product modified_product)
    {
        RemoveFromShelf(byteoffset_to_product);
        return InsertToShelf(modified_product);
    }

    Product SeekOnShelf(long byteoffset_to_product)
    {
        long whereWasI = ftell(mIoFileStream);
        product_storage_t lReadProductStructure;

        LOG_fseek_call(fseek(mIoFileStream, byteoffset_to_product, SEEK_SET));
        LOG_fread_call(fread((char*)&lReadProductStructure, sizeof(product_storage_t), 1, mIoFileStream));
        LOG_fseek_call(fseek(mIoFileStream, whereWasI, SEEK_SET));

        return lReadProductStructure.product_itself;
    }

    void CleanFile();

#ifdef _DEBUG
    void printProducts()
    {
        long tell = ftell(mIoFileStream);
        product_storage_t lReadProductStructure;
        Product lReadProduct;

        std::cout << " \n\n\n--- PRINTING PRODUCTS AT SHELFS --- " << gNum << std::endl;
        ReadFileHeader();

        LOG_fseek_call(fseek(mIoFileStream, mFileHeader.OffsetToShelfBlock, SEEK_SET));

        for (int k = 0; k < gNum; k++)
        {
            std::cout << "\n";
            tell = ftell(mIoFileStream);
            std::cout << " PRINTING PRODUCT NUMBER : " << k << " OFFSET : " << tell << std::endl;
            LOG_fread_call(fread((char*)&lReadProductStructure, sizeof(product_storage_t), 1, mIoFileStream));
            lReadProduct = lReadProductStructure.product_itself;
            std::cout << "IS PRODUCT DELETED ??? = " << lReadProductStructure.is_deleted << std::endl;
            lReadProduct.printProduct();
            std::cout << "\n";
        }    
        std::cout << " --- END OF PRINTING PRODUCTS ---\n\n\n " << std::endl;
    }

    void PrintFileHeader()
    {
        ReadFileHeader();
        mFileHeader.printFileHeader();
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

        LOG_fopen_call(fopen_s(&mIoFileStream, FILENAME, "w+"));
        LOG_fseek_call(fseek(mIoFileStream, 0, SEEK_SET));
        long long int actual_byte_offset_for_fileheader = ftell(mIoFileStream);
        LOG_fwrite_call(fwrite((char*)&mFileHeader, sizeof(mFileHeader), 1, mIoFileStream));
        long long int actual_byte_offset_for_mtf = ftell(mIoFileStream);
        LOG_fwrite_call(fwrite((char*)&mMtf, sizeof(mMtf), 1, mIoFileStream));
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

        LOG_fseek_call(fseek(mIoFileStream, 0, SEEK_SET));
        LOG_fwrite_call(fwrite((char*)&mFileHeader, sizeof(mFileHeader), 1, mIoFileStream));
        LOG_fseek_call(fseek(mIoFileStream, pos,  SEEK_SET));

        fclose(mIoFileStream);

        std::cout << " --- Opening file both for reading and writing --- " << std::endl;
        LOG_fopen_call(fopen_s(&mIoFileStream, FILENAME, "r+"));
        LOG_fseek_call(fseek(mIoFileStream, 0, SEEK_END));
    }
}

template<int TNumOfSections, int TNumOfShelfsOnSection, int TNumOfProductsOnShelf>
inline IO<TNumOfSections, TNumOfShelfsOnSection, TNumOfProductsOnShelf>::~IO()
{
    fclose(mIoFileStream);
}

#endif // !IO_H