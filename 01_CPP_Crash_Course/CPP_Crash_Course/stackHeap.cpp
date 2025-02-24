#include "stackHeap.h"
#include <iostream>

void functionOnStack()
{
    int imOnTheStackToo = 0;
    std::cout << &imOnTheStackToo << std::endl;
}

void stackHeap()
{
    // C-ben ha stack-en akarsz deklarálni objektumot
    // CPP-ben is ugyanez
    int imOnTheStack = 1;

    // Ha a heap-en akarunk allokálni C-ben
    int* onHeapCstyle = (int*)std::malloc(1 * sizeof(int)); // 1, mert csak egy int-nek akarunk helyet foglalni
    *onHeapCstyle = -5;
    std::cout << "Value on heap C style: " << *onHeapCstyle << std::endl;
    // CPP módszer
    int* onHeapCPPstyle = new int(5);
    std::cout << "Value on heap CPP style: " << *onHeapCPPstyle << std::endl;
    // Hol vannak a onHeapCstyle és a onHeapCPPstyle változók stack-en vagy a heap-en?

    // A heap-en lefoglalt memoriát nekünk kell manuálisan felszabadítani!
    // C-ben a free fv-el CPP-ben a delete operátorral
    // NE keverjük a kettõt!
    std::free(onHeapCstyle);
    delete onHeapCPPstyle;

    // Ha egy tömböt akarunk lefoglalni a stack-en
    int arrayOnStack[3];
    // Lehet választani kinek melyik indexelés tetszik
    arrayOnStack[0] = 1;
    *(arrayOnStack + 1) = 3;
    (arrayOnStack + 4)[-2] = 5;

    // Inicializálás adott értékkel, csak deklarálással együtt lehet
    // int arrayOnStack[] = {2,4,6};
    std::cout << "Array on stack: ";
    for (int i = 0; i < sizeof(arrayOnStack) / sizeof(arrayOnStack[0]); ++i)
        std::cout << arrayOnStack[i] << " ";
    std::cout << std::endl;

    // Ha a heap-en akarunk lefoglalni
    int* arrayOnHeap = new int[5] {0, 1, 2, 3, 4};
    std::cout << "Array on heap: ";
    for (int i = 0; i < 5; ++i)
        std::cout << arrayOnHeap[i] << " ";
    std::cout << std::endl;

    // NE keverd a delete[]-et a delete-vel
    delete[] arrayOnHeap;

    // A meghívott függvények és a belsõ blokkok is a stack-en vannak,
    // amit sejthetünk a memória címek közelségébõl
    std::cout << "Stack memory addresses: " << std::endl;
    {
        int valueOnTheStack = 0;
        functionOnStack();
        std::cout << &valueOnTheStack << std::endl;
    }
}