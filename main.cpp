#include "Ball.h"
#include "Box.h"
#include "Physics.h"
#include "Vector2D.h"

#include <SFML/Graphics.hpp>
#include <random>

constexpr unsigned WINDOW_WIDTH = 1920;
constexpr unsigned WINDOW_HEIGHT = 1080;

static sf::Color toSfColor(Color c) {
	return {c.r, c.g, c.b, c.a};
}

void drawBall(sf::RenderWindow &window, const Ball &ball) {
	sf::CircleShape shape(ball.getRadius());
	shape.setFillColor(toSfColor(ball.getColor()));
	shape.setPosition({
	    ball.getPosition().getX() - ball.getRadius(),
	    ball.getPosition().getY() - ball.getRadius(),
	});
	window.draw(shape);
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

int main() {
	sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Epic Ball Movements");

	std::default_random_engine generator;
	std::uniform_int_distribution<int> radiusDist(15, 25);
	std::uniform_int_distribution<int> xVelDist(-200, 200);
	std::uniform_int_distribution<int> yVelDist(-100, 50);
	std::uniform_int_distribution<int> rgbDist(60, 255);

	float boxW = WINDOW_WIDTH * 0.8f;
	float boxH = WINDOW_HEIGHT * 0.8f;
	Box box(boxW, boxH, {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f});
	std::vector<Ball> balls;

	sf::Clock clock;

	while (window.isOpen()) {
		while (const auto event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
			if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>();
			    keyPressed && keyPressed->code == sf::Keyboard::Key::Enter) {
				float r = static_cast<float>(radiusDist(generator));
				Vector2D spawnPos(box.getX(), box.getY() - boxH / 4.0f);
				Vector2D spawnVel(static_cast<float>(xVelDist(generator)),
				                  static_cast<float>(yVelDist(generator)));
				Ball ball(spawnPos, spawnVel, r);
				Color c{
				    static_cast<uint8_t>(rgbDist(generator)),
				    static_cast<uint8_t>(rgbDist(generator)),
				    static_cast<uint8_t>(rgbDist(generator)),
				};
				ball.setColor(c);
				balls.push_back(ball);
			}
		}

		float dt = clock.restart().asSeconds();
		if (dt > 0.05f) dt = 0.05f;
		updatePhysics(balls, box, dt);

		window.clear();
		drawBox(window, box);
		for (const Ball &ball : balls) {
			drawBall(window, ball);
		}
		window.display();
	}

	return 0;
}
