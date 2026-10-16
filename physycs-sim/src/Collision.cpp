#include "Collision.h"

#include <variant>
#include <cassert>

namespace Collision {
bool circleCircle(const Body &a, const Body &b, Manifold &m) {
    auto *ra = std::get_if<CircleShape>(&a.shape);
    auto *rb = std::get_if<CircleShape>(&b.shape);
    assert(ra && rb && "circleCircle called with non-circle body");

    Vec2 delta = b.position - a.position;

    float distSq = delta.lengthSquared();

    float radiusSum = ra->radius + rb->radius;

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