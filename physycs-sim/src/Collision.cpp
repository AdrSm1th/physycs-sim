#include "Collision.h"

namespace Collision {
bool circleCircle(const Body &a, const Body &b, Manifold &m) {
    Vec2 delta = b.position - a.position;

    float distSq = delta.lengthSquared();

    float radiusSum = a.radius + b.radius;
    if (distSq >= radiusSum * radiusSum) {
        m.valid = false;
        return false;
    }

    float dist = sqrt(distSq);

    Vec2 normal;
    if (dist < 1e-6) {
        normal = {0, 1};
    } else {
        normal = delta * (1 / dist);
    }

    m.penetration = radiusSum - dist;
    m.normal = normal;

    m.valid = true;
    return true;
}
}  // namespace Collision