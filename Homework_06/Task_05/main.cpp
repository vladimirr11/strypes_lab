#include <cassert>
#include <iostream>
#include <limits>
#include <numbers>

/// @brief Simple struct that keeps matrix data as 1D array for better spatial locality
struct Matrix {
    int32_t* data = nullptr;
    int32_t rows = 0;
    int32_t cols = 0;
};

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

/// @brief Helper function that reads positive integer from standard input
bool readPositiveInt32(std::istream& istream, int32_t& outValue) {
    if (readInt32(istream, outValue) && outValue > 0) {
        return true;
    }
    return false;
}

/// @brief Validates if matrix M can be multiplied with matrix N
bool isMatrixMulValid(const Matrix& M, const Matrix& N) {
    if (M.cols != N.rows) {
        return false;
    }
    return true;
}

/// @brief Returns matrix with data allocated (and zeroed) on the heap. Preconditions that rows and
/// cols are positive numbers are met outside the function.
Matrix allocateMatrix(const int32_t rows, const int32_t cols) {
    Matrix matrix;
    matrix.rows = rows;
    matrix.cols = cols;
    matrix.data = new (std::nothrow) int[matrix.rows * matrix.cols]{};
    assert(matrix.data && "Allocation of matrix failed");
    return matrix;
}

/// @brief Prints the given matrix
void printMatrix(const Matrix& matrix) {
    for (int32_t i = 0; i < matrix.rows; ++i) {
        for (int32_t j = 0; j < matrix.cols; ++j) {
            std::cout << matrix.data[i * matrix.cols + j] << " ";
        }
        std::cout << std::endl;
    }
}
}  // namespace detail

/// @brief Populates matrix data from standard input
Matrix readMatrix(const int32_t rows, const int32_t cols) {
    Matrix matrix = detail::allocateMatrix(rows, cols);
    const int32_t matrixSize = matrix.rows * matrix.cols;
    for (int32_t i = 0; i < matrixSize; ++i) {
        assert(detail::readInt32(std::cin, matrix.data[i]) && "Failed to read matrix entry");
    }

    return matrix;
}

/// @brief Matrix multiplication with reordered indices for better spacial locality
void matrixMul(const Matrix& M, const Matrix& N, Matrix& P) {
    for (int32_t i = 0; i < M.rows; ++i) {
        for (int32_t k = 0; k < M.cols; ++k) {
            for (int32_t j = 0; j < N.cols; ++j) {
                P.data[i * P.cols + j] += M.data[i * M.cols + k] * N.data[k * N.cols + j];
            }
        }
    }
}

/// @brief Frees the allocated storage on the heap
void deleteMatrix(Matrix& matrix) {
    delete[] matrix.data;
    matrix.data = nullptr;
}

/// @brief Prints the matrices M, N, and P
void printMatrixMulResult(const Matrix& M, const Matrix& N, const Matrix& P) {
    std::cout << "Matrix M\n";
    detail::printMatrix(M);
    std::cout << "Matrix N\n";
    detail::printMatrix(N);
    std::cout << "Matrix P\n";
    detail::printMatrix(P);
}

int main() {
    std::cout << "Matrix multiplication program P = M x N\n";

    // Input dims
    int32_t rowsM = 0, colsM = 0, rowsN = 0, colsN = 0;

    // Read and validate matrix M dims
    std::cout << "Enter dimensions for matrix M [rows, cols]: ";
    if (!detail::readPositiveInt32(std::cin, rowsM) ||
        !detail::readPositiveInt32(std::cin, colsM)) {
        std::cerr << "Failed to read dimensions for matrix M. Exit with error code 1" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Read and validate matrix N dims
    std::cout << "Enter dimensions for matrix N [rows, cols]: ";
    if (!detail::readPositiveInt32(std::cin, rowsN) ||
        !detail::readPositiveInt32(std::cin, colsN)) {
        std::cerr << "Failed to read dimensions for matrix N. Exit with error code 1" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Read matrix M data
    std::cout << "Enter data for matrix M[" << rowsM << ", " << colsM << "]" << std::endl;
    Matrix M = readMatrix(rowsM, colsM);

    // Read matrix N data
    std::cout << "Enter data for matrix N[" << rowsN << ", " << colsN << "]" << std::endl;
    Matrix N = readMatrix(rowsN, colsN);

    // Assert that matrix multiplication is possible
    if (!detail::isMatrixMulValid(M, N)) {
        std::cerr << "Matrix multiplication failed. Number of columns in M must be equal to the "
                     "number of rows in N"
                  << std::endl;
        exit(EXIT_FAILURE);
    }

    // Allocate the result matrix P
    Matrix P = detail::allocateMatrix(M.rows, N.cols);

    // P = M x N
    matrixMul(M, N, P);

    // Print the result
    printMatrixMulResult(M, N, P);

    // Free the heap memory
    deleteMatrix(M);
    deleteMatrix(N);
    deleteMatrix(P);

    return 0;
}
