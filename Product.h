#pragma once
#ifndef PRODUCT_H
#define PRODUCT_H

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <ctime>

typedef unsigned long long int rrn_t;

class Product {
private:
	rrn_t mRrn;
	char mName[20];
	char mDescription[80];
	char mBrand[10];
	time_t mExpirationDate;
	time_t mManufacturingDate;
	time_t AdditionDate;
	unsigned int Lote;
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
