#include <cassert>
#include <iostream>

// The PI constant
static constexpr float PI = 3.141592653589793238f;

class VerticalCylinderVolumeCalculator {
public:
    VerticalCylinderVolumeCalculator() = delete;

    VerticalCylinderVolumeCalculator(const float radius, const float height)
        : barrelRadius(radius), barrelHeight(height) {}

    float calcVolumeInCubicMeters() const {
        assert(barrelRadius > 0.f && barrelHeight > 0.f &&
               "Barrel radius and height must be positive numbers");
        return PI * (barrelRadius * barrelRadius) * barrelHeight;
    }

    float getRadius() const { return barrelRadius; }
    float getHeight() const { return barrelHeight; }

private:
    float barrelRadius{};
    float barrelHeight{};
};

int main() {
    float barrelRadius = 0.f, barrelHeight = 0.f;

    std::cout << "Enter barrel radius in meters: ";
    std::cin >> barrelRadius;
    if (std::cin.fail()) {
        std::cerr << "Program failed to read barrel radius\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "Enter barrel height in meters: ";
    std::cin >> barrelHeight;
    if (std::cin.fail()) {
        std::cerr << "Program failed to read barrel height\n";
        exit(EXIT_FAILURE);
    }

    VerticalCylinderVolumeCalculator calc(barrelRadius, barrelHeight);

    std::cout << "Volume of vertical cylinder is: " << calc.calcVolumeInCubicMeters() << " m^3\n";

    return 0;
}
