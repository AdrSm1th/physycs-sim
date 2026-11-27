#include "Renderer.h"

#include <imgui-SFML.h>
#include <imgui.h>

#include <SFML/OpenGL.hpp>
#include <algorithm>
#include <variant>

Renderer::Renderer(unsigned int width, unsigned int height,
                   const std::string &title, unsigned int fpsLimit)
    : font(), window(sf::VideoMode({width, height}), title) {
    window.setFramerateLimit(fpsLimit);

    if (!ImGui::SFML::Init(window)) {
        throw std::runtime_error("ImGui initialization failed");
    }

    assert(ImGui::GetCurrentContext() != nullptr &&
           "Context missing immediately after Init");

    if (!font.openFromFile("assets/Insight Sans SSi.ttf")) {
        throw std::runtime_error("Font not loaded");
    }
}

Renderer::~Renderer() { ImGui::SFML::Shutdown(); }

bool Renderer::isOpen() { return window.isOpen(); }

Input Renderer::pollEvent() {
    Input in;

    while (const std::optional event = window.pollEvent()) {
        ImGui::SFML::ProcessEvent(window, *event);

        if (event->is<sf::Event::Closed>()) {
            window.close();
        }

        if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            switch (keyPressed->code) {
                case sf::Keyboard::Key::D:
                    in.toggleDebug = true;
                    break;

                case sf::Keyboard::Key::R:
                    in.reset = true;
                    break;

                case sf::Keyboard::Key::Space:
                    in.togglePause = true;
                    break;

                case sf::Keyboard::Key::N:
                    in.stepOnce = true;
                    break;
            }
        }

        if (const auto *mousePressed =
                event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mousePressed->button == sf::Mouse::Button::Left) {
                in.mousePressed = true;
            } else if (mousePressed->button == sf::Mouse::Button::Right) {
                in.selectQuery = true;
            }
        }

        if (const auto *mouseReleased =
                event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseReleased->button == sf::Mouse::Button::Left) {
                in.mouseReleased = true;
            }
        }
    }

    updateSceneView();
    in.mouseWorldPos = getMouseWorldPos();
    return in;
}

void Renderer::beginFrame(sf::Time elapsed) {
    window.clear(sf::Color::Black);

    ImGui::SFML::Update(window, elapsed);
    updateSceneView();
    window.setView(sceneView);
}

void Renderer::drawCircle(Vec2 pos, float radius, sf::Color color) {
    sf::CircleShape shape(radius);
    shape.setOrigin(
        {radius, radius});  // ”становка позиции отрисовки в центр круга
    shape.setPosition(
        {pos.x, pos.y});  // ѕозици€ круга в глобальных координатах
    shape.setFillColor(color);
    window.draw(shape);
}

void Renderer::drawBox(Vec2 pos, Vec2 halfExterns, sf::Color color,
                       float angle) {
    sf::RectangleShape box;
    box.setSize({halfExterns.x * 2.f, halfExterns.y * 2.f});
    box.setOrigin({halfExterns.x, halfExterns.y});
    box.setPosition({pos.x, pos.y});
    box.setRotation(sf::radians(angle));
    box.setFillColor(color);
    window.draw(box);
}

void Renderer::endFrame() {
    window.setView(window.getDefaultView());

    ImGui::SFML::Render(window);
    window.display();
}

void Renderer::drawLine(Vec2 from, Vec2 to, sf::Color color) {
    sf::VertexArray line(sf::PrimitiveType::Lines, 2);
    line[0] = sf::Vertex{{from.x, from.y}, color};
    line[1] = sf::Vertex{{to.x, to.y}, color};
    window.draw(line);
}

void Renderer::drawPoint(Vec2 pos, float radius, sf::Color color) {
    sf::CircleShape shape(radius);
    shape.setOrigin({radius, radius});
    shape.setPosition({pos.x, pos.y});
    shape.setFillColor(color);
    window.draw(shape);
}

void Renderer::drawArrow(Vec2 start, Vec2 end, float headLen, sf::Color color) {
    drawLine(start, end, color);

    Vec2 back = (start - end).normalized();
    if (back.lengthSquared() < 1e-6f) return;

    constexpr float COS30 = 0.8660254f;
    constexpr float SIN30 = 0.5f;

    Vec2 head = back * headLen;

    Vec2 left(head.x * COS30 - head.y * SIN30, head.x * SIN30 + head.y * COS30);

    Vec2 right(head.x * COS30 + head.y * SIN30,
               -head.x * SIN30 + head.y * COS30);

    drawLine(end, end + left, color);
    drawLine(end, end + right, color);
}

Vec2 Renderer::getMouseWorldPos() {
    auto mousePosPixel = sf::Mouse::getPosition(window);

    auto mousePosCoord = window.mapPixelToCoords(mousePosPixel, sceneView);

    return Vec2(mousePosCoord.x, mousePosCoord.y);
}

void Renderer::debugDrawText(float FPS, int bodiesCount, int contatctCount,
                             float energy) {
    const float x = 30.0f;
    const float y = 20.0f;
    const float h = 30.0f;

    std::string textstr = "FPS: ";
    textstr += std::to_string(FPS);
    sf::Text text(font, textstr, 18);
    text.setFillColor(sf::Color::White);
    text.setPosition({x, y});
    window.draw(text);

    textstr = "Bodies: ";
    textstr += std::to_string(bodiesCount);
    text.setString(textstr);
    text.setPosition({x, y + h * 1});
    window.draw(text);

    textstr = "Contacts: ";
    textstr += std::to_string(contatctCount);
    text.setString(textstr);
    text.setPosition({x, y + h * 2});
    window.draw(text);

    textstr = "Energy: ";
    textstr += std::to_string(energy);
    text.setString(textstr);
    text.setPosition({x, y + h * 3});
    window.draw(text);
}

