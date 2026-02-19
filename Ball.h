#pragma once
#include "Vector2D.h"
#include <cmath>
#include <cstdint>

struct Color {
	uint8_t r = 255, g = 255, b = 255, a = 255;
};

class Ball {
 private:
	Vector2D position;
	Vector2D velocity;
	float radius = 1.0f;
	float mass = 1.0f;
	Color color;

	void computeMass() { mass = 3.14159f * radius * radius; }

 public:
	Ball() = default;

	Ball(Vector2D position, Vector2D velocity, float radius)
	    : position(position), velocity(velocity), radius(radius) {
		computeMass();
	}

	const Vector2D &getPosition() const { return position; }
	const Vector2D &getVelocity() const { return velocity; }
	float getRadius() const { return radius; }
	float getMass() const { return mass; }
	Color getColor() const { return color; }

	void setPosition(const Vector2D &pos) { position = pos; }
	void setVelocity(const Vector2D &vel) { velocity = vel; }

	void setRadius(float r) {
		radius = r;
		computeMass();
	}

	void setColor(Color c) { color = c; }
};
