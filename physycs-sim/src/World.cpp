#include "World.h"

void World::addBody(const Body& b) { bodies.push_back(b); }

void World::step(float dt) {
    for (Body& b : bodies) {
        b.acceleration += gravity;
        b.integrate(dt);
    }
}

void World::reset() { setupScene(); }

void World::setupScene() {
    bodies.clear();
    bodies.emplace_back(Vec2{500, 100}, 50, 5, sf::Color::White);
    bodies.emplace_back(Vec2{1000, 100}, 80, 200, sf::Color::Yellow);
}