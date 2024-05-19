#include <cassert>
#include <cstdint>

/// @brief Metafunction that calculate factorial of number at compile time
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

int main() {
    static_assert(StaticFactorial<0>::value == 1, "factorial<0> failed");
    static_assert(StaticFactorial<1>::value == 1, "factorial<1> failed");
    static_assert(StaticFactorial<2>::value == 2, "factorial<2> failed");
    static_assert(StaticFactorial<3>::value == 6, "factorial<3> failed");
    static_assert(StaticFactorial<4>::value == 24, "factorial<4> failed");
    static_assert(StaticFactorial<5>::value == 120, "factorial<5> failed");
    return 0;
}
