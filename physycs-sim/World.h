#pragma once

#include <vector>

#include "Body.h"

struct World {
  std::vector<Body> bodies;

  void addBody(const Body& b);
  void step(float dt);
};