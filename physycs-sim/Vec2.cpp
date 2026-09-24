#include "Vec2.h"

float Vec2::dot(Vec2 const other) const { return x * other.x + y * other.y; }

float Vec2::length() const { return sqrt(x * x + y * y); }

float Vec2::lengthSquared() const { return x * x + y * y; }

Vec2 Vec2::normalized() const {
  float len = length();
  if (len < 1e-8f) return *this;
  return Vec2(x / len, y / len);
}

void Vec2::normalize() {
  float len = length();
  if (len < 1e-8f) {
    return;
  }
  x /= len;
  y /= len;
}

void Vec2::perpendicular() {
  float temp = x;
  x = -y;
  y = temp;
}