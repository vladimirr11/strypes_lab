#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <type_traits>

/// @brief The Kaham summation algorithm - used to improve the accuracy of adding a series of
/// floating-point numbers
template <typename FloatT, typename = std::enable_if_t<std::is_floating_point_v<FloatT>>>
FloatT accumKahamSum(const FloatT startValue, const FloatT toAdd = 0.1) {
    FloatT sum = startValue;
    FloatT err = 0;
    for (int32_t i = 0; i < 10; i++) {
        FloatT currVal = toAdd - err;
        FloatT currSum = sum + currVal;
        err = (currSum - sum) - currVal;
        sum = currSum;
    }
    return sum;
}

/// @brief Plain floating-point summation
template <typename FloatT, typename = std::enable_if_t<std::is_floating_point_v<FloatT>>>
FloatT accumDefault(const FloatT startValue, const FloatT toAdd = 0.1) {
    FloatT sum = startValue;
    for (int32_t i = 0; i < 10; i++) {
        sum += toAdd;
    }
    return sum;
}

template <typename FloatT, typename = std::enable_if_t<std::is_floating_point_v<FloatT>>>
bool isEqualStaticComp(const FloatT f1, const FloatT f2) {
    const FloatT eps = std::numeric_limits<FloatT>::epsilon();
    return std::fabs(f1 - f2) <= eps;
}

template <typename FloatT, typename = std::enable_if_t<std::is_floating_point_v<FloatT>>>
bool isEqualDynamicComp(const FloatT f1, const FloatT f2) {
    const FloatT eps = std::numeric_limits<FloatT>::epsilon();
    return std::fabs(f1 - f2) <= eps * std::fmax(f1, f2);
}

template <typename FloatT, typename = std::enable_if_t<std::is_floating_point_v<FloatT>>>
bool isEqualExactComp(const FloatT f1, const FloatT f2) {
    return f1 == f2;
}

double getRandomDouble(const double from = 0.0, const double to = 1.0) {
    static std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> uniformDist(from, to);
    return uniformDist(engine);
}

size_t getRandomUInt64(const size_t from, const size_t to) {
    static std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<size_t> uniformDist(from, to);
    return uniformDist(engine);
}

template <typename FloatT, typename = std::enable_if_t<std::is_floating_point_v<FloatT>>>
void runTestCase(const std::string& message, const FloatT f1, const FloatT f2,
                 bool (*compFunc)(const FloatT, const FloatT)) {
    std::cout << message;
    if (compFunc(f1, f2)) {
        std::cout << "[PASSED]\n";
    } else {
        std::cout << "[FAILED WITH DIFF] " << std::fabs(f1 - f2) << "\n";
    }
}

template <typename FloatT, typename = std::enable_if_t<std::is_floating_point_v<FloatT>>>
void runTests(const FloatT randRealNum) {
    static int32_t testCounter = 1;
    std::cout << "============= Run comparison test #" << testCounter++ << " =============\n";

    const FloatT expectedValue = randRealNum + 1.0;
    const FloatT kahamSumValue = accumKahamSum(randRealNum);
    const FloatT plainSumValue = accumDefault(randRealNum);

    std::cout << "Expected number:  " << std::fixed << std::setprecision(sizeof(randRealNum) * 8)
              << expectedValue << "\n";
    std::cout << "Kaham sum number: " << std::fixed << std::setprecision(sizeof(randRealNum) * 8)
              << kahamSumValue << "\n";
    std::cout << "Plain sum number: " << std::fixed << std::setprecision(sizeof(randRealNum) * 8)
              << plainSumValue << "\n";

    // Run test cases for the Kaham summation
    std::cout << ">>> KAHAM SUM TESTS:\n";
    runTestCase("Exact compare test:   ", expectedValue, kahamSumValue, isEqualExactComp<FloatT>);
    runTestCase("Static compare test:  ", expectedValue, kahamSumValue, isEqualStaticComp<FloatT>);
    runTestCase("Dynamic compare test: ", expectedValue, kahamSumValue, isEqualDynamicComp<FloatT>);

    // Run test cases for the plain summation
    std::cout << ">>> PLAIN SUM TESTS:\n";
    runTestCase("Exact compare test:   ", expectedValue, plainSumValue, isEqualExactComp<FloatT>);
    runTestCase("Static compare test:  ", expectedValue, plainSumValue, isEqualStaticComp<FloatT>);
    runTestCase("Dynamic compare test: ", expectedValue, plainSumValue, isEqualDynamicComp<FloatT>);

    std::cout << std::endl;
}

int main() {
    size_t startRange = 0;
    for (size_t i = 1; i < (size_t)1e12; i *= 2) {
        // Switch these to test float and double
        // const float randReal = getRandomUInt64(startRange, i) + getRandomDouble();
        const double randReal = getRandomUInt64(startRange, i) + getRandomDouble();
        runTests(randReal);
        startRange = i;
    }

    return 0;
}
