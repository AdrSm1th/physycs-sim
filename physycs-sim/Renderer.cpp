#include "Renderer.h"

Renderer::Renderer(unsigned int width, unsigned int height,
                   const std::string &title, unsigned int fpsLimit)
    : font(),
      window(sf::VideoMode({width, height}), title),
      fpsText(font, "FPS: 0", 18) {
  window.setFramerateLimit(fpsLimit);

  if (!font.openFromFile("assets/Insight Sans SSi.ttf")) {
    throw std::runtime_error("Font not loaded");
  }

  fpsText.setFillColor(sf::Color::White);
  fpsText.setPosition({10.f, 10.f});
}

bool Renderer::isOpen() { return window.isOpen(); }

void Renderer::pollEvent() {
  while (const std::optional event = window.pollEvent()) {
    if (event->is<sf::Event::Closed>()) {
      window.close();
    }
  }
}

void Renderer::beginFrame() { window.clear(sf::Color::Black); }

void Renderer::drawCircle(float x, float y, float radius, sf::Color color) {
  sf::CircleShape shape(radius);
  shape.setOrigin(
      {radius, radius});      // ”становка позиции отрисовки в центр круга
  shape.setPosition({x, y});  // ѕозици€ круга в глобальных координатах
  shape.setFillColor(color);
  window.draw(shape);
}

void Renderer::endFrame() { window.display(); }

void Renderer::drawFPS(float fps) {
  fpsText.setString("FPS: " + std::to_string(static_cast<int>(fps)));
  window.draw(fpsText);
}