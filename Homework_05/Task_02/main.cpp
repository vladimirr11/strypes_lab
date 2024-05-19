#include <iostream>
#include <limits>
#include <numbers>

/// @brief Iterative function to find factorial of number in the range [0, 20]. Above 20 the
/// uint64_t overflows.
uint64_t findFactorialIter(const int32_t num) {
    uint64_t res = 1ull;
    for (int32_t i = num; i >= 2; --i) {
        res *= i;
    }
    return res;
}

/// @brief Recursive function to find factorial of number in the range [0, 20].
uint64_t findFactorialRec(const int32_t num) {
    return num < 2 ? 1 : num * findFactorialRec(num - 1);
}

/// @brief Tries to read integer type in the range [low, high]
bool readPositiveInt32(std::istream& istream, int32_t& num, const int32_t low = 0,
                       const int32_t high = 20) {
    if (istream >> num && num >= low && num <= high) {
        return true;
    }
    istream.clear();
    istream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cerr << "Invalid input. Please enter positive number in the range [0, 20]. Try again.\n";
    return false;
}

int main() {
    int32_t num = 0;
    do {
        std::cout << "Enter number to find its factorial: ";
    } while (!readPositiveInt32(std::cin, num));

    std::cout << num << "! = " << findFactorialIter(num) << "\n";
    std::cout << num << "! = " << findFactorialRec(num) << "\n";

    return 0;
}
