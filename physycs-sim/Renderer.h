#pragma once

#include <SFML/Graphics.hpp>

#include <string.h>

class Renderer {
public:
	Renderer(unsigned int width, unsigned int height, const std::string& title, unsigned int fpsLimit);
	bool isOpen();
	void pollEvent();
	void beginFrame();
	void drawCircle(float x, float y, float radius, sf::Color color);
	void endFrame();
	void drawFPS(float fps);

private:
	sf::RenderWindow window;
	sf::Font font;
	sf::Text fpsText;
};

