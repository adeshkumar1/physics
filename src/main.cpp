#include "entities/Ball.h"
#include "entities/Box.h"
#include "entities/Obstacle.h"
#include "math/Vector2D.h"
#include "physics/Forces.h"
#include "physics/Physics.h"
#include "physics/PhysicsThread.h"
#include <SFML/Graphics.hpp>
#include <random>
#include <sstream>

static unsigned WINDOW_WIDTH = 1280;
static unsigned WINDOW_HEIGHT = 720;

static sf::Color toSfColor(Color c, uint8_t alpha = 255) {
	return {c.r, c.g, c.b, alpha};
}

void drawBall(sf::RenderWindow &window, const Ball &ball, bool debugMode) {
	float r = ball.getRadius();
	float x = ball.getPosition().getX();
	float y = ball.getPosition().getY();

	sf::CircleShape shape(r);
	sf::Color fillColor = toSfColor(ball.getColor());
	if (ball.isSleeping()) {
		fillColor.a = 100;
	}
	shape.setFillColor(fillColor);
	shape.setPosition({x - r, y - r});
	window.draw(shape);

	// Rotation indicator
	float angle = ball.getAngle();
	float lineR = r * 0.8f;
	sf::Vertex rotLine[] = {
	    sf::Vertex{sf::Vector2f{x, y}, sf::Color::White},
	    sf::Vertex{sf::Vector2f{x + std::cos(angle) * lineR,
	                            y + std::sin(angle) * lineR},
	               sf::Color::White},
	};
	window.draw(rotLine, 2, sf::PrimitiveType::Lines);

	if (debugMode) {
		float vx = ball.getVelocity().getX() * 0.2f;
		float vy = ball.getVelocity().getY() * 0.2f;
		sf::Vertex velLine[] = {
		    sf::Vertex{sf::Vector2f{x, y}, sf::Color::Green},
		    sf::Vertex{sf::Vector2f{x + vx, y + vy}, sf::Color::Green},
		};
		window.draw(velLine, 2, sf::PrimitiveType::Lines);
	}
}

void drawBox(sf::RenderWindow &window, const Box &box) {
	sf::RectangleShape shape(sf::Vector2f{box.getWidth(), box.getHeight()});
	shape.setOrigin({box.getWidth() / 2.0f, box.getHeight() / 2.0f});
	shape.setOutlineThickness(4.0f);
	shape.setFillColor(sf::Color(220, 220, 220, 20));
	shape.setOutlineColor(sf::Color::Blue);
	shape.setPosition({box.getX(), box.getY()});
	window.draw(shape);
}

void drawAttractor(sf::RenderWindow &window, const Attractor &att) {
	float r = 12.0f;
	sf::CircleShape shape(r);
	shape.setOrigin({r, r});
	shape.setPosition({att.position.getX(), att.position.getY()});
	if (att.strength > 0) {
		shape.setFillColor(sf::Color(0, 200, 255, 150));
		shape.setOutlineColor(sf::Color(0, 200, 255));
	} else {
		shape.setFillColor(sf::Color(255, 80, 80, 150));
		shape.setOutlineColor(sf::Color(255, 80, 80));
	}
	shape.setOutlineThickness(2.0f);
	window.draw(shape);

	// Draw influence radius
	sf::CircleShape radius(att.radius);
	radius.setOrigin({att.radius, att.radius});
	radius.setPosition({att.position.getX(), att.position.getY()});
	radius.setFillColor(sf::Color::Transparent);
	radius.setOutlineColor(sf::Color(255, 255, 255, 30));
	radius.setOutlineThickness(1.0f);
	window.draw(radius);
}

void drawSpring(sf::RenderWindow &window, const Spring &spring,
                const std::vector<Ball> &balls) {
	if (spring.ballA >= balls.size() || spring.ballB >= balls.size()) return;
	const auto &a = balls[spring.ballA];
	const auto &b = balls[spring.ballB];
	sf::Vertex line[] = {
	    sf::Vertex{sf::Vector2f{a.getPosition().getX(),
	                            a.getPosition().getY()},
	               sf::Color(255, 255, 100, 180)},
	    sf::Vertex{sf::Vector2f{b.getPosition().getX(),
	                            b.getPosition().getY()},
	               sf::Color(255, 255, 100, 180)},
	};
	window.draw(line, 2, sf::PrimitiveType::Lines);
}

void drawRectObstacle(sf::RenderWindow &window, const RectObstacle &obs) {
	sf::RectangleShape shape(sf::Vector2f{obs.width, obs.height});
	shape.setOrigin({obs.width / 2.0f, obs.height / 2.0f});
	shape.setPosition({obs.position.getX(), obs.position.getY()});
	shape.setFillColor(sf::Color(100, 100, 100, 200));
	shape.setOutlineColor(sf::Color(180, 180, 180));
	shape.setOutlineThickness(2.0f);
	window.draw(shape);
}

void drawLineObstacle(sf::RenderWindow &window, const LineObstacle &obs) {
	sf::Vertex line[] = {
	    sf::Vertex{sf::Vector2f{obs.start.getX(), obs.start.getY()},
	               sf::Color(180, 180, 180)},
	    sf::Vertex{sf::Vector2f{obs.end.getX(), obs.end.getY()},
	               sf::Color(180, 180, 180)},
	};
	window.draw(line, 2, sf::PrimitiveType::Lines);
}

