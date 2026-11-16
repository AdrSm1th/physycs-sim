#pragma once

#include "Vec2.h"

struct Manifold {
    // Индексы в World::bodies
    int a = -1;
    int b = -1;

    float penetration = 0;
    Vec2 normal;
    Vec2 tangent = {0.0f, 0.0f};
    Vec2 contactPoint;
    bool valid = false;

    float normalImpulse = 0.0f;
    float tangentImpulse = 0.0f;
    float targetNormalVelocity = 0.0f;

    void computeTangent() {
        tangent.x = -normal.y;
        tangent.y = normal.x;
    }
};