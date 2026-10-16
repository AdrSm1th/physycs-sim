#include "Collision.h"

#include <algorithm>
#include <cassert>
#include <variant>

namespace Collision {
bool circleCircle(const Body& a, const Body& b, Manifold& m) {
    auto* ra = std::get_if<CircleShape>(&a.shape);
    auto* rb = std::get_if<CircleShape>(&b.shape);
    assert(ra && rb && "circleCircle called with non-circle body");

    Vec2 delta = b.position - a.position;

    float distSq = delta.lengthSquared();

    float radiusSum = ra->radius + rb->radius;

    if (distSq >= radiusSum * radiusSum) {
        m.valid = false;
        return false;
    }

    float dist = sqrt(distSq);

    if (dist < 1e-6) {
        m.normal = {0, 1};
    } else {
        m.normal = delta * (1 / dist);
    }

    m.penetration = radiusSum - dist;

    m.valid = true;
    return true;
}

bool circleBoxImpl(Vec2 posA, float radius, Vec2 posB, Vec2 halfExterns,
                   Manifold& m) {
    Vec2 closest;
    closest.x =
        std::clamp(posA.x, posB.x - halfExterns.x, posB.x + halfExterns.x);
    closest.y =
        std::clamp(posA.y, posB.y - halfExterns.y, posB.y + halfExterns.y);

    Vec2 delta = closest - posA;

    float distSq = delta.lengthSquared();

    if (distSq > radius * radius) {
        m.valid = false;
        return false;
    }

    float dist = sqrt(distSq);

    if (dist < 1e-6) {
        float distLeft = posA.x - (posB.x - halfExterns.x);
        float distRight = (posB.x + halfExterns.x) - posA.x;
        float distUp = (posB.y + halfExterns.y) - posA.y;
        float distDown = posA.y - (posB.y - halfExterns.y);

        auto minDist = std::min({distLeft, distRight, distUp, distDown});

        if (minDist == distLeft) {
            m.normal = {1.0f, 0.0f};
        } else if (minDist == distRight) {
            m.normal = {-1.0f, 0.0f};
        } else if (minDist == distUp) {
            m.normal = {0.0f, -1.0f};
        } else {
            m.normal = {0.0, 1.0f};
        }

        m.penetration = radius + minDist;
    } else {
        m.normal = delta * (1 / dist);
        m.penetration = radius - dist;
    }

    m.valid = true;

    return true;
}

bool circleBox(const Body& a, const Body& b, Manifold& m) {
    auto* ca = std::get_if<CircleShape>(&a.shape);
    auto* cb = std::get_if<CircleShape>(&b.shape);
    auto* ba = std::get_if<BoxShape>(&a.shape);
    auto* bb = std::get_if<BoxShape>(&b.shape);

    if (ca && bb) {
        // a - круг, b — бокс. Нормаль смотрит от a к b.
        return circleBoxImpl(a.position, ca->radius, b.position,
                             bb->halfExtents, m);
    }
    if (ba && cb) {
        bool hit = circleBoxImpl(b.position, cb->radius, a.position,
                                 ba->halfExtents, m);
        if (hit) m.normal = -m.normal;
        return hit;
    }
    return false;
}

bool boxBox(const Body& a, const Body& b, Manifold& m) {
    auto* ca = std::get_if<BoxShape>(&a.shape);
    auto* cb = std::get_if<BoxShape>(&b.shape);

    assert(ca && cb && "box_box called with non-box body");

    float overlapX = (ca->halfExtents.x + cb->halfExtents.x) -
                     fabs(a.position.x - b.position.x);
    float overlapY = (ca->halfExtents.y + cb->halfExtents.y) -
                     fabs(a.position.y - b.position.y);

    if (overlapX <= 0 || overlapY < 0) {
        m.valid = false;
        return false;
    }

    if (overlapX < overlapY) {
        m.normal = b.position.x - a.position.x >= 0 ? Vec2{1.0f, 0.0f}
                                                   : Vec2{-1.0f, 0.0f};
        m.penetration = overlapX;
    } else if (overlapX > overlapY) {
        m.normal = b.position.y - a.position.y >= 0 ? Vec2{0.0f, 1.0f}
                                                   : Vec2{0.0f, -1.0f};
        m.penetration = overlapY;
    } else {
        m.normal = {1.0f, 0.0f};
        m.penetration = std::min(overlapY, overlapY);
    }

    m.valid = true;
    return true;
}
}  // namespace Collision
