#include "Body.h"

void Body::Init(Vec2 pos, Vec2 v, Vec2 a, float m, Shape s, float e, float nu,
                float theta, float w, sf::Color col) {
    auto *c = std::get_if<CircleShape>(&s);
    auto *b = std::get_if<BoxShape>(&s);

    position = pos;
    velocity = v;
    acceleration = a;
    mass = m;
    invMass = m != 0 ? 1 / mass : 0;
    shape = s;
    restitution = e;
    friction = nu;
    angleOfRotation = theta;
    angularVelocity = w;
    color = col;

    if (c) {
        momentOfInertia = 0.5 * mass * c->radius * c->radius;
    } else if (b) {
        momentOfInertia = (mass / 3) * (b->halfExtents.x * b->halfExtents.x +
                                        b->halfExtents.y * b->halfExtents.y);
    }

    invMomentOfInertia = momentOfInertia != 0 ? 1 / momentOfInertia : 0;
}

Body::Body() {
    Init({0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, 1.0f, CircleShape(1.0f),
         0.5f, 0.5f, 0.0f, 0.0f, sf::Color::White);
}

Body::Body(Vec2 pos, float radius, float mass, sf::Color color) {
    Init(pos, {0.0f, 0.0f}, {0.0f, 0.0f}, mass, CircleShape(radius), 0.5f, 0.5f,
         0.0f, 0.0f, color);
}

Body::Body(Vec2 pos, Vec2 halfExtents, float mass, sf::Color color) {
    Init(pos, {0.0f, 0.0f}, {0.0f, 0.0f}, mass, BoxShape(halfExtents), 0.5f,
         0.5f, 0.0f, 0.0f, color);
}

Body::Body(Vec2 pos, float radius, float mass, float friction,
           sf::Color color) {
    Init(pos, {0.0f, 0.0f}, {0.0f, 0.0f}, mass, CircleShape(radius), 0.5f,
         friction, 0.0f, 0.0f, color);
}

Body::Body(Vec2 pos, Vec2 halfExtents, float mass, float friction,
           sf::Color color) {
    Init(pos, {0.0f, 0.0f}, {0.0f, 0.0f}, mass, BoxShape(halfExtents), 0.5f,
         friction, 0.0f, 0.0f, color);
}

void Body::applyForce(Vec2 f) { acceleration += f * invMass; }

void Body::integrate(float dt, Vec2 gravity) {
    if (invMass == 0.0f) {
        return;
    }

    velocity += (acceleration + gravity) * dt;
    position += velocity * dt;

    angleOfRotation += angularVelocity * dt;
}

void Body::setMass(float newMass) {
    mass = newMass;
    invMass = mass > 0.0f ? 1.0f / mass : 0.0f;

    auto *c = std::get_if<CircleShape>(&shape);
    auto *b = std::get_if<BoxShape>(&shape);

    if (c) {
        momentOfInertia = 0.5 * mass * c->radius * c->radius;
    } else if (b) {
        momentOfInertia = (mass / 3) * (b->halfExtents.x * b->halfExtents.x +
                                        b->halfExtents.y * b->halfExtents.y);
    }

    invMomentOfInertia = momentOfInertia != 0 ? 1 / momentOfInertia : 0;
}