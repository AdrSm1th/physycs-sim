#include <SFML/Graphics.hpp>

#include "Renderer.h"
#include "Vec2.h"
#include "Vec2Tests.h"
#include "Body.h"
#include "World.h"

int WinMain() {
	Renderer renderer(1280, 720, "physics simulator", 60);
	sf::Clock clock;
	float smoothedFps = 0;

#ifdef _DEBUG
	testVec2();
#endif

	World world;
	world.gravity = { 0, 500 };
	Body body_1({ 500, 100 }, 100, 5);
	world.addBody(body_1);

	const float FIXED_DT = 1.0f / 120.0f; // 120 √ц физика
	const int MAX_STEPS = 5;
	float accumulator = 0.f;

	while (renderer.isOpen()) {
		renderer.pollEvent();
		float frameDt = clock.restart().asSeconds();
		if (frameDt > 0.25f) frameDt = 0.25f;

		accumulator += frameDt;
		int steps = 0;
		while (accumulator >= FIXED_DT) {
			world.step(FIXED_DT);
			accumulator -= FIXED_DT;
			steps++;
		}
		if (steps == MAX_STEPS) accumulator = 0.f;

		float instantFps = 1.f / frameDt;
		smoothedFps = smoothedFps * 0.9f + instantFps * 0.1f;

		renderer.beginFrame();
		renderer.drawFPS(smoothedFps);
		for (Body &body : world.bodies) {
			renderer.drawCircle(body.position, body.radius, body.color);
		}
		renderer.endFrame();
	}

	return 0;
}