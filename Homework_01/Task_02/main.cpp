#include <cassert>
#include <cmath>
#include <iostream>

class BodyMassIndexCalculator {
public:
    BodyMassIndexCalculator() = delete;

    BodyMassIndexCalculator(const float mass, const float height)
        : bodyMass(mass), bodyHeight(height) {}

    float calcLowPrecisionBMI() const {
        assert(bodyMass > 0.0f && "Body mass must be positive number");
        assert(bodyHeight > 0.0f && "Body height must be positive number");
        return bodyMass / std::pow(bodyHeight, 2.f);
    }

    float calcHighPrecisionBMI() const {
        assert(bodyMass > 0.0f && "Body mass must be positive number");
        assert(bodyHeight > 0.0f && "Body height must be positive number");
        return 1.3f * (bodyMass / std::pow(bodyHeight, 2.5f));
    }

    float getBodyMass() const { return bodyMass; }
    float getBodyHeight() const { return bodyHeight; }

private:
    float bodyMass{};
    float bodyHeight{};
};

int main() {
    float mass = 0.0f, height = 0.0;

    std::cout << "Enter body mass in kg: ";
    std::cin >> mass;
    if (std::cin.fail()) {
        std::cerr << "Program failed to read body mass\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "Enter body height in meters: ";
    std::cin >> height;
    if (std::cin.fail()) {
        std::cerr << "Program failed to read body height\n";
        exit(EXIT_FAILURE);
    }

    BodyMassIndexCalculator bmiCalc(mass, height);

    std::cout << "Your low precision Body Mass Index is: " << bmiCalc.calcLowPrecisionBMI() << "\n";
    std::cout << "Your high precision Body Mass Index is: " << bmiCalc.calcHighPrecisionBMI()
              << "\n";

    return 0;
}
