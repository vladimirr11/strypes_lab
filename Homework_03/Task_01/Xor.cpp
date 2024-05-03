#include <iostream>
#include <sstream>

int32_t logicalXor(const int32_t a, const int32_t b) {
    // return (!a != !b); // This approach is potentially faster - but it uses comparison operator
    return (a || b) && !(a && b);  // Using only logical operators
}

int main() {
    int32_t a, b;
    std::cout << "Logical XOR function\n";

    while (!std::cin.eof()) {
        std::cin >> a >> b;
        std::cout << "xor(" << a << ", " << b << ") = " << logicalXor(a, b) << "\n";
    }

    return 0;
}
