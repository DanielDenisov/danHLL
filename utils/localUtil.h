#ifndef DANHLL_LOCALUTIL_H
#define DANHLL_LOCALUTIL_H

#include "config.h"
#include <cmath>

struct Vector2 {
    double x, y;

    // Constructor
    Vector2(double _x = 0.0, double _y = 0.0)
        : x(_x), y(_y) {}
};

struct Vector3 {
    double x, y, z;

    // Constructor
    Vector3(double _x = 0.0, double _y = 0.0, double _z = 0.0)
        : x(_x), y(_y), z(_z) {}

    // Dot product
    double Dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Subtraction operator
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    double Dist(const Vector3& other) const {
        return std::sqrt(std::pow(x - other.x, 2) +
                         std::pow(y - other.y, 2) +
                         std::pow(z - other.z, 2));
    }

    bool operator==(const Vector3& other) const {
        return (x == other.x && y == other.y && z == other.z);
    }

    void Print() {
        std::cout << x << " " << y << " " << z << std::endl;
    }
};



#endif //DANHLL_LOCALUTIL_H