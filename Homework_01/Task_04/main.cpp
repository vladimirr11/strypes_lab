#include <array>
#include <fstream>
#include <iostream>
#include <limits>

template <typename T, size_t N>
T findMaxNumber(const std::array<T, N>& numsArr) {
    T maxNum = std::numeric_limits<T>::min();
    for (const auto num : numsArr) {
        if (num > maxNum) {
            maxNum = num;
        }
    }
    return maxNum;
}

int main() {
    const char* filePath = "Homework_01/Task_04/numbers.txt";
    std::ifstream inputFile(filePath, std::ios::in);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open input file\n";
        exit(EXIT_FAILURE);
    }

    std::array<float, 3> numsArr;
    inputFile >> numsArr[0] >> numsArr[1] >> numsArr[2];
    if (inputFile.eof()) {
        std::cout << "Numbers successfully read\n";
    }
    inputFile.close();

    std::cout << "Max number is: " << findMaxNumber(numsArr) << "\n";

    return 0;
}
