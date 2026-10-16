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
    float r1 = std::get<CircleShape>(b1.shape).radius;
    float r2 = std::get<CircleShape>(b2.shape).radius;
    float radiusSum = r1 + r2;
    Collision::circleCircle(b1, b2, m);
    assert(m.valid && m.penetration == radiusSum);

    b2.position.x = 15;
    b2.position.y = 100;
    Collision::circleCircle(b1, b2, m);
    assert(m.normal.x > 0);

    b2.position.x = 5;
    b2.position.y = 100;
    Collision::circleCircle(b1, b2, m);
    assert(m.normal.x < 0);

    Body circle({100, 100}, 10, 5, sf::Color::White);
    Body box({500, 100}, {10, 10}, 5, sf::Color::White);
    Collision::circleBox(circle, box, m);
    assert(m.valid == false);

    box.position.x = 120;
    Collision::circleBox(circle, box, m);
    assert(m.valid == true && m.penetration < 1e-6);

    circle.position.x = 135;
    circle.position.y = 85;
    Collision::circleBox(circle, box, m);
    assert(m.normal.x > 0 && m.normal.y < 0);

    circle.position.x = 120;
    circle.position.y = 100;
    Collision::circleBox(circle, box, m);
    assert(m.penetration > 0);

    Body boxA({100, 100}, Vec2{20, 10}, 5, sf::Color::White);
    Body boxB({500, 100}, Vec2{20, 10}, 5, sf::Color::White);
    Collision::boxBox(boxA, boxB, m);
    assert(m.valid == false);

    boxA.position = {100, 100};
    boxB.position = {140, 100};
    Collision::boxBox(boxA, boxB, m);
    assert(m.valid == false);

    boxA.position = {100, 100};
    boxB.position = {130, 100};
    Collision::boxBox(boxA, boxB, m);
    assert(m.valid == true);
    assert(m.normal.x > 0 && fabs(m.normal.y) < 1e-6);
    assert(fabs(m.penetration - 10.f) < 1e-4);

    boxA.position = {100, 100};
    boxB.position = {100, 130};
    boxB.position = {100, 115};
    Collision::boxBox(boxA, boxB, m);
    assert(m.valid == true);
    assert(fabs(m.normal.x) < 1e-6 && m.normal.y > 0);
    assert(fabs(m.penetration - 5.f) < 1e-4);

    boxA.position = {100, 100};
    boxB.position = {70, 100};
    Collision::boxBox(boxA, boxB, m);
    assert(m.valid == true);
    assert(m.normal.x < 0 && fabs(m.normal.y) < 1e-6);

    boxA.position = {100, 100};
    boxB.position = {100, 85};
    Collision::boxBox(boxA, boxB, m);
    assert(m.valid == true);
    assert(fabs(m.normal.x) < 1e-6 && m.normal.y < 0);

    boxA.position = {100, 100};
    boxB.position = {125, 112};
    Collision::boxBox(boxA, boxB, m);
    assert(m.valid == true);
    assert(fabs(m.normal.length() - 1.f) < 1e-4);

    boxA.position = {100, 100};
    boxB.position = {100, 100};
    Collision::boxBox(boxA, boxB, m);
    assert(m.valid == true);
    assert(m.normal == Vec2({0, 1}));
    assert(fabs(m.penetration - 20.f) < 1e-4);

    Body boxC({100, 100}, Vec2{30, 15}, 5, sf::Color::White);
    boxA.position = {100, 100};
    Collision::boxBox(boxA, boxC, m);
    assert(m.valid == true);
    assert(m.normal == Vec2({0, 1}));
    assert(fabs(m.penetration - 25.f) < 1e-4);
}