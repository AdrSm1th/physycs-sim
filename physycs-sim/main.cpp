#include <SFML/Graphics.hpp>

int WinMain() {
	sf::RenderWindow window(sf::VideoMode({1280, 720}), "Physics Sim");
	window.setFramerateLimit(60);
	while (window.isOpen()) {
		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
		}
		window.clear(sf::Color::Black);
		window.display();
	}

	return 0;
}