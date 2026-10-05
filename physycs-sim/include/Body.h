#pragma once

#include <SFML/Graphics.hpp>

#include "Vec2.h"

struct Body {
  Vec2 position;
  Vec2 velocity;
  Vec2 acceleration;
  float mass;
  float invMass;
  float radius;
  float restitution;
  sf::Color color;

  Body(Vec2 pos, float radius, float mass, sf::Color color);
  void applyForce(Vec2 f);
  void integrate(float dt);
};