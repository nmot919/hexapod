#include <cmath>
#define VEC_H
// used for vectors not dependent on cinder when compiling for microcontroller

struct Vec3 {
    float x, y, z;

    Vec3(float x=0, float y=0, float z=0) : x(x), y(y), z(z){}

    Vec3 operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    Vec3 operator*(float s)       const { return {x*s,   y*s,   z*s};   }
    Vec3 operator*(const Vec3& o) const { return {x*o.x, y*o.y, z*o.z}; }
    bool operator==(const Vec3& o) const { return x==o.x && y==o.y && z==o.z; }
    bool operator!=(const Vec3& o) const { return !(*this == o); }

    float length() const { return sqrt(x*x + y*y + z*z); }

    Vec3 normalize() const {
        float l = length();
        if(l < 0.0001f) return {0,0,0};
        return {x/l, y/l, z/l};
    }
};

inline Vec3 mix(Vec3 a, Vec3 b, float t){ return a + (b - a) * t; }
inline float clamp(float v, float lo, float hi){ return v < lo ? lo : v > hi ? hi : v; }
inline float length(const Vec3& v){ return v.length(); }
inline Vec3 normalize(const Vec3& v){ return v.normalize(); }


inline float toRadians(float d){ return d * 0.01745329251f; }
inline float toDegrees(float r){ return r * 57.2957795131f; }