int main() {
	auto desktop = sf::VideoMode::getDesktopMode();
	WINDOW_WIDTH = static_cast<unsigned>(desktop.size.x * 0.75f);
	WINDOW_HEIGHT = static_cast<unsigned>(desktop.size.y * 0.75f);
	sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}),
	                        "Epic Ball Movements");

	sf::Font font("/System/Library/Fonts/Geneva.ttf");
	sf::Text hudText(font);
	hudText.setCharacterSize(16);
	hudText.setFillColor(sf::Color::White);
	hudText.setPosition({10.0f, 10.0f});

	sf::Text helpText(font);
	helpText.setCharacterSize(16);
	helpText.setFillColor(sf::Color(255, 255, 255, 220));

	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_int_distribution<int> radiusDist(15, 25);
	std::uniform_int_distribution<int> xVelDist(-200, 200);
	std::uniform_int_distribution<int> yVelDist(-100, 50);
	std::uniform_int_distribution<int> rgbDist(60, 255);

	float boxW = WINDOW_WIDTH * 0.8f;
	float boxH = WINDOW_HEIGHT * 0.8f;
	Box box(boxW, boxH, {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f});

	PhysicsConfig config;
	PhysicsThread physics(box, config);
	physics.start();

	sf::Clock clock;
	bool debugMode = false;
	bool showHelp = false;



	while (window.isOpen()) {
		while (const auto event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}

			// Key events
			if (const auto *keyPressed =
			        event->getIf<sf::Event::KeyPressed>()) {
				auto key = keyPressed->code;

				if (key == sf::Keyboard::Key::Enter) {
					float r = static_cast<float>(radiusDist(generator));
					Vector2D spawnPos(box.getX(), box.getY() - boxH / 4.0f);
					Vector2D spawnVel(
					    static_cast<float>(xVelDist(generator)),
					    static_cast<float>(yVelDist(generator)));
					Ball ball(spawnPos, spawnVel, r);
					Color c{
					    static_cast<uint8_t>(rgbDist(generator)),
					    static_cast<uint8_t>(rgbDist(generator)),
					    static_cast<uint8_t>(rgbDist(generator)),
					};
					ball.setColor(c);
					physics.addBall(ball);
				}
				if (key == sf::Keyboard::Key::D) {
					debugMode = !debugMode;
				}
				if (key == sf::Keyboard::Key::H) {
					showHelp = !showHelp;
				}
				if (key == sf::Keyboard::Key::C) {
					physics.clearAllAttractors();
				}
				if (key == sf::Keyboard::Key::X) {
					physics.clearAllSprings();
				}
				if (key == sf::Keyboard::Key::R) {
					physics.clearAllBalls();
				}
			}

		}

		float dt = clock.restart().asSeconds();
		if (dt > 0.05f) dt = 0.05f;

		// Get physics snapshot for rendering
		auto balls = physics.getSnapshot();
		auto attractors = physics.getAttractors();
		auto springs = physics.getSprings();
		auto rects = physics.getRectObstacles();
		auto lines = physics.getLineObstacles();

		// --- Render ---
		window.clear();
		drawBox(window, box);

		// Obstacles
		for (const auto &r : rects) drawRectObstacle(window, r);
		for (const auto &l : lines) drawLineObstacle(window, l);

		// Springs
		for (const auto &s : springs) drawSpring(window, s, balls);

		// Attractors
		for (const auto &a : attractors) drawAttractor(window, a);

		// Balls
		for (const auto &ball : balls) drawBall(window, ball, debugMode);

		// HUD
		int sleepCount = 0;
		for (const auto &b : balls) {
			if (b.isSleeping()) sleepCount++;
		}
		std::ostringstream hud;
		hud << "FPS: " << static_cast<int>(1.0f / (dt > 0.0001f ? dt : 0.0001f))
		    << "  Balls: " << balls.size()
		    << "  Sleeping: " << sleepCount
		    << "  Attractors: " << attractors.size()
		    << "  Springs: " << springs.size()
		    << "  Obstacles: " << rects.size() + lines.size();
		hudText.setString(hud.str());
		window.draw(hudText);

		// Help overlay
		if (showHelp) {
			sf::RectangleShape bg(sf::Vector2f{340.0f, 280.0f});
			bg.setPosition({WINDOW_WIDTH / 2.0f - 170.0f,
			                WINDOW_HEIGHT / 2.0f - 140.0f});
			bg.setFillColor(sf::Color(0, 0, 0, 200));
			window.draw(bg);

			helpText.setString(
			    "Controls:\n\n"
			    "Enter        Spawn ball\n"
			    "R            Reset (clear all balls)\n"
			    "Left-click   Place attractor\n"
			    "Right-click  Place repulsor\n"
			    "Mid-click x2 Spring between balls\n"
			    "O + drag     Place obstacle\n"
			    "C            Clear attractors\n"
			    "X            Clear springs\n"
			    "D            Toggle debug\n"
			    "H            Toggle this help\n");
			helpText.setPosition({WINDOW_WIDTH / 2.0f - 150.0f,
			                      WINDOW_HEIGHT / 2.0f - 120.0f});
			window.draw(helpText);
		}

		window.display();
	}

	physics.stop();
	return 0;
}
