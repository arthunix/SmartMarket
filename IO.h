#pragma once
#ifndef IO_H
#define IO_H

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "Product.h"

/*
* FILE STRUCTURE ORGANIZATION :
* 
first block     : 1 gb - inventory
second block    : 1 gb - shelf
*/

class IO {
private:
    std::string _fileName;
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

#endif !IO_H
