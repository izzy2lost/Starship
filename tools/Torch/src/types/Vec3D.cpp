#include "Vec3D.h"
#include <iomanip>

static int GetPrecision(float f) {
    int p = 0;
    int shift = 1;
    float approx = std::round(f);

    while(f != approx && p < 12 ){
        shift *= 10;
        p++;
        approx = std::round(f * shift) / shift;
    }
    return p;
}

static int GetMagnitude(float f) {
    int w = 1;
    float a = std::abs(f);

    if(a >= 1) {
        w += std::log10(a);
    } 
    if(f < 0) {
        w++;
    }
    return w;
}

Vec3f::Vec3f(float xv, float yv, float zv) : x(xv), y(yv), z(zv) {}

int Vec3f::precision() {
    auto px = GetPrecision(this->x);
    auto py = GetPrecision(this->y);
    auto pz = GetPrecision(this->z);

    return std::max(px, std::max(py, pz));
}

int Vec3f::width() {
    auto wx = GetMagnitude(this->x);
    auto wy = GetMagnitude(this->y);
    auto wz = GetMagnitude(this->z);

    return std::max(wx, std::max(wy, wz)) + 1 + this->precision();
}

std::ostream& operator<< (std::ostream& stream, const Vec3f& vec) {
    int width = stream.width();

    stream << std::setw(0) << "{" << std::setw(width) << vec.x << ", " << std::setw(width) << vec.y << ", " << std::setw(width) << vec.z << "}";
    return stream;
}

Vec3s::Vec3s(int16_t xv, int16_t yv, int16_t zv) : x(xv), y(yv), z(zv) {}

int Vec3s::width() {
    auto wx = GetMagnitude(this->x);
    auto wy = GetMagnitude(this->y);
    auto wz = GetMagnitude(this->z);

    return std::max(wx, std::max(wy, wz));
}

std::ostream& operator<< (std::ostream& stream, const Vec3s& vec) {
    int width = stream.width();

    stream << std::setw(0) << "{" << std::setw(width) << vec.x << ", " << std::setw(width) << vec.y << ", " << std::setw(width) << vec.z << "}";
    return stream;
}

Vec3i::Vec3i(int32_t xv, int32_t yv, int32_t zv) : x(xv), y(yv), z(zv) {}

int Vec3i::width() {
    auto wx = GetMagnitude(this->x);
    auto wy = GetMagnitude(this->y);
    auto wz = GetMagnitude(this->z);

    return std::max(wx, std::max(wy, wz));
}

std::ostream& operator<< (std::ostream& stream, const Vec3i& vec) {
    int width = stream.width();

    stream << std::setw(0) << "{" << std::setw(width) << vec.x << ", " << std::setw(width) << vec.y << ", " << std::setw(width) << vec.z << "}";
    return stream;
}

Vec3iu::Vec3iu(uint32_t xv, uint32_t yv, uint32_t zv) : x(xv), y(yv), z(zv) {}

int Vec3iu::width() {
    auto wx = GetMagnitude(this->x);
    auto wy = GetMagnitude(this->y);
    auto wz = GetMagnitude(this->z);

    return std::max(wx, std::max(wy, wz));
}

std::ostream& operator<< (std::ostream& stream, const Vec3iu& vec) {
    int width = stream.width();

    stream << std::setw(0) << "{" << std::setw(width) << vec.x << ", " << std::setw(width) << vec.y << ", " << std::setw(width) << vec.z << "}";
    return stream;
}

Vec2f::Vec2f(float xv, float zv) : x(xv), z(zv) {}

int Vec2f::precision() {
    auto px = GetPrecision(this->x);
    auto pz = GetPrecision(this->z);

    return std::max(px, pz);
}

int Vec2f::width() {
    auto wx = GetMagnitude(this->x) + 1 + GetPrecision(this->x);
    auto wz = GetMagnitude(this->z) + 1 + GetPrecision(this->z);

    return std::max(wx,  wz);
}

std::ostream& operator<< (std::ostream& stream, const Vec2f& vec) {
    int width = stream.width();

    stream << std::setw(0) << "{" << std::setw(width) << vec.x << ", " << std::setw(width) << vec.z << "}";
    return stream;
}

Vec4f::Vec4f(float xv, float yv, float zv, float wv) : x(xv), y(yv), z(zv), w(wv) {}

int Vec4f::width() {
    auto wx = GetMagnitude(this->x) + 1 + GetPrecision(this->x);
    auto wy = GetMagnitude(this->y) + 1 + GetPrecision(this->y);
    auto wz = GetMagnitude(this->z) + 1 + GetPrecision(this->z);
    auto ww = GetMagnitude(this->w) + 1 + GetPrecision(this->w);

    return std::max(std::max(wy, ww), std::max(wx,  wz));
}

int Vec4f::precision() {
    auto px = GetPrecision(this->x);
    auto py = GetPrecision(this->y);
    auto pz = GetPrecision(this->z);
    auto pw = GetPrecision(this->w);

    return std::max(std::max(px, pw), std::max(py, pz));
}

std::ostream& operator<< (std::ostream& stream, const Vec4f& vec) {
    int width = stream.width();

    stream << std::setw(0) << "{" << std::setw(width) << vec.x << ", " << std::setw(width) << vec.y << ", " << std::setw(width) << vec.z << ", " << std::setw(width) << vec.w << "}";
    return stream;
}

Vec4s::Vec4s(int16_t xv, int16_t yv, int16_t zv, int16_t wv) : x(xv), y(yv), z(zv), w(wv) {}

int Vec4s::width() {
    auto wx = GetMagnitude(this->x);
    auto wy = GetMagnitude(this->y);
    auto wz = GetMagnitude(this->z);
    auto ww = GetMagnitude(this->w);

    return std::max(std::max(wx, ww), std::max(wy, wz));
}

std::ostream& operator<< (std::ostream& stream, const Vec4s& vec) {
    int width = stream.width();

    stream << std::setw(0) << "{" << std::setw(width) << vec.x << ", " << std::setw(width) << vec.y << ", " << std::setw(width) << vec.z << ", " << std::setw(width) << vec.w << "}";
    return stream;
}
