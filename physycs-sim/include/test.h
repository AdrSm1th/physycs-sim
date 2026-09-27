#pragma once

#include <math.h>

#include <cassert>

#include "Vec2.h"

void testVec2() {
  Vec2 a(3.f, 4.f), b(1.f, 2.f);

  assert((a + b) == Vec2({4.f, 6.f}));
  assert((a - b) == Vec2({2.f, 2.f}));
  assert((a * 2.f) == Vec2({6.f, 8.f}));
  assert((a / 2.f) == Vec2({1.5f, 2.f}));
  assert((-a) == Vec2({-3.f, -4.f}));

  assert(a.dot(b) == 3.f * 1.f + 4.f * 2.f);  // 11
  assert(a.lengthSquared() == 25.f);
  assert(fabs(a.length() - 5.f) < 1e-5f);

  auto n = a.normalized();
  assert(fabs(n.length() - 1.f) < 1e-5f);
  assert(n == Vec2({0.6f, 0.8f}));

  Vec2 z(0.f, 0.f);
  assert(z.normalized() == Vec2({0.f, 0.f}));  // нет NaN!

  Vec2 orig(3.f, 4.f);
  Vec2 p = orig;
  p.perpendicular();
  assert(p == Vec2(-4.f, 3.f));
  assert(fabs(p.dot(orig)) < 1e-5f);

  Vec2 c = a;
  c.normalize();
  assert(c == Vec2(n));  // in-place == copy
}