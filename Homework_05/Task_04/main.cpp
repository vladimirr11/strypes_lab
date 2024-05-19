#include <iostream>
#include <limits>
#include <numbers>
#include <vector>

/// @brief Recursive function to find the nth Fubonacci number
uint64_t findFibonacciNumRec(const int32_t num, std::vector<uint64_t>& memo) {
    if (num <= 1) {  // Base case
        return num;
    }
    // If the result for this number is already computed dont step into this recursive branch
    if (memo[num] > 0) {
        return memo[num];
    }

    memo[num] = findFibonacciNumRec(num - 1, memo) + findFibonacciNumRec(num - 2, memo);
    return memo[num];
}

/// @brief Iterative function to find the nth Fubonacci number
uint64_t findFibonacciNumIter(const int32_t num) {
    if (num <= 1) {
        return num;
    }

    uint64_t prevPrevFib = 0, prevFib = 1, fib = 0;
    for (int32_t i = 2; i <= num; ++i) {
        fib = prevFib + prevPrevFib;
        prevPrevFib = prevFib;
        prevFib = fib;
    }

    return fib;
}

/// @brief Tries to read positive number
bool readPositiveInt32(std::istream& istream, int32_t& num) {
    if (istream >> num && num >= 0) {
        return true;
    }
    istream.clear();
    istream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cerr << "Invalid input. Please enter positive number. Try again.\n";
    return false;
}

int main() {
    int32_t num = 0;
    do {
        std::cout << "Enter number to find its Fibonacci value: ";
    } while (!readPositiveInt32(std::cin, num));

    std::vector<uint64_t> memo(num + 1, 0);
    std::cout << "Fibonacci(" << num << ") = " << findFibonacciNumRec(num, memo) << "\n";
    std::cout << "Fibonacci(" << num << ") = " << findFibonacciNumIter(num) << "\n";

    return 0;
}
