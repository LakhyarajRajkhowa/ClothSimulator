#pragma once


struct Vec2 {
    float x, y;
    Vec2 operator+(const Vec2& o) const { return { x + o.x, y + o.y }; }
    Vec2 operator-(const Vec2& o) const { return { x - o.x, y - o.y }; }
    Vec2 operator*(float s) const { return { x * s, y * s }; }
    Vec2 operator/(float s) const { return { x / s, y / s }; }
};


inline float length(const Vec2& v) { return sqrt(v.x * v.x + v.y * v.y); }
inline Vec2 normalize(const Vec2& v) { float l = length(v); return (l > 0) ? v / l : Vec2{ 0,0 }; }