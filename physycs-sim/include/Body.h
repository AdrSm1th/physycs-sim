#pragma once

#include <SFML/Graphics.hpp>

#include "Vec2.h"

#include <variant>

struct CircleShape {
    float radius;
};

struct BoxShape {
    Vec2 halfExtents;
};

using Shape = std::variant<CircleShape, BoxShape>;

struct Body {
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    float mass;
    float invMass;
    Shape shape;
    float restitution;
    sf::Color color;

    Body(Vec2 pos, float radius, float mass, sf::Color color);
    Body(Vec2 pos, Vec2 halfExtents, float mass, sf::Color color);
    void applyForce(Vec2 f);
    void integrate(float dt);
};