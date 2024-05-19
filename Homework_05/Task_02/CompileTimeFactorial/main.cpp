#include <cassert>
#include <cstdint>
#include <iostream>
#include <tuple>
#include <utility>

/// @brief Metafunction that calculates factorial of number at compile time
template <uint64_t N>
struct StaticFactorial {
    static constexpr uint64_t value = N * StaticFactorial<N - 1>::value;
};

/// @brief Specialization that defines one of the base cases when N = 0
template <>
struct StaticFactorial<0> {
    static constexpr uint64_t value = 1;
};

/// @brief Specialization that defines a base case when N = 1
template <>
struct StaticFactorial<1> {
    static constexpr uint64_t value = 1;
};

/// @brief Function template that computes the nth factorial number and test its validity
template <typename IntT, IntT nthNum>
void runTest() {
    constexpr auto factorial = [=]() {
        IntT res = 1;
        for (IntT i = nthNum; i >= 2; --i) {
            res *= i;
        }
        return res;
    }();
    std::cout << "Factorail for number " << nthNum << " = " << factorial;
    static_assert(StaticFactorial<nthNum>::value == factorial, " [TEST FAILED]");
    std::cout << " [TEST PASSED]\n";
}

/// @brief Run test cases for an integer sequence from 0 to idxPack::size - 1
template <typename IntT, IntT... idxPack>
void testStaticFactorial(std::integer_sequence<IntT, idxPack...>) {
    (runTest<IntT, idxPack>(), ...);
}

int main() {
    // Just for fun :)
    testStaticFactorial(std::make_integer_sequence<uint64_t, 21>{});

    return 0;
}
