#pragma once
#ifndef IO_H
#define IO_H

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "Product.h"
#include "File.h"

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
    void CleanFile();
    void RemoveFromShelf();
    void InsertToShelf(Product new_product);
    void ModifyFromShelf();
    Product SeekOnShelf();
};

IO::IO(std::string p_Filename)
{
    /* I think I am creating a file for the first time?? This is an ethernal server
    but can we deal with energy issues but I will not deal with journaling here
    just I will verify if the file already exist */
}

#endif !IO_H
