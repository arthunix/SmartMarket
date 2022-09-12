#pragma once
#ifndef PRODUCT_H
#define PRODUCT_H

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <ctime>

#define NAME_SIZE 20
#define DESCRIPTION_SIZE 80
#define BRAND_SIZE 10

class Product {
private:
	char mName[NAME_SIZE];
	char mDescription[DESCRIPTION_SIZE];
	char mBrand[BRAND_SIZE];
	time_t mExpirationDate;
	time_t mManufacturingDate;
	time_t mAdditionDate;
	double mPrice;
	unsigned int mLote;
	// BarCode _barcode; /* Stands for computer vision I think */
public:
	Product(char Name[NAME_SIZE], char Description[DESCRIPTION_SIZE], char Brand[BRAND_SIZE], time_t ExpirationDate,
		time_t ManufacturingDate, unsigned int Lote, double Price);

	~Product();

	const char* getName();

	void setName(const char* Name);

	const char* getDescription();

	void setDescription(const char* Description);

	const char* getBrand();

	void setBrand(const char* Brand);

	time_t getExpirationDate();

	void setExpirationDate(time_t ExpirationDate);

	time_t getManufacturingDate();

	void setManufacturingDate(time_t ManufacturingDate);

	time_t getAdditionDate();

	void setAdditionDate(time_t AdditionDate);

	unsigned int getLote();

	void setLote(unsigned int Lote);

	double getPrice();

	void setPrice(double Price);

	void printProduct()
	{
		std::cout << "Name: " << mName << std::endl;
		std::cout << "Description: " << mDescription << std::endl;
		std::cout << "Brand: " << mBrand << std::endl;

		struct tm  when;
		char buff[80];

		localtime_s(&when, &mExpirationDate);
		asctime_s(buff, sizeof(buff), &when);
		printf("Expiration time is %s", buff);

		localtime_s(&when, &mManufacturingDate);
		asctime_s(buff, sizeof(buff), &when);
		printf("Expiration time is %s", buff);

		localtime_s(&when, &mAdditionDate);
		asctime_s(buff, sizeof(buff), &when);
		printf("Expiration time is %s", buff);

		std::cout << "Lote: " << mLote << std::endl;
		std::cout << "Price: " << mPrice << std::endl;
	}
};

Product::Product (
char Name[NAME_SIZE], 
char Description[DESCRIPTION_SIZE],
char Brand[BRAND_SIZE],
time_t ExpirationDate, 
time_t ManufacturingDate,
unsigned int Lote,
double Price
)
{
	strcpy_s(mName, NAME_SIZE, Name);
	strcpy_s(mDescription, DESCRIPTION_SIZE, Description);
	strcpy_s(mBrand, BRAND_SIZE, Brand);
	mExpirationDate = ExpirationDate;
	mManufacturingDate = ManufacturingDate;
	time_t currTime; mAdditionDate = time(&currTime);
	mLote = Lote;
	mPrice = Price;
}

Product::~Product()
{
	std::cerr << "TRACE RING 5 : Destructing the object" << std::endl;
}

const char* Product::getName()
{
	return mName;
}

void Product::setName(const char* Name)
{
	strcpy_s(mName, NAME_SIZE, Name);
}

const char* Product::getDescription()
{
	return mDescription;
}

void Product::setDescription(const char* Description)
{
	strcpy_s(mDescription, DESCRIPTION_SIZE, Description);
}

const char* Product::getBrand()
{
	return mBrand;
}

void Product::setBrand(const char* Brand)
{
	strcpy_s(mBrand, BRAND_SIZE, Brand);
}

time_t Product::getExpirationDate()
{
	return mExpirationDate;
}

void Product::setExpirationDate(time_t ExpirationDate)
{
	mExpirationDate = ExpirationDate;
}

time_t Product::getManufacturingDate()
{
	return mManufacturingDate;
}

void Product::setManufacturingDate(time_t ManufacturingDate)
{
	mManufacturingDate = ManufacturingDate;
}

time_t Product::getAdditionDate()
{
	return mAdditionDate;
}

void Product::setAdditionDate(time_t AdditionDate)
{
	mAdditionDate = AdditionDate;
}

unsigned int Product::getLote()
{
	return mLote;
}

void Product::setLote(unsigned int Lote)
{
	mLote = Lote;
}

double Product::getPrice()
{
	return mPrice;
}

void Product::setPrice(double Price)
{
	mPrice = Price;
}

#endif !PRODUCT_H
