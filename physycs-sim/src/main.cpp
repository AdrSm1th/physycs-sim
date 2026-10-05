#include <SFML/Graphics.hpp>

#include "Renderer.h"
#include "Vec2.h"
#include "Vec2Tests.h"
#include "Body.h"
#include "World.h"

int WinMain() {
#ifdef _DEBUG
	testVec2();
#endif

	Renderer renderer(1280, 720, "physics simulator", 60);
	sf::Clock clock;
	float smoothedFps = 0;

	World world;
	world.gravity = { 0, 500 };
	Body body_1({ 500, 100 }, 100, 5, sf::Color::White);
	Body body_2({ 1000, 100 }, 100, 200, sf::Color::Yellow);
	world.addBody(body_1);
	world.addBody(body_2);

	const float FIXED_DT = 1.0f / 120.0f; // 120 √ц физика
	const int MAX_STEPS = 5;
	const float BASE_RADIUS = 100.0f;
	const float BASE_MASS = 10.0f;
	float accumulator = 0.f;

	bool debugDraw = false;
	bool pause = false;

	while (renderer.isOpen()) {
		Input in = renderer.pollEvent();

		if (in.toggleDebud) { debugDraw = !debugDraw; }

		if (in.togglePause) { pause = !pause; }

		if (in.reset) { world.reset(); }

		if (in.spawn) {
			Vec2 mousePos = renderer.getMouseWorldPos();
			Body newBody({mousePos.x, mousePos.y}, BASE_RADIUS, BASE_MASS, sf::Color::White);
			world.addBody(newBody);
		}

		float frameDt = clock.restart().asSeconds();
		if (frameDt > 0.25f) frameDt = 0.25f;

		if (!pause) {
			accumulator += frameDt;
			int steps = 0;
			while (accumulator >= FIXED_DT) {
				world.step(FIXED_DT);
				accumulator -= FIXED_DT;
				steps++;
			}
			if (steps >= MAX_STEPS) accumulator = 0.f;
		}
		else if (in.stepOnce) {
			world.step(FIXED_DT);
		}

		float instantFps = 1.f / frameDt;
		smoothedFps = smoothedFps * 0.9f + instantFps * 0.1f;

		renderer.beginFrame();
		renderer.drawFPS(smoothedFps);
		for (Body &body : world.bodies) {
			renderer.drawCircle(body.position, body.radius, body.color);

			if (debugDraw) {
				renderer.drawLine(body.position, body.position + body.velocity * 0.1f, sf::Color::Red);
			}
		}
		renderer.endFrame();
	}

	return 0;
}