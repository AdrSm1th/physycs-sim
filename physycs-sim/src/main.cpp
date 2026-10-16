#include <SFML/Graphics.hpp>
#include <algorithm>
#include <variant>

#include "Body.h"
#include "CollisionTests.h"
#include "Manifold.h"
#include "Renderer.h"
#include "Vec2.h"
#include "Vec2Tests.h"
#include "World.h"

int WinMain() {
#ifdef _DEBUG
    testVec2();
    testCollision();
#endif

    Renderer renderer(1280, 720, "physics simulator", 60);
    sf::Clock clock;
    float smoothedFps = 0;

    World world;
    world.gravity = {0, 500};
    world.setupScene();

    const float FIXED_DT = 1.0f / 120.0f;  // 120 √ц физика
    const int MAX_STEPS = 5;
    const float BASE_RADIUS = 40.0f;
    const float BASE_MASS = 10.0f;
    float accumulator = 0.f;

    bool debugDraw = false;
    bool pause = false;

    while (renderer.isOpen()) {
        Input in = renderer.pollEvent();

        if (in.toggleDebug) {
            debugDraw = !debugDraw;
        }

        if (in.togglePause) {
            pause = !pause;
        }

        if (in.reset) {
            world.reset();
        }

        if (in.spawn) {
            Vec2 mousePos = renderer.getMouseWorldPos();
            Body newBody({mousePos.x, mousePos.y}, BASE_RADIUS, BASE_MASS,
                         sf::Color::White);
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
        } else if (in.stepOnce) {
            world.step(FIXED_DT);
        }

        float instantFps = 1.f / frameDt;
        smoothedFps = smoothedFps * 0.9f + instantFps * 0.1f;

        renderer.beginFrame();

        renderer.debugDrawText(smoothedFps, world.bodies.size(),
                               world.contacts.size(), world.kineticEnergy());

        for (Body &body : world.bodies) {
            std::visit(
                [&](auto &&arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, CircleShape>) {
                        renderer.drawCircle(body.position, arg.radius,
                                            body.color);
                    } else if constexpr (std::is_same_v<T, BoxShape>) {
                        renderer.drawBox(body.position, arg.halfExtents,
                                         body.color);
                    }
                },
                body.shape);

            if (debugDraw) {
                renderer.drawLine(body.position,
                                  body.position + body.velocity * 0.1f,
                                  sf::Color::Red);
            }
        }

if (debugDraw) {
            constexpr float MAX_PENETRATION = 50.0f;
            constexpr float SCALE = 2.f;
            constexpr float HEAD_LEN = 10.f;
            constexpr float MIN_VISIBLE = 3.f;

            for (const Manifold &m : world.contacts) {
                const Body &a = world.bodies[m.a];
                const Body &b = world.bodies[m.b];

                float t = std::clamp(m.penetration / MAX_PENETRATION, 0.f, 1.f);
                sf::Color color(255, static_cast<std::uint8_t>(255 * (1.f - t)),
                                0);
                renderer.drawLine(a.position, b.position, color);

                Vec2 pointOfContact;
                bool hasPoint = false;

                if (auto *ca = std::get_if<CircleShape>(&a.shape)) {
                    pointOfContact = a.position + m.normal * ca->radius;
                    hasPoint = true;
                } else if (auto *ba = std::get_if<BoxShape>(&a.shape)) {
                    Vec2 d = {
                        ba->halfExtents.x * (m.normal.x >= 0.f ? 1.f : -1.f),
                        ba->halfExtents.y * (m.normal.y >= 0.f ? 1.f : -1.f)};
                    pointOfContact = a.position + d;
                    hasPoint = true;
                }

                if (!hasPoint) continue;

                renderer.drawPoint(pointOfContact, 2.0f, sf::Color::Red);

                float arrowLen = m.penetration * SCALE;
                if (arrowLen < MIN_VISIBLE) continue;

                Vec2 end = pointOfContact + m.normal * arrowLen;
                renderer.drawArrow(pointOfContact, end, HEAD_LEN,
                                   sf::Color::Cyan);
            }
        }

        renderer.endFrame();
    }

    return 0;
}