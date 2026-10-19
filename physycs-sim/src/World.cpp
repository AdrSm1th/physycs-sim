#include "World.h"

#include <algorithm>
#include <variant>

#include "Body.h"
#include "Collision.h"

constexpr float SLOP = 0.5f;  // защита от дрожани€ (когда тело будет сто€ть на
                               // полу, без этого параметра будет дрожание)
constexpr float CORRECTION_PERCENT =
    0.3f;  // — этим параметром выталкивание будет более плавным. Ќужно дл€
           // столкновени€ большого количества тел, чтобы не создавать лишних
           // коллизий

constexpr int ITERATIONS = 10;  // ƒл€ корректного разрешени€ столкновени€
                                // цепочки тел нужно несколько итераций

constexpr float MIN_FEATURE_SIZE = 10.0f;
constexpr int MAX_SUBSTEPS = 30;

void World::addBody(const Body& b) { bodies.push_back(b); }

void World::step(float dt) {
    float maxSpeed = 0.0f;
    for (Body& b : bodies) {
        maxSpeed = std::max(maxSpeed, b.velocity.length());
    }

    float maxPath = maxSpeed * dt;
    int k = 1;
    if (maxPath > MIN_FEATURE_SIZE) {
        k = static_cast<int>(ceil(maxPath / MIN_FEATURE_SIZE));
        k = std::min(k, MAX_SUBSTEPS);
    }

    float subDt = dt / static_cast<float>(k);
    for (int i = 0; i < k; i++) {
        stepInternal(subDt);
    }
}

void World::stepInternal(float dt) {
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
        // ≈сли оба тела статические, то дальнейша€ обработка не имеет смысла
        if (invMassSum == 0) {
            continue;
        }

        // correction - защита от дрожани€ и лишних коллизий при групповом
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
    bodies.emplace_back(Vec2{640, 710}, Vec2{620, 20}, 0,
                        sf::Color(80, 80, 80));
    bodies.emplace_back(Vec2{640, 10}, Vec2{620, 20}, 0, sf::Color(80, 80, 80));
    bodies.emplace_back(Vec2{10, 360}, Vec2{20, 360}, 0, sf::Color(80, 80, 80));
    bodies.emplace_back(Vec2{1270, 360}, Vec2{20, 360}, 0,
                        sf::Color(80, 80, 80));

    // “онка€ стена дл€ теста туннелировани€ (6 px толщиной)
    bodies.emplace_back(Vec2{800, 360}, Vec2{3, 150}, 0,
                        sf::Color(150, 150, 150));

    const float R = 25.f;
    const float rowH = R * 1.7320508f;

    // “ест 1: пирамида из 10 шаров
    //{
    //    const float baseY = 665.f;
    //    const float baseX = 400.f;
    //    for (int row = 0; row < 4; row++) {
    //        int count = 4 - row;
    //        float y = baseY - row * rowH;
    //        float x = baseX + row * R;
    //        for (int i = 0; i < count; i++) {
    //            Body ball(Vec2{x, y}, R, 5.f, sf::Color(220, 220, 255));
    //            ball.restitution = 0.0f;
    //            bodies.push_back(ball);
    //        }
    //    }
    //}

    // “ест 2 : вертикальна€ колонна из 5 шаров

    //for (int i = 0; i < 5; i++) {
    //    bodies.emplace_back(Vec2{950.f, 665.f - 50.f * i}, R, 5.f,
    //                        sf::Color(100, 200, 255));
    //}

    // “ест 3: быстро лет€щие тела
    {
        Body fastA(Vec2{230, 300}, 15, 1, sf::Color::Red);
        fastA.velocity = {5000.f, 0.f};
        bodies.push_back(fastA);
        Body fastB(Vec2{250, 420}, 15, 1, sf::Color(255, 100, 100));
        fastB.velocity = {5000.f, 0.f};
        bodies.push_back(fastB);
    }

    // “ест 4: эталон Ч свободное падение ---
    //bodies.emplace_back(Vec2{1150, 100}, 20, 5, sf::Color(255, 200, 100));
}

void World::resolveContact(const Manifold& contact) {
    Body& a = bodies[contact.a];
    Body& b = bodies[contact.b];
    float vRel = contact.normal.dot(b.velocity - a.velocity);

    if (vRel >= 0) {
        return;
    }

    // ¬ыбор минимальной упругости
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