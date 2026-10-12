#pragma once

#include "Vec2.h"

struct Manifold {
    // Индексы в World::bodies
    int a = -1;
    int b = -1;

    Vec2 normal;
    float penetration = 0;
    bool valid = false;
};