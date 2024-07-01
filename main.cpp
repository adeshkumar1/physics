#include "Ball.h"
#include "Box.h"
#include "Physics.h"
#include "Vector2D.h"
#include <SFML/Graphics.hpp>
#include <random>

void drawBall(sf::RenderWindow &window, const Ball &ball) {
	sf::CircleShape shape(ball.getRadius());
	shape.setFillColor(ball.getColor());
	shape.setPosition(ball.getPosition().getX() - ball.getRadius(), ball.getPosition().getY() - ball.getRadius());
	window.draw(shape);
}

void drawBox(sf::RenderWindow &window, const Box &box) {
	sf::RectangleShape shape({box.getWidth(), box.getHeight()});
	shape.setOrigin(box.getWidth() / 2, box.getHeight() / 2);
	shape.setOutlineThickness(4.0);
	shape.setFillColor(sf::Color(220, 220, 220, 20));
	shape.setOutlineColor(sf::Color::Blue);
	shape.setPosition(sf::Vector2f(400, 300));
	window.draw(shape);
}

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 600), "Epic Ball Movements");

	std::default_random_engine generator;
	std::uniform_int_distribution<int> circleRadius(15, 25);
	std::uniform_int_distribution<int> xVel(-150, 150);
	std::uniform_int_distribution<int> yVel(0, 100);
	std::uniform_int_distribution<int> rgb(0, 255);

	Box box(700, 550);
	std::vector<Ball> balls;

	sf::Clock clock;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
				Ball ball(Vector2D(400, 110), Vector2D(xVel(generator), yVel(generator)), circleRadius(generator));
				sf::Color randColor(rgb(generator), rgb(generator), rgb(generator));
				ball.setColor(randColor);
				balls.push_back(ball);
			}
		}

		float dt = clock.restart().asSeconds();
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
