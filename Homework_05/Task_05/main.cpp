#include <iostream>
#include <limits>
#include <numbers>
#include <vector>

/// @brief Extracts each digit from given number in reverse order.
std::vector<uint8_t> extractDigits(const int64_t num) {
    std::vector<uint8_t> digits;
    digits.reserve(30);
    int64_t numCopy = num;
    while (numCopy > 0) {
        const uint8_t currDigit = numCopy % 10;
        digits.push_back(currDigit);
        numCopy /= 10;
    }
    return digits;
}

/// @brief Naive implementation to check if given number is a palindrome. It extracts each digit and
/// stores it in std::vector and then iterates the vector from both sides to verify if the number is
/// palindrome.
bool isNumberPalindromeV1(const int64_t num) {
    if (num < 10) {  // Early return if number is only one digit
        return true;
    }

    const auto digitsVec = extractDigits(num);
    const auto halfSize = digitsVec.size() >> 1;
    for (size_t i = 0, j = digitsVec.size() - 1; i < halfSize; ++i, --j) {
        if (digitsVec[i] != digitsVec[j]) {
            return false;
        }
    }
    return true;
}

/// @brief More space and time efficient function to check if given number is palindrome. It builds
/// a reversed number and check if it's the same as the original one (should be true in the case of
/// palindrome).
bool isNumberPalindromeV2(const int64_t num) {
    if (num < 10) {  // Early return if number is only one digit
        return true;
    }

    int64_t copyNum = num;
    int64_t revNum = 0;
    while (copyNum > 0) {
        const int32_t currDigit = copyNum % 10;
        revNum = revNum * 10 + currDigit;
        copyNum /= 10;
    }

    return revNum == num;
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
        std::cout << "Enter number to check if it's a palindrome: ";
    } while (!readPositiveInt64(std::cin, num));

    // Use version 1 function to test if number is palindrome
    if (isNumberPalindromeV1(num)) {
        std::cout << num << " is a palindrome\n";
    } else {
        std::cout << num << " is NOT a palindrome\n";
    }

    // Use version 2
    if (isNumberPalindromeV2(num)) {
        std::cout << num << " is a palindrome\n";
    } else {
        std::cout << num << " is NOT a palindrome\n";
    }

    return 0;
}
