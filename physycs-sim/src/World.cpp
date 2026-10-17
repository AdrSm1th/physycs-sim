#include "World.h"

#include <algorithm>
#include <variant>

#include "Body.h"
#include "Collision.h"

constexpr float SLOP = 0.01f;  // защита от дрожания (когда тело будет стоять на
                               // полу, без этого параметра будет дрожание)
constexpr float CORRECTION_PERCENT =
    0.8f;  // С этим параметром выталкивание будет более плавным. Нужно для
           // столкновения большого количества тел, чтобы не создавать лишних
           // коллизий

constexpr int ITERATIONS = 10;  // Для корректного разрешения столкновения
                                // цепочки тел нужно несколько итераций

void World::addBody(const Body& b) { bodies.push_back(b); }

void World::step(float dt) {
    for (Body& b : bodies) {
        b.applyForce(gravity * b.mass);
        b.integrate(dt);
    }

    contacts.clear();
    for (int i = 0; i < bodies.size(); i++) {
        for (int j = i + 1; j < bodies.size(); j++) {
            Manifold m;
            auto* ca = std::get_if<CircleShape>(&bodies[i].shape);
            auto* cb = std::get_if<CircleShape>(&bodies[j].shape);
            auto* ba = std::get_if<BoxShape>(&bodies[i].shape);
            auto* bb = std::get_if<BoxShape>(&bodies[j].shape);
            if (ca && cb && Collision::circleCircle(bodies[i], bodies[j], m)) {
                m.a = i;
                m.b = j;
                contacts.push_back(m);
            } else if ((ca && bb || cb && ba) &&
                           Collision::circleBox(bodies[i], bodies[j], m)) {
                m.a = i;
                m.b = j;
                contacts.push_back(m);
            } else if (ba && bb && Collision::boxBox(bodies[i], bodies[j], m)) {
                m.a = i;
                m.b = j;
                contacts.push_back(m);
            }
        }
    }

    for (int i = 0; i < ITERATIONS; i++) {
        for (const Manifold& contact : contacts) {
            resolveContact(contact);
        }
        // resolveContact(contact);
    }

    for (const Manifold& contact : contacts) {
        Body& a = bodies[contact.a];
        Body& b = bodies[contact.b];

        float invMassSum = a.invMass + b.invMass;
        // Если оба тела статические, то дальнейшая обработка не имеет смысла
        if (invMassSum == 0) {
            continue;
        }

        // correction - защита от дрожания и лишних коллизий при групповом
        // столкновении
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
    bodies.emplace_back(Vec2{640, 710}, Vec2{620, 20}, 0, sf::Color::Yellow);
    bodies.emplace_back(Vec2{640, 0}, Vec2{620, 20}, 0, sf::Color::Yellow);
    bodies.emplace_back(Vec2{0, 360}, Vec2{20, 360}, 0, sf::Color::Yellow);
    bodies.emplace_back(Vec2{1280, 360}, Vec2{20, 360}, 0, sf::Color::Yellow);
    bodies.emplace_back(Vec2{640, 360}, Vec2{100, 100}, 0, sf::Color::Yellow);
    bodies.emplace_back(Vec2{840, 440}, Vec2{100, 20}, 0, sf::Color::Yellow);
    bodies.emplace_back(Vec2{100, 100}, 50, 5, sf::Color::White);
    bodies.emplace_back(Vec2{300, 100}, 50, 500, sf::Color::White);
}

void World::resolveContact(const Manifold& contact) {
    Body& a = bodies[contact.a];
    Body& b = bodies[contact.b];
    float vRel = contact.normal.dot(b.velocity - a.velocity);

    if (vRel >= 0) {
        return;
    }

    // Выбор минимальной упругости
    float restitution = std::min(a.restitution, b.restitution);

    float pulse = -(1 + restitution) * vRel / (a.invMass + b.invMass);

    a.velocity -= contact.normal * (pulse * a.invMass);
    b.velocity += contact.normal * (pulse * b.invMass);
}

float World::kineticEnergy() const {
    float kineticEnergy = 0;
    for (const Body& body : bodies) {
        if (body.invMass > 0) {
            kineticEnergy += 0.5 * body.velocity.lengthSquared() * body.mass;
        }
    }
    return kineticEnergy;
}