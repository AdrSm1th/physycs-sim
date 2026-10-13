#pragma once

#include <vector>

#include "Body.h"
#include "Manifold.h"

struct World {
    std::vector<Body> bodies;
    Vec2 gravity;
    std::vector<Manifold> contacts;

    void addBody(const Body& b);
    void step(float dt);
    void reset();
    void setupScene();
    void resolveContact(const Manifold& contact);
};