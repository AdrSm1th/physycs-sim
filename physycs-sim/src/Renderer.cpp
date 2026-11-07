#include "Renderer.h"

#include <imgui-SFML.h>
#include <imgui.h>
#include <algorithm>

Renderer::Renderer(unsigned int width, unsigned int height,
                   const std::string &title, unsigned int fpsLimit)
    : font(), window(sf::VideoMode({width, height}), title) {
    window.setFramerateLimit(fpsLimit);

    if (!ImGui::SFML::Init(window)) {
        throw std::runtime_error("ImGui initialization failed");
    }

    ImGuiIO &io = ImGui::GetIO();
    if (io.Fonts->Fonts.Size == 0) {
        io.Fonts->AddFontDefault();
    }

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

void Renderer::drawBox(Vec2 pos, Vec2 halfExterns, sf::Color color) {
    sf::RectangleShape box;
    box.setSize({halfExterns.x * 2.f, halfExterns.y * 2.f});
    box.setOrigin({halfExterns.x, halfExterns.y});
    box.setPosition({pos.x, pos.y});
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

void Renderer::drawImGuiPanel(bool &pause, bool &debugDraw, World &world) {
    const ImVec2 windowSize = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(PANEL_WIDTH, windowSize.y),
                             ImGuiCond_Always);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

    if (ImGui::Begin("Simulation", nullptr, flags)) {
        ImGui::Text("Simulation");
        ImGui::Separator();

        if (ImGui::Button(pause ? "Continue" : "Pause")) {
            pause = !pause;
        }

        ImGui::Checkbox("Debug draw", &debugDraw);

        ImGui::SliderFloat("Gravity Y", &world.gravity.y, -1000.0f, 1000.0f);
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

    return pos.x >= PANEL_WIDTH && pos.x < static_cast<int>(size.x) &&
           pos.y >= 0 && pos.y < static_cast<int>(size.y);
}