#pragma once

#include <string>

#include <SFML/Graphics.hpp>

#include "Vec2.h"

struct Input {
	bool toggleDebud = false;
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
	void drawFPS(float fps);
	void drawLine(Vec2 from, Vec2 to, sf::Color color);
	void drawPoint(Vec2 pos, float radius, sf::Color color);

private:
	sf::RenderWindow window;
	sf::Font font;
	sf::Text fpsText;
};
