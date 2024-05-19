#include <cmath>
#include <iostream>
#include <limits>
#include <numbers>

/// @brief Simple function to check if number is prime. Time complexity O(sqrt(num)), space
/// complexity O(1).
bool checkPrimeNum(const int64_t num) {
    if (num <= 1) {  // 0 and 1 are not considered prime numbers
        return false;
    }

    // Even numbers greater than 2 are not prime (since they are divisible by 2)
    if (num % 2 == 0 && num > 2) {
        return false;
    }

    // Check all odd numbers less that sqrt(num) to verify if num is prime
    const int64_t rootNum = std::sqrt(num);
    for (int64_t i = 3; i <= rootNum; i += 2) {
        if (num % i == 0) {
            return false;
        }
    }
    
    return true;
}

/// @brief Tries to read integer greater than 0.
bool readPositiveInt64(std::istream& istream, int64_t& num) {
    if (istream >> num && num >= 0) {
        return true;
    }
    istream.clear();
    istream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cerr << "Invalid input. Please enter integer greater than 0. Try again.\n";
    return false;
}

int main() {
    int64_t num = 0;
    do {
        std::cout << "Enter number to check if it's prime: ";
    } while (!readPositiveInt64(std::cin, num));

    if (checkPrimeNum(num)) {
        std::cout << num << " IS prime number\n";
    } else {
        std::cout << num << " IS NOT prime number\n";
    }

    return 0;
}
