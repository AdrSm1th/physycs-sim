#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "Vec2.h"
#include "World.h"

struct Input {
    bool toggleDebug = false;
    bool togglePause = false;
    bool reset = false;
    bool stepOnce = false;

    bool mousePressed = false;
    bool mouseReleased = false;
    Vec2 mouseWorldPos;
};

class Renderer {
   public:
    static constexpr float PANEL_WIDTH = 250.0f;

    Renderer(unsigned int width, unsigned int height, const std::string &title,
             unsigned int fpsLimit);
    ~Renderer();
    bool isOpen();
    Input pollEvent();
    void beginFrame(sf::Time elapsed);
    void drawCircle(Vec2 pos, float radius, sf::Color color);
    void drawBox(Vec2 pos, Vec2 halfExterns, sf::Color color);
    void endFrame();
    void drawLine(Vec2 from, Vec2 to, sf::Color color);
    void drawPoint(Vec2 pos, float radius, sf::Color color);
    void drawArrow(Vec2 start, Vec2 end, float headLen, sf::Color color);
    Vec2 getMouseWorldPos();
    void debugDrawText(float FPS, int bodiesCount, int contatctCount,
                       float energy);
    void drawImGuiPanel(bool &pause, bool &debugDraw, World &world);
    bool isMouseInScene() const;

   private:
    sf::View sceneView;
    sf::RenderWindow window;
    sf::Font font;
    void updateSceneView();
};
