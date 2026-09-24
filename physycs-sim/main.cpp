#include "Renderer.h"

#include <SFML/Graphics.hpp>

int WinMain() {
	Renderer renderer(1280, 720, "physics simulator", 60);
	sf::Clock clock;
	float smoothedFps = 0;

	while (renderer.isOpen()) {
		renderer.pollEvent();
		float frameDt = clock.restart().asSeconds();
		if (frameDt > 0.25f) frameDt = 0.25f; 

		float instantFps = 1.f / frameDt;
		smoothedFps = smoothedFps * 0.9f + instantFps * 0.1f;

		renderer.beginFrame();
		renderer.drawFPS(smoothedFps);
		renderer.drawCircle(500, 500, 100, sf::Color::Red);
		renderer.endFrame();
	}

	return 0;
}