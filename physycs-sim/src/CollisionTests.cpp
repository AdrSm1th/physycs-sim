#include "CollisionTests.h"
#include "Collision.h"
#include "World.h"

#include <cassert>
#include <cmath>

void testCollision() {
    Body b1({100, 100}, 10, 5, sf::Color::White);
    Body b2({500, 100}, 10, 5, sf::Color::White);

    Manifold m;
    Collision::circleCircle(b1, b2, m);
    assert(m.valid == false);

    b1.position.x = 10;
    b2.position.x = 30;
    Collision::circleCircle(b1, b2, m);
    assert(m.valid == false);

    b2.position.x = 20;
    b2.position.y = 102;
    Collision::circleCircle(b1, b2, m);
    assert(m.valid == true && fabs(m.normal.length() - 1) < 1e-6 &&
           m.penetration > 0);

    b2.position.x = 10;
    b2.position.y = 100;
    Collision::circleCircle(b1, b2, m);
    assert(m.valid == true && m.penetration == b1.radius + b2.radius);

    b2.position.x = 15;
    b2.position.y = 100;
    Collision::circleCircle(b1, b2, m);
    assert(m.normal.x > 0);

    b2.position.x = 5;
    b2.position.y = 100;
    Collision::circleCircle(b1, b2, m);
    assert(m.normal.x < 0);
}