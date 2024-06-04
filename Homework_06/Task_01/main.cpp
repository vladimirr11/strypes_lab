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

/// @brief Verfies if an array is sorted
bool isArraySorted(const int32_t* const arr, const int32_t arrSize) {
    for (int32_t i = 0; i < arrSize - 1; ++i) {
        if (arr[i] > arr[i + 1]) {
            return false;
        }
    }
    return true;
}

/// @brief Simple insertion sort function
void sort(int32_t*& arr, const int32_t arrSize) {
    for (int32_t i = 1; i < arrSize; ++i) {
        const int32_t key = arr[i];
        int32_t j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
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

/// @brief Merges two sorted arrays into a third one. Returns the result as an out parameter
void merge(const int32_t* arrOne, const int32_t arrOneSize, const int32_t* arrTwo,
           const int32_t arrTwoSize, int32_t*& resultArr) {
    const int32_t resultArrSize = arrOneSize + arrTwoSize;
    int32_t i = 0, j = 0, k = 0;
    while (k < resultArrSize) {
        if (i >= arrOneSize || j >= arrTwoSize) {
            break;
        }

        while (i < arrOneSize && arrOne[i] <= arrTwo[j]) {
            resultArr[k++] = arrOne[i++];
        }

        while (j < arrTwoSize && arrTwo[j] <= arrOne[i]) {
            resultArr[k++] = arrTwo[j++];
        }
    }

    if (i < arrOneSize) {
        while (k < resultArrSize) {
            resultArr[k++] = arrOne[i++];
        }
    } else {
        while (k < resultArrSize) {
            resultArr[k++] = arrTwo[j++];
        }
    }
}

/// @brief Frees the memory of the given array
void deleteArray(int32_t*& arr) {
    delete[] arr;
    arr = nullptr;
}

/// @brief Prints the given array
void printArray(const int32_t* const arr, const int32_t arrSize) {
    std::cout << "Result array: ";
    for (int32_t i = 0; i < arrSize; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

int main() {
    int32_t arrOneSize = 0, arrTwoSize = 0;
    std::cout << "Program that merge 2 sorted arrays\n";

    // Read array 1 size
    std::cout << "Enter array one size: ";
    if (!detail::readInt32(std::cin, arrOneSize) || arrOneSize < 0) {
        std::cerr << "Failed to read size for array one. Exit with error code 1" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Read array 2 size
    std::cout << "Enter array two size: ";
    if (!detail::readInt32(std::cin, arrTwoSize) || arrTwoSize < 0) {
        std::cerr << "Failed to read size for array two. Exit with error code 1" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Read array 1 (if any)
    int32_t* arrOne = nullptr;
    if (arrOneSize > 0) {
        std::cout << "Enter arrray one: ";
        arrOne = readArray(arrOneSize);
        if (!detail::isArraySorted(arrOne, arrOneSize)) {
            std::cout << "Unsorted array entered. Sorting array...\n";
            detail::sort(arrOne, arrOneSize);
        }
    }

    // Read array 2 (if any)
    int32_t* arrTwo = nullptr;
    if (arrTwoSize > 0) {
        std::cout << "Enter arrray two: ";
        arrTwo = readArray(arrTwoSize);
        if (!detail::isArraySorted(arrTwo, arrTwoSize)) {
            std::cout << "Unsorted array entered. Sorting array...\n";
            detail::sort(arrTwo, arrTwoSize);
        }
    }

    // Allocate result array (if any arrays entered)
    const int32_t resultArrSize = arrOneSize + arrTwoSize;
    int32_t* resultArr = nullptr;
    if (resultArrSize > 0) {
        resultArr = detail::allocArray(resultArrSize);
    }

    // Merge array one and array two into the result array
    merge(arrOne, arrOneSize, arrTwo, arrTwoSize, resultArr);

    // Print the merged array
    printArray(resultArr, resultArrSize);

    // Free heap memory
    deleteArray(arrOne);
    deleteArray(arrTwo);
    deleteArray(resultArr);

    return 0;
}
