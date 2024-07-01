#include "Ball.h"
#include "Box.h"
#include "Physics.h"
#include "Vector2D.h"
#include <SFML/Graphics.hpp>
#include <iostream>

void drawBall(sf::RenderWindow &window, const Ball &ball) {
	sf::CircleShape shape(ball.getRadius());
	shape.setFillColor(sf::Color::Green);
	shape.setPosition(ball.getPosition().getX() - ball.getRadius(),
							ball.getPosition().getY() - ball.getRadius());
	window.draw(shape);
}

void drawBox(sf::RenderWindow &window, const Box &box) {
	sf::RectangleShape shape({box.getWidth(), box.getHeight()});
	shape.setOrigin(box.getWidth() / 2, box.getHeight() / 2);
	shape.setOutlineThickness(4.0);
	shape.setFillColor(sf::Color(0, 255, 0, 128));
	shape.setOutlineColor(sf::Color::Blue);
	shape.setPosition(sf::Vector2f(400, 300));
	window.draw(shape);
}

int main() {
	std::cout << "Hello World" << std::endl;
	sf::RenderWindow window(sf::VideoMode(800, 600), "Epic Ball Movements");

	Box box(500, 500);
	std::vector<Ball> balls;

	sf::Clock clock;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
				Ball ball(Vector2D(400, 110), Vector2D(-100, 100), 20);
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
