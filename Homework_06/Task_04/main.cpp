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

/// @brief Swaps the given elements
void swap(int32_t& el1, int32_t& el2) {
    el1 ^= el2;
    el2 ^= el1;
    el1 ^= el2;
}
}  // namespace detail

/// @brief Reads an matrix as 1D array from the standard input and store it on the heap
int32_t* readMatrix(const int32_t matrixDim) {
    const int32_t matrixSize = matrixDim * matrixDim;
    int32_t* matrix = new int[matrixSize]{};
    for (int32_t i = 0; i < matrixSize; ++i) {
        assert(detail::readInt32(std::cin, matrix[i]) && "Failed to read matrix entry");
    }

    return matrix;
}

/// @brief Prints 1D array as 2D matrix
void printMatrix(const int32_t* matrix, const int32_t matrixDim) {
    for (int32_t i = 0; i < matrixDim; ++i) {
        for (int32_t j = 0; j < matrixDim; ++j) {
            std::cout << matrix[i * matrixDim + j] << " ";
        }
        std::cout << std::endl;
    }
}

/// @brief Frees the heap memory for the provided matrix
void deleteMatrix(int32_t*& matrix) {
    delete[] matrix;
    matrix = nullptr;
}

/// @brief Rotates clockwise the elements of a matrix with 90 degrees inplace. It takes the matrix
/// as 1D array but it does the rotations as if the matrix is in 2D form - by layers (from outer to
/// inner) with appropriate indexing
void rotateMatrix(int32_t* const matrix, const int32_t matrixDim) {
    const int32_t matrixSize = matrixDim * matrixDim - 1;
    int32_t startRow = 0, startCol = 0;
    int32_t layerRot = matrixDim - 1;
    while (layerRot > 0) {
        const int32_t layerStartIdx = startRow * matrixDim + startCol;
        for (int32_t i = 0; i < layerRot; ++i) {
            // Came up with this super easy indexing scheme
            const int32_t topIdx = layerStartIdx + i;  // Walk left to right
            const int32_t rightIdx = layerStartIdx + layerRot + (i * matrixDim);  // Top to bottom
            const int32_t bottomIdx = matrixSize - topIdx;                        // Right to left
            const int32_t leftIdx = matrixSize - rightIdx;                        // Bottom to top

            // Rotate the elements
            int32_t temp = matrix[topIdx];
            detail::swap(matrix[leftIdx], matrix[topIdx]);
            detail::swap(matrix[bottomIdx], matrix[leftIdx]);
            detail::swap(matrix[rightIdx], matrix[bottomIdx]);
            detail::swap(matrix[rightIdx], temp);
        }
        layerRot -= 2;  // Layer rotations decrease with 2 for each outer to inner layer transition
        startRow++, startCol++; // Walk on the main diagonal
    }
}

int main() {
    int32_t matrixDim = 0;
    std::cout << "Program that rotates matrix with 90 degrees clockwise\nEnter matrix dimension: ";
    if (!detail::readInt32(std::cin, matrixDim) || matrixDim <= 0) {
        std::cerr << "Failed to read positive integer. Program exit with error code 1" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Enter " << matrixDim << "x" << matrixDim << " matrix of integers" << std::endl;

    // Read the matrix as 1D array
    int* matrix = readMatrix(matrixDim);

    std::cout << "Before matrix rotation" << std::endl;
    printMatrix(matrix, matrixDim);

    // Rotate 90 degrees clockwise the given matrix
    rotateMatrix(matrix, matrixDim);

    std::cout << "After clockwise matrix rotation of 90 degrees" << std::endl;
    printMatrix(matrix, matrixDim);

    // Free the memory
    deleteMatrix(matrix);

    return 0;
}

// 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35
// 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67
// 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99
// 100