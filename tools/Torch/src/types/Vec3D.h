#pragma once

#include "../factories/BaseFactory.h"

class Vec3f {
public:
    float x;
    float y;
    float z;

    Vec3f(float xv = 0, float yv = 0, float zv = 0);
    int width();
    int precision();
    friend std::ostream& operator<< (std::ostream& stream, const Vec3f& vec);
};

class Vec3s {
public:
    int16_t x;
    int16_t y;
    int16_t z;

    Vec3s(int16_t xv = 0, int16_t yv = 0, int16_t zv = 0);
    int width();
    friend std::ostream& operator<< (std::ostream& stream, const Vec3s& vec);
};

class Vec3i {
public:
    int32_t x;
    int32_t y;
    int32_t z;

    Vec3i(int32_t xv = 0, int32_t yv = 0, int32_t zv = 0);
    int width();
    friend std::ostream& operator<< (std::ostream& stream, const Vec3i& vec);
};

class Vec3iu {
public:
    uint32_t x;
    uint32_t y;
    uint32_t z;

    Vec3iu(uint32_t xv = 0, uint32_t yv = 0, uint32_t zv = 0);
    int width();
    friend std::ostream& operator<< (std::ostream& stream, const Vec3iu& vec);
};

class Vec2f {
public:
    float x;
    float z;

    Vec2f(float xv = 0, float zv = 0);
    int precision();
    int width();
    friend std::ostream& operator<< (std::ostream& stream, const Vec2f& vec);
};

class Vec4f {
public:
    float x;
    float y;
    float z;
    float w;

    Vec4f(float xv = 0, float yv = 0, float zv = 0, float wv = 0);
    int precision();
    int width();
    friend std::ostream& operator<< (std::ostream& stream, const Vec4f& vec);
};

class Vec4s {
public:
    int16_t x;
    int16_t y;
    int16_t z;
    int16_t w;

    Vec4s(int16_t xv = 0, int16_t yv = 0, int16_t zv = 0, int16_t wv = 0);
    int width();
    friend std::ostream& operator<< (std::ostream& stream, const Vec4s& vec);
};