#include <SFML/Graphics.hpp>

#include "Renderer.h"
#include "Vec2.h"
#include "test.h"
#include "Body.h"
#include "World.h"

int WinMain() {
  Renderer renderer(1280, 720, "physics simulator", 60);
  sf::Clock clock;
  float smoothedFps = 0;

  testVec2();

  World world;
  world.gravity = {0, 500};
  Body body_1({500, 100}, 100, 5);
  world.addBody(body_1);

  while (renderer.isOpen()) {
    renderer.pollEvent();
    float frameDt = clock.restart().asSeconds();
    if (frameDt > 0.25f) frameDt = 0.25f;

    float instantFps = 1.f / frameDt;
    smoothedFps = smoothedFps * 0.9f + instantFps * 0.1f;

    world.step(frameDt);

    renderer.beginFrame();
    renderer.drawFPS(smoothedFps);
    for (Body &body : world.bodies) {
      renderer.drawCircle(body.position, body.radius, body.color);
    }
    renderer.endFrame();
  }

  return 0;
}