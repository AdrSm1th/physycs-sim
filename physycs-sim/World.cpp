#include "World.h"

void World::addBody(const Body& b) { bodies.push_back(b); }

void World::step(float dt) {
  for (Body &b : bodies) {
    b.acceleration += gravity;
    b.integrate(dt);
  }
}