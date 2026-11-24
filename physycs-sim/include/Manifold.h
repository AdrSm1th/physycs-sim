#pragma once

#include "Vec2.h"

struct ContactPoint {
    Vec2 position{};
    float penetration = 0.0f;

    float normalImpulse = 0.0f;
    float tangentImpulse = 0.0f;
    float targetNormalVelocity = 0.0f;

    ContactPoint(Vec2 pointPos) : position(pointPos) {}
};

struct Manifold {
    // Индексы в World::bodies
    int a = -1;
    int b = -1;

    Vec2 normal;
    Vec2 tangent = {0.0f, 0.0f};
    std::vector<ContactPoint> points;
    bool valid = false;

    void computeTangent() {
        tangent.x = -normal.y;
        tangent.y = normal.x;
    }
};