#pragma once
#ifndef PRODUCT_H
#define PRODUCT_H

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <ctime>

typedef unsigned long long int uuid_t;
typedef unsigned long long int rrn_t;

class Product {
private:
	uuid_t _rrn;
	char _name[20];
	char _description[80];
	char _brand[10];
	time_t _expiration_date;
	time_t _manufacturing_date;
	time_t _addition_date;
	unsigned int _lote;
	// BarCode _barcode; /* Stands for computer vision I think */
public:
	Product();
	~Product();
	const char* getName();
	void setName(const char* name);
	const char* getDescription();
	void setDescription(const char* description);
	const char* getBrand();
	void setBrand(const char* brand);
	time_t getExpirationDate();
	void setExpirationDate(time_t expiration_date);
	time_t getManufacturingDate();
	void setManufacturingDate(time_t manufacturing_date);
	time_t getAdditionDate();
	void setAdditionDate(time_t adding_date);
	unsigned int getLote();
	void setLote(unsigned int lote);
};


#endif !PRODUCT_H
