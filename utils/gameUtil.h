#ifndef DANHLL_GAMEUTIL_H
#define DANHLL_GAMEUTIL_H
#include "config.h"


struct Vector2 {
    double x, y;

    // Constructor
    Vector2(double _x = 0.0, double _y = 0.0)
        : x(_x), y(_y) {}
};

struct Vector3 {
    float x, y, z;

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

struct FMinimalViewInfo {
    Vector3 Location;
    Vector3 Rotation;
    float FOV;

    //more values not not really important

    void Print() const {
        printf("========== [ Camera View Info ] ==========\n");
        printf("[+] Location : X: %7.2f | Y: %7.2f | Z: %7.2f\n", Location.x, Location.y, Location.z);
        printf("[+] Rotation : P: %7.2f | Y: %7.2f | R: %7.2f\n", Rotation.x, Rotation.y, Rotation.z);
        printf("[+] FOV      : %.2f\n", FOV);
        printf("==========================================\n");
    }
};

struct FCameraCacheEntry {
    float timestamp{};
    char pad_0x0004[12];
    FMinimalViewInfo viewInfo;
};

/*
    TYPE	MEMBER	OFFSET	SIZE
    FVector	Location	0x0	12
    FRotator	Rotation	0xc	12
    float	FOV	0x18	4
    float	DesiredFOV	0x1c	4
    float	OrthoWidth	0x20	4
    float	OrthoNearClipPlane	0x24	4
    float	OrthoFarClipPlane	0x28	4
    float	AspectRatio	0x2c	4
    uint8	bConstrainAspectRatio : 1	0x30 : 0	1
    uint8	bUseFieldOfViewForLOD : 1	0x30 : 1	1
    ECameraProjectionMode	ProjectionMode	0x34	1
    float	PostProcessBlendWeight	0x38	4
    FPostProcessSettings	PostProcessSettings	0x40	1344
    FVector2D	OffCenterProjectionOffset	0x580	8
 */


#endif //DANHLL_GAMEUTIL_H