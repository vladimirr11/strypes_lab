#include <array>
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
    std::array<float, 3> numsArr;
    std::cin >> numsArr[0] >> numsArr[1] >> numsArr[2];

    std::cout << "Max number is: " << findMaxNumber(numsArr) << "\n";

    return 0;
}
