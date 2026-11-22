#pragma once

#include <vector>

#include "Body.h"
#include "Manifold.h"

struct World {
    std::vector<Body> bodies;
    Vec2 gravity;
    std::vector<Manifold> contacts;
    int selectedBodyIndex = -1;

    void addBody(const Body& b);
    void step(float dt);
    void reset();
    void setupScene();
    void resolveContact(Manifold& contact);
    float kineticEnergy() const;
    bool containsPoint(int bodyIdx, Vec2 point) const;

    private:
    void stepInternal(float dt);
};