#include <cassert>
#include <iostream>
#include <limits>
#include <numbers>

namespace detail {
/// @brief Helper function that reads integer from standard input
bool readInt32(std::istream& istream, int32_t& outValue) {
    if (istream >> outValue) {
        return true;
    }
    istream.clear();
    istream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return false;
}

/// @brief Returns a pointer to an allocated and zeroed array of ints on the heap
int32_t* allocArray(const int32_t arrSize) {
    int32_t* arr = new (std::nothrow) int[arrSize]{};
    assert(arr && "Failed to allocate memory");
    return arr;
}
}  // namespace detail

/// @brief Reads an array from standard input and stores it on the heap
int32_t* readArray(const int32_t arrSize) {
    int32_t* arr = detail::allocArray(arrSize);
    for (int32_t i = 0; i < arrSize; ++i) {
        assert(detail::readInt32(std::cin, arr[i]) && "Failed to read array entry");
    }
    return arr;
}

/// @brief Frees the memory of the given array
void deleteArray(int32_t*& arr) {
    delete[] arr;
    arr = nullptr;
}

/// @brief Finds the majority element in an array and its count using standard implementation of
/// Boyer–Moore majority vote algorithm. Time complexity O(N), space complexity O(1)
/// @param arr The array to search
/// @param arrSize The array size
/// @param outMajorityEl Out parameter that returns the majority element in the array if the
/// occurrences of that elements are more than _arrSize_ / 2. Otherwise returns the first element
/// @param outMajorityCount Out parameter that returns the count of the majority element if found
/// one. Otherwise returns -1
/// @return True if majority element occurs more than _arrSize_ / 2, false otherwise
bool hasMajority(const int32_t* const arr, const int32_t arrSize, int32_t& outMajorityEl,
                 int32_t& outMajorityCount) {
    assert(arr && "No array provided");
    outMajorityCount = 1;
    outMajorityEl = arr[0];
    for (int32_t i = 1; i < arrSize; ++i) {
        if (outMajorityEl == arr[i]) {
            outMajorityCount++;
        } else {
            outMajorityCount--;
            if (outMajorityCount == 0) {
                outMajorityEl = arr[i];
                outMajorityCount = 1;
            }
        }
    }

    outMajorityCount = 0;
    for (int32_t i = 0; i < arrSize; i++) {
        if (arr[i] == outMajorityEl) {
            outMajorityCount++;
        }
    }

    if (outMajorityCount < arrSize / 2) {
        outMajorityEl = arr[0];
        outMajorityCount = -1;
        return false;
    }

    return true;
}

int main() {
    int32_t arrSize = 0;
    std::cout << "Program that finds element of an array with more than a half of occurrences\n";
    std::cout << "Enter array size: ";
    if (!detail::readInt32(std::cin, arrSize) || arrSize <= 0) {
        std::cerr << "Failed to read array size. Program exit with error code 1" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Read input array
    std::cout << "Enter an array of size " << arrSize << ": ";
    int* inputArr = readArray(arrSize);

    // Find the majority element and its count (if any)
    int32_t majorityEl = inputArr[0], majorityCount = -1;
    if (hasMajority(inputArr, arrSize, majorityEl, majorityCount)) {
        std::cout << "The element with more than a half of occurrences is: " << majorityEl
                  << " with " << majorityCount << " occurrences" << std::endl;
    } else {
        std::cout << "No element with more than a half of occurrences found." << std::endl;
    }

    // Free the heap
    deleteArray(inputArr);

    return 0;
}
