#include "World.h"

#include <algorithm>

#include "Collision.h"

constexpr float SLOP = 0.01f;  // защита от дрожания (когда тело будет стоять на
                               // полу, без этого параметра будет дрожание)
constexpr float CORRECTION_PERCENT =
    0.8f;  // С этим параметром выталкивание будет более плавным. Нужно для
           // столкновения большого количества тел, чтобы не создавать лишних
           // коллизий

void World::addBody(const Body& b) { bodies.push_back(b); }

void World::step(float dt) {
    for (Body& b : bodies) {
        b.acceleration += gravity;
        b.integrate(dt);
    }

    contacts.clear();
    for (int i = 0; i < bodies.size(); i++) {
        for (int j = i + 1; j < bodies.size(); j++) {
            Manifold m;
            if (Collision::circleCircle(bodies[i], bodies[j], m)) {
                m.a = i;
                m.b = j;
                contacts.push_back(m);
            }
        }
    }

    for (const Manifold& contact : contacts) {
        Body& a = bodies[contact.a];
        Body& b = bodies[contact.b];

        float invMassSum = a.invMass + b.invMass;
        // Если оба тела статические, то дальнейшая обработка не имеет смысла
        if (invMassSum == 0) {
            continue;
        }

        // correction - защита от дрожания и лишних коллизий при групповом столкновении
        float correction =
            std::max(contact.penetration - SLOP, 0.0f) * CORRECTION_PERCENT;
        correction /= invMassSum;

        a.position -= contact.normal * correction * a.invMass;
        b.position += contact.normal * correction * b.invMass;
    }
}

void World::reset() { setupScene(); }

void World::setupScene() {
    bodies.clear();
    bodies.emplace_back(Vec2{500, 100}, 50, 5, sf::Color::White);
    bodies.emplace_back(Vec2{1000, 100}, 80, 200, sf::Color::Yellow);
}