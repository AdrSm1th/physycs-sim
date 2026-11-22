#pragma once

#include <SFML/Graphics.hpp>
#include <variant>

#include "Vec2.h"

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
    float friction;
    float angleOfRotation;
    float angularVelocity;
    float momentOfInertia;
    float invMomentOfInertia;

    sf::Color color;

    Body();
    Body(Vec2 pos, float radius, float mass, sf::Color color);
    Body(Vec2 pos, Vec2 halfExtents, float mass, sf::Color color);
    Body(Vec2 pos, float radius, float mass, float friction, sf::Color color);
    Body(Vec2 pos, Vec2 halfExtents, float mass, float friction,
         sf::Color color);
    void applyForce(Vec2 f);
    void integrate(float dt, Vec2 gravity = {});
    void setMass(float newMass);

    private:
    void Init(Vec2 pos, Vec2 v, Vec2 a, float m, Shape s, float e, float nu, float theta, float w, sf::Color col);
};