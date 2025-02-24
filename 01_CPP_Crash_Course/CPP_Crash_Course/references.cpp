#include "references.h"
#include <iostream>

void references()
{
	// Azt láttuk, hogy a pointerek néha nem a legfelhasználóbarátabbak
	// Helyette a legtöbb helyen referenciákat használunk, de
	// nem kell megijedni fogunk használni pointereket és pointer aritmetikát is

	// Miért használunk pointereket?
	int number = 0;
	increase(number);
	std::cout << "number: " << number << std::endl;
	// Látjuk, hogy nem mûködik, helyette:
	increasePointer(&number);
	std::cout << "number: " << number << std::endl;
	// De hogyan nézne ki ez referenciákkal?
	increaseReference(number);
	std::cout << "number: " << number << std::endl;

	// Szóval, a referencia csak szintaktikai cukorka pointerekhez?
	// NEM

	// Például lehet nullptr-ünk de nem lehet null referenciánk
	// igazából a referenciák nem is lehetnek nem inicializálva

	int a = 5;
	int* ap;
	ap = nullptr;
	ap = &a;

	//int& ar;		// Fordítási hiba
	int& ar = a;	// Helyes

	// Refenciáknál nem lehet megváltoztatni, hogy mire mutatnak
	int b = 0;
	ar = b;	// a-nak az értékét változtatja

	// Lefordul vagy sem?
	int aNumber = 7;
	const int constNumber = 8;

	// int& ref0;
	// int& ref1 = aNumber;
	// int& ref2 = constNumber;
	// int& ref3 = 9;

	// const int& ref4;
	// const int& ref5 = aNumber;
	// const int& ref6 = constNumber;
	// const int& ref7 = 9;
}

void increase(int n)
{
	// n másolódik
	n++;
}

void increasePointer(int* np)
{
	// itt az np-t másoljuk és azt dereferáljuk
	// , hogy megkapjuk a számot 
	(*np)++;
}

void increaseReference(int& n)
{
	n++;
}
