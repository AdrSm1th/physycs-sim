#pragma once

#include <string.h>

#include <SFML/Graphics.hpp>

#include "Vec2.h"

class Renderer {
 public:
  Renderer(unsigned int width, unsigned int height, const std::string& title,
           unsigned int fpsLimit);
  bool isOpen();
  void pollEvent();
  void beginFrame();
  void drawCircle(Vec2 pos, float radius, sf::Color color);
  void endFrame();
  void drawFPS(float fps);

 private:
  sf::RenderWindow window;
  sf::Font font;
  sf::Text fpsText;
};
