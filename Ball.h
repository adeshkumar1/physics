#pragma once
#include "Vector2D.h"
#include <sfml/Graphics.hpp>

class Ball {
 private:
	Vector2D position;
	Vector2D velocity;
	float radius;
	sf::Color color;

 public:
	Ball() = default;
	Ball(Vector2D position, Vector2D velocity, float radius) : position(position), velocity(velocity), radius(radius) {}

	Vector2D getPosition() const {
		return position;
	}
	Vector2D getVelocity() const {
		return velocity;
	}
	float getRadius() const {
		return radius;
	}

	void setPosition(const Vector2D &other) {
		position = other;
	}

	void setVelocity(const Vector2D &other) {
		velocity = other;
		if (velocity.getX() >= 300) {
			velocity.setX(290);
		}
		if (velocity.getX() <= -300) {
			velocity.setX(-290);
		}
	}
	void setRadius(float r) {
		radius = r;
	}

	sf::Color getColor() const {
		return color;
	}

	void setColor(sf::Color c) {
		color = c;
	}
};
