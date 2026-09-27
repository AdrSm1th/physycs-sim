#include "../include/Body.h"

Body::Body(Vec2 pos, float radius, float mass) {
  position = pos;
  this->radius = radius;
  this->mass = mass;
  invMass = mass > 0.0 ? 1.0f / mass : 0.0;
  velocity = Vec2(0, 0);
  acceleration = Vec2(0, 0);
  restitution = 0.5;         // ”пругость по умолчанию
  color = sf::Color::White;  // ÷вет по умолчанию
}

void Body::applyForce(Vec2 f) { acceleration += f * invMass; }

void Body::integrate(float dt) { 
  velocity += acceleration * dt;
  position += velocity * dt;
  acceleration = {0, 0};
}