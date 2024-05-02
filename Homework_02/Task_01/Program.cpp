#include <iostream>
#include "Swap.h"

int main(void) {
    int num1, num2;
    std::cout << "Enter first number: ";
    std::cin >> num1;
    std::cout << "Enter second number: ";
    std::cin >> num2;
    std::cout << "Before swap: Num1 = " << num1 << ", Num2 = " << num2 << std::endl;
    swap(num1, num2);
    std::cout << "After swap: Num1 = " << num1 << ", Num2 = " << num2 << std::endl;

    return 0;
}
