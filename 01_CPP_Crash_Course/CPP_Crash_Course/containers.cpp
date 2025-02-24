#include "containers.h"
#include <vector>
#include <array>
#include <iostream>

void printVector(const std::vector<int>& vec);

void containers()
{
	// Azt láthattuk, hogy nem a legkényelmesebb
	// manuálisan foglalni memóriát, vagy 
	// külön változót fent tartani iteráláshoz
	// erre van a container könyvtárak amiben különbözõ
	// adatstruktúrák vannak
	// https://en.cppreference.com/w/cpp/container

	// A félév során a legtöbbet használt osztály a std::vector
	// nem összekeverendõ a vektorokkal matekból
	// #include <vector> -ban van

	std::vector<int> emptyVector; // Üres vektor inicializálás, <int> -> int-et tartalmaz
	std::vector<int> vectorWithValues = { 0,1,2 };			// 0,1,2 értékekkel inicializálva
	std::vector<int> vectorFromVector1 = vectorWithValues;	// Copy constructor (deep copy)
	std::vector<int> vectorFromVector2(vectorWithValues);	// ez is copy constructor (deep copy)

	// Push back, a vector mérete dinamikus és az adatokat a heap-en tárolja
	emptyVector.push_back(3);
	emptyVector.push_back(2);

	std::cout << "vector with for loop: ";
	for (int i = 0; i < emptyVector.size(); ++i) // .size() hogy hány darab elem van benne
	{
		std::cout << emptyVector[i] << " "; // Mûködik a [] operátorral való indexelés
	}
	std::cout << std::endl;

	// Ha egy pointert akarunk az adattömb elemeihez
	int* p = vectorWithValues.data();
	// int* p = &vectorWithValues[0]; // Ez is mûködik
	*p = 10;

	printVector(vectorWithValues);

	// Van a stack-en lévõ array container
	std::array<double, 6> doubleArr{ 0,0,0,0,0,0 }; // <adat tipus, méret>
	std::cout << "Array size/capacity: " << doubleArr.size() << std::endl;
}

void printVector(const std::vector<int>& vec)
{
	// Van for each is, lehet választani
	// Mi a különbség közöttük?
	// Melyik megy most, melyik nem és miért?
	std::cout << "vector with for each: ";
	for (const int& v : vec)
	//for (int& v : vec)
	//for (int v : vec)
	{
		std::cout << v << " ";
	}
	std::cout << std::endl;
}