void Renderer::drawBodyInspector(const Body *body) {
    const ImVec2 windowSize = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(windowSize.x - INSPECTOR_WIDTH, 0.0f));

    ImGui::SetNextWindowSize(ImVec2(INSPECTOR_WIDTH, windowSize.y));

    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoResize |
                                   ImGuiWindowFlags_NoCollapse;

    if (ImGui::Begin("Body inspector", nullptr, flags)) {
        if (body == nullptr) {
            ImGui::TextWrapped("Select body (right mouse button)");
        } else {
            ImGui::Text("Mass: %.1f", body->mass);
            ImGui::Text("Moment of inertia: %.1f", body->momentOfInertia);
            ImGui::Text("Friction: %.2f", body->friction);
            ImGui::Text("Restitutuion: %.2f", body->restitution);
            ImGui::Text("Velocity: %.2f, %.2f", body->velocity.x,
                        body->velocity.y);
            ImGui::Text("Angular velocity: %.2f", body->angularVelocity);
        }
    }

    ImGui::End();
}

void Renderer::drawImGuiPanel(bool &pause, bool &debugDraw, World &world,
                              float fixedDt) {
    const ImVec2 windowSize = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(PANEL_WIDTH, windowSize.y),
                             ImGuiCond_Always);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

    if (ImGui::Begin("Simulation", nullptr, flags)) {
        ImGui::Text("Scene settings");

        if (ImGui::Button(pause ? "Continue" : "Pause")) {
            pause = !pause;
        }

        if (pause) {
            if (ImGui::Button("Step")) {
                world.step(fixedDt);
            }
        }

        ImGui::Checkbox("Debug draw", &debugDraw);

        ImGui::SliderFloat("Gravity Y", &world.gravity.y, -1000.0f, 1000.0f);

        ImGui::Separator();

        ImGui::Text("Bodies");

        if (ImGui::Button("Circle")) {
            selected = true;
            shapeId = 0;
        }

        ImGui::SameLine();

        if (ImGui::Button("Box")) {
            selected = true;
            shapeId = 1;
        }

        if (selected) {
            ImGui::InputFloat("Mass", &bodyForCreate.mass, 1.0f, 5.0f, "%.2f");
            bodyForCreate.mass = std::max(0.0f, bodyForCreate.mass);

            ImGui::ColorEdit3("Body color", bodyForCreate.color);

            ImGui::InputFloat("Restitution", &bodyForCreate.restitution, 0.01f,
                              0.1f, "%.2f");
            bodyForCreate.restitution =
                std::clamp(bodyForCreate.restitution, 0.0f, 1.0f);

            ImGui::InputFloat("Friction", &bodyForCreate.friction, 0.01f, 0.1f,
                              "%.2f");
            bodyForCreate.friction =
                std::clamp(bodyForCreate.friction, 0.0f, 1.0f);

            if (shapeId == 0) {
                ImGui::InputFloat("Radius", &bodyForCreate.radius, 1.0f, 5.0f,
                                  "%.1f");
                bodyForCreate.radius = std::max(1.0f, bodyForCreate.radius);

                bodyForCreate.shape = CircleShape(bodyForCreate.radius);
            } else if (shapeId == 1) {
                ImGui::InputFloat("Height", &bodyForCreate.height, 1.0f, 5.0f,
                                  "%.1f");
                bodyForCreate.height = std::max(1.0f, bodyForCreate.height);

                ImGui::Separator();

                ImGui::InputFloat("Width", &bodyForCreate.width, 1.0f, 5.0f,
                                  "%.1f");
                bodyForCreate.width = std::max(1.0f, bodyForCreate.width);

                bodyForCreate.shape = BoxShape(
                    {bodyForCreate.width / 2.0f, bodyForCreate.height / 2.0f});
            }
        }
    }
    ImGui::End();
}

void Renderer::updateSceneView() {
    const auto window_size = window.getSize();

    const float width = static_cast<float>(window_size.x);
    const float height = static_cast<float>(window_size.y);

    if (width <= PANEL_WIDTH || height <= 0.0f) {
        return;
    }

    const float sceneWidth = width - PANEL_WIDTH;

    constexpr float WORLD_WIDTH = 1280.0f;
    constexpr float WORLD_HEIGHT = 720.0f;

    const float scale =
        std::min(sceneWidth / WORLD_WIDTH, height / WORLD_HEIGHT);

    sceneView.setCenter({WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f});

    sceneView.setSize({sceneWidth / scale, height / scale});

    // Viewport задаЄтс€ дол€ми размера окна, а не пиксел€ми.
    sceneView.setViewport(
        sf::FloatRect({PANEL_WIDTH / width, 0.0f}, {sceneWidth / width, 1.0f}));
}

bool Renderer::isMouseInScene() const {
    const auto pos = sf::Mouse::getPosition(window);
    const auto size = window.getSize();

    return pos.x >= PANEL_WIDTH &&
           pos.x < static_cast<int>(size.x) - INSPECTOR_WIDTH && pos.y >= 0 &&
           pos.y < static_cast<int>(size.y);
}

Body Renderer::GetBodyForCreate() const {
    Body body;
    body.shape = bodyForCreate.shape;
    body.setMass(bodyForCreate.mass);
    body.restitution = bodyForCreate.restitution;
    body.friction = bodyForCreate.friction;
    body.color.r = static_cast<std::uint8_t>(bodyForCreate.color[0] * 255.0f);
    body.color.g = static_cast<std::uint8_t>(bodyForCreate.color[1] * 255.0f);
    body.color.b = static_cast<std::uint8_t>(bodyForCreate.color[2] * 255.0f);

    return body;
}

bool Renderer::IsSelect() const { return selected; }