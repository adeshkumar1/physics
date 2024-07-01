#pragma once
#include "Vector2D.h"

class Ball {
 private:
	Vector2D position;
	Vector2D velocity;
	float radius;

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
	}
	void setRadius(float r) {
		radius = r;
	}
};
