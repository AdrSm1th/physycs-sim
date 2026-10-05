#include "Renderer.h"

Renderer::Renderer(unsigned int width, unsigned int height,
	const std::string &title, unsigned int fpsLimit)
	: font(),
	window(sf::VideoMode({ width, height }), title),
	fpsText(font, "FPS: 0", 18) {
	window.setFramerateLimit(fpsLimit);

	if (!font.openFromFile("assets/Insight Sans SSi.ttf")) {
		throw std::runtime_error("Font not loaded");
	}

	fpsText.setFillColor(sf::Color::White);
	fpsText.setPosition({ 10.f, 10.f });
}

bool Renderer::isOpen() { return window.isOpen(); }

Input Renderer::pollEvent() {
	Input in;

	while (const std::optional event = window.pollEvent()) {
		if (event->is<sf::Event::Closed>()) {
			window.close();
		}

		if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
			if (keyPressed->code == sf::Keyboard::Key::D) {
				in.toggleDebud = true;
			}
		}
	}

	return in;
}

void Renderer::beginFrame() { window.clear(sf::Color::Black); }

void Renderer::drawCircle(Vec2 pos, float radius, sf::Color color) {
	sf::CircleShape shape(radius);
	shape.setOrigin(
		{ radius, radius });      // ”становка позиции отрисовки в центр круга
	shape.setPosition({ pos.x, pos.y });  // ѕозици€ круга в глобальных координатах
	shape.setFillColor(color);
	window.draw(shape);
}

void Renderer::endFrame() { window.display(); }

void Renderer::drawFPS(float fps) {
	fpsText.setString("FPS: " + std::to_string(static_cast<int>(fps)));
	window.draw(fpsText);
}

void Renderer::drawLine(Vec2 from, Vec2 to, sf::Color color) {
	sf::VertexArray line(sf::PrimitiveType::Lines, 2);
	line[0] = sf::Vertex{ {from.x, from.y}, color };
	line[1] = sf::Vertex{ {to.x, to.y}, color };
	window.draw(line);
}

void Renderer::drawPoint(Vec2 pos, float radius, sf::Color color) {
	sf::CircleShape shape(radius);
	shape.setOrigin(
		{ radius, radius });
	shape.setPosition({ pos.x, pos.y });
	shape.setFillColor(color);
	window.draw(shape);
}