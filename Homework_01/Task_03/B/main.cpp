#include <cassert>
#include <cmath>
#include <iostream>

// The PI constant
static constexpr float PI = 3.141592653589793238f;

class HorizontalCylinderVolumeCalculator {
public:
    HorizontalCylinderVolumeCalculator() = delete;

    HorizontalCylinderVolumeCalculator(const float radius, const float length,
                                       const float filledHeight)
        : barrelRadius(radius), barrelLength(length), barrelFilledHeight(filledHeight) {}

    float calcVolumeInCubicMeters() const {
        assert(barrelRadius > 0.f && barrelLength > 0.f && barrelFilledHeight > 0.F &&
               "Barrel radius, length, and filled height must be positive numbers");

        const float radiusMinusHeight = barrelRadius - barrelFilledHeight;
        const float heightSquared = barrelFilledHeight * barrelFilledHeight;
        const float radiusSquared = barrelRadius * barrelRadius;
        const float areaOfSector = std::acos(radiusMinusHeight / barrelRadius) * radiusSquared;
        const float areaOfTriangle =
            radiusMinusHeight * std::sqrt(2 * barrelRadius * barrelFilledHeight - heightSquared);
        const float area = areaOfSector - areaOfTriangle;
        const float volume = area * barrelLength;

        return volume;
    }

    float getRadius() const { return barrelRadius; }
    float getLength() const { return barrelLength; }
    float getFilledHeight() const { return barrelFilledHeight; }

private:
    float barrelRadius{};
    float barrelLength{};
    float barrelFilledHeight{};
};

int main() {
    float barrelRadius = 0.f, barrelLength = 0.f, barrelFilledHeight = 0.f;

    std::cout << "Enter barrel radius in meters: ";
    std::cin >> barrelRadius;
    if (std::cin.fail()) {
        std::cerr << "Program failed to read barrel radius\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "Enter barrel length in meters: ";
    std::cin >> barrelLength;
    if (std::cin.fail()) {
        std::cerr << "Program failed to read barrel length\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "Enter barrel filled height in meters: ";
    std::cin >> barrelFilledHeight;
    if (std::cin.fail()) {
        std::cerr << "Program failed to read barrel filled height\n";
        exit(EXIT_FAILURE);
    }

    HorizontalCylinderVolumeCalculator calc(barrelRadius, barrelLength, barrelFilledHeight);

    std::cout << "Volume of horizontal cylinder is: " << calc.calcVolumeInCubicMeters() << " m^3\n";

    return 0;
}
