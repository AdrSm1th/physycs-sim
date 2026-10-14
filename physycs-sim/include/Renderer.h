#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "Vec2.h"

struct Input {
    bool toggleDebug = false;
    bool togglePause = false;
    bool reset = false;
    bool stepOnce = false;
    bool spawn = false;
};

class Renderer {
   public:
    Renderer(unsigned int width, unsigned int height, const std::string &title,
             unsigned int fpsLimit);
    bool isOpen();
    Input pollEvent();
    void beginFrame();
    void drawCircle(Vec2 pos, float radius, sf::Color color);
    void endFrame();
    void drawLine(Vec2 from, Vec2 to, sf::Color color);
    void drawPoint(Vec2 pos, float radius, sf::Color color);
    void drawArrow(Vec2 start, Vec2 end, float headLen,
                             sf::Color color);
    Vec2 getMouseWorldPos();
    void debugDrawText(float FPS, int bodiesCount, int contatctCount,
                       float energy);

   private:
    sf::RenderWindow window;
    sf::Font font;
};
