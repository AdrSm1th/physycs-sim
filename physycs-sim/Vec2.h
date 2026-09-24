#pragma once

#include <algorithm>

class Vec2 {
 public:
  Vec2(float _x, float _y) : x(_x), y(_y) {}
  Vec2 operator+(const Vec2 &other) const {
    return Vec2(x + other.x, y + other.y);
  }
  Vec2 &operator+=(const Vec2 &other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  Vec2 operator-(const Vec2 &other) const {
    return Vec2(x - other.x, y - other.y);
  }
  Vec2 &operator-=(const Vec2 &other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }
  Vec2 operator-() const { return Vec2(-x, -y); }

  Vec2 operator*(float c) const { return Vec2(x * c, y * c); }
  Vec2 &operator*=(float c) {
    x *= c;
    y *= c;
    return *this;
  }

  Vec2 operator/(float c) const { return Vec2(x / c, y / c); }
  Vec2 &operator/=(float c) {
    x /= c;
    y /= c;
    return *this;
  }

  bool operator==(const Vec2 &other) const {
    constexpr float eps = 1e-5f;
    float scale = std::max({1.f, x, y, other.x, other.y});
    float tol = eps * scale;
    return fabs(x - other.x) <= tol && fabs(y - other.y) <= tol;
  }
  bool operator!=(const Vec2 &o) const { return !(*this == o); }

  float dot(const Vec2 other) const;
  float length() const;
  float lengthSquared() const;
  Vec2 normalized() const;
  void normalize();
  void perpendicular();

 private:
  float x;
  float y;
};