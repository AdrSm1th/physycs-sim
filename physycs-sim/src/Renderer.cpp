#include "Renderer.h"

Renderer::Renderer(unsigned int width, unsigned int height,
                   const std::string &title, unsigned int fpsLimit)
    : font(), window(sf::VideoMode({width, height}), title) {
    window.setFramerateLimit(fpsLimit);

    if (!font.openFromFile("assets/Insight Sans SSi.ttf")) {
        throw std::runtime_error("Font not loaded");
    }
}

bool Renderer::isOpen() { return window.isOpen(); }

Input Renderer::pollEvent() {
    Input in;

    while (const std::optional event = window.pollEvent()) {
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
                in.spawn = true;
            }
        }
    }

    return in;
}

void Renderer::beginFrame() { window.clear(sf::Color::Black); }

void Renderer::drawCircle(Vec2 pos, float radius, sf::Color color) {
    sf::CircleShape shape(radius);
    shape.setOrigin(
        {radius, radius});  // ”становка позиции отрисовки в центр круга
    shape.setPosition(
        {pos.x, pos.y});  // ѕозици€ круга в глобальных координатах
    shape.setFillColor(color);
    window.draw(shape);
}

void Renderer::endFrame() { window.display(); }

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

    auto mousePosCoord = window.mapPixelToCoords(mousePosPixel);

    return Vec2(mousePosCoord.x, mousePosCoord.y);
}

void Renderer::debugDrawText(float FPS, int bodiesCount, int contatctCount, float energy) {
    const float x = 10.0f;
    const float y = 10.0f;
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