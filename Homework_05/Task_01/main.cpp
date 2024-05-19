#include <cassert>
#include <iostream>

static const float PI = 3.14159265358979323846f;

float findAreaOfCircle(const float r) {
    assert(r > 0 && "Radius must be a positive number");
    return PI * r * r;
}

int main() {
    float r = 0.f;
    std::cout << "Enter radius of circle: ";
    std::cin >> r;
    std::cout << "The area of circle with radius " << r << " is: " << findAreaOfCircle(r) << "\n";

    return 0;
}
