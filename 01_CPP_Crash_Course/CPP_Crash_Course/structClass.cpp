#include "structClass.h"
#include <iostream>
#include <vector>
//#include <cmath>

class ComplexNumber
{
public: // Ez alatt publikus
	float real = 0;
	float imaginary = 0;

	ComplexNumber() {};
	ComplexNumber(float _real, float _imaginary)
	{
		real = _real;
		imaginary = _imaginary;
	}

	void add(const ComplexNumber& rhs)
	{
		real += rhs.real;
		imaginary += rhs.imaginary;
	}

	float magnitude() const
	{
		return std::sqrt(real * real + imaginary * imaginary);
	}
};

class X
{
	std::string name = "default_name";
public:
	// Lefut, amikor egy új egyedét hozzuk létre az osztálynak
	X() {
		std::cout << "Constructor " << name << std::endl;
	}

	X(int value, const std::string& _name) {
		name = _name;
		std::cout << "Constructor with value: " << value << " " << name << std::endl;
	}

	// Lefut, amikor az egyed az élettartalma végéhez ért
	~X() {
		std::cout << "Destructor " << name << std::endl;
	}
};

void structClass()
{
	// C-hez képest itt a struct-nak lehet metódusa, statikus változója
	// konstruktora, destrutora, stb.

	// CPP-ban bejött egy új kulcsszó, ami a class.
	// Struct-ban a változók alapból publikusak, míg a class-ban privátok, más lényegi különbség nincs
	ComplexNumber n;
	n.imaginary = 1.0f;
	n.real = 0.0f;

	ComplexNumber n2(3.0f, 4.0f);
	n2.add(n);
	std::cout << "complex number magnitude: " << n2.magnitude() << std::endl;

	// Osztáloy mérete fix -> le tudjuk kérdezni a méretüket, fordítás alatt
	std::cout << "ComplexNumber size: " << sizeof(ComplexNumber) << std::endl;
	// Fordításkor tudnunk kell az osztályok méretét -> le tudjuk kérdezni, hogy hol vannak az egyes tagok az osztályban (byte eltolás)
	std::cout << "Offset of real: " << offsetof(ComplexNumber, real) << std::endl;
	std::cout << "Offset of imaginary: " << offsetof(ComplexNumber, imaginary) << std::endl;

	// Konstruktor, destruktor példa
	{
		X x1;
		X x2(100, "x2");
	}

	// char és int mérete
	std::cout << "Char size: " << sizeof(char) << " Int size: " << sizeof(int) << std::endl;
	
	// Miért változik a méret, ha más sorrendben vannak az adattagok?
	// struct S1 {
	// 	char c1;
	// 	char c2;
	// 	int i;
	// };
	// struct S2 {
	// 	char c1;
	// 	int i;
	// 	char c2;
	// };
	// std::cout << "S1: " << sizeof(S1) << " S2: " << sizeof(S2) << std::endl;
}