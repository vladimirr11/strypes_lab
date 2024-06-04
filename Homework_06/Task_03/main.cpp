#include <cassert>
#include <cstring>
#include <iostream>
#include <limits>
#include <numbers>

// Note: this program supports only standard ASCII codes. No extended ASCII.
static constexpr int32_t MAX_STRING_SIZE = 1024;
static constexpr int32_t STANDARD_ASCII_CODES = 128;
static constexpr int32_t UNPRINTABLE_ASCII_CODES = 32;
static constexpr int32_t MAX_PRINTABLE_SYMS = STANDARD_ASCII_CODES - UNPRINTABLE_ASCII_CODES;

namespace detail {
/// @brief Helper function to read string from standard input limited by the constant
/// _kMaxStringSize_. Returns the string as out parameter
bool readString(std::istream& istream, char str[]) {
    if (istream.getline(str, MAX_STRING_SIZE)) {
        return true;
    }
    std::memset(str, 0, MAX_STRING_SIZE);  // Zero out str
    istream.clear();
    istream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return false;
}
}  // namespace detail

/// @brief Checks if two given C-style strings are anagrams of each other. It uses standard
/// algorithm to count and store the occurrences of each char in _str1_ and then subtract the chars
/// from _str2_ until 1) mismatch found (count less than 0), or 2) both strings have different
/// lengths. Time complexity O(len(_str1_) + len(_str2_)), space complexity O(MAX_PRINTABLE_SYMS)
bool areStringsAnagrams(const char str1[], const char str2[]) {
    int32_t countedChars[MAX_PRINTABLE_SYMS]{};

    int32_t strOneCounter = 0;
    while (*str1) {
        const int32_t currCharIdx = *str1++ - UNPRINTABLE_ASCII_CODES;
        // Auxiliary check - can be removed with NDEBUG
        assert(currCharIdx >= 0 && currCharIdx < MAX_PRINTABLE_SYMS && "Unsupported char found");
        countedChars[currCharIdx]++;
        strOneCounter++;
    }

    int32_t strTwoCounter = 0;
    while (*str2 && strOneCounter > strTwoCounter++) {
        const int32_t currCharIdx = *str2++ - UNPRINTABLE_ASCII_CODES;
        assert(currCharIdx >= 0 && currCharIdx < MAX_PRINTABLE_SYMS && "Unsupported char found");
        countedChars[currCharIdx]--;
        if (countedChars[currCharIdx] < 0) {  // Mismatch found
            return false;
        }
    }

    if (strOneCounter != strTwoCounter) {  // Both strings have different lengths
        return false;
    }

    return true;
}

int main() {
    std::cout << "Program that checks if two strings are anagrams\nEnter string one: ";

    char str1[MAX_STRING_SIZE]{};
    if (!detail::readString(std::cin, str1)) {
        std::cerr << "Failed to read string one." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Enter string two: ";
    char str2[MAX_STRING_SIZE]{};
    if (!detail::readString(std::cin, str2)) {
        std::cerr << "Failed to read string two." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (areStringsAnagrams(str1, str2)) {
        std::cout << "Both strings are anagrams." << std::endl;
    } else {
        std::cout << "Both strings are NOT anagrams." << std::endl;
    }

    return 0;
}
