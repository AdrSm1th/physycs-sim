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
    bool selectQuery = false;

    bool mousePressed = false;
    bool mouseReleased = false;
    Vec2 mouseWorldPos;
};

struct BodyForCreate {
    float mass = 10.0f;
    float color[3] = {1, 1, 1};
    float radius = 40.0f;
    float height = 40.0f;
    float width = 60.0f;
    float restitution = 0.5f;
    float friction = 0.5f;
    Shape shape = CircleShape(radius);
};

class Renderer {
   public:
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
    void drawImGuiPanel(bool &pause, bool &debugDraw, World &world,
                        float fixedDt);
    bool isMouseInScene() const;
    Body GetBodyForCreate() const;
    bool IsSelect() const;
    void drawBodyInspector(const Body *body);

   private:
    static constexpr float PANEL_WIDTH = 250.0f;
    static constexpr float INSPECTOR_WIDTH = 250.0f;

    sf::View sceneView;
    sf::RenderWindow window;
    sf::Font font;
    BodyForCreate bodyForCreate;
    bool selected = false;
    int shapeId = 0;

    void updateSceneView();
};
