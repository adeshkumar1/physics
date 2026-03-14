#pragma once
#include "math/Vector2D.h"
#include <cmath>
#include <cstdint>

struct Color {
	uint8_t r = 255, g = 255, b = 255, a = 255;
};

struct Material {
	float restitution = 0.75f;
	float friction = 0.15f;
};

class Ball {
 private:
	Vector2D position;
	Vector2D velocity;
	float angularVelocity = 0.0f;
	float angle = 0.0f;
	float radius = 1.0f;
	float mass = 1.0f;
	float inertia = 1.0f;
	Material material;
	Color color;
	bool sleeping = false;
	float sleepTimer = 0.0f;

	void computeMassAndInertia() {
		mass = 3.14159f * radius * radius;
		inertia = 0.5f * mass * radius * radius;
	}

 public:
	Ball() = default;

	Ball(Vector2D position, Vector2D velocity, float radius)
	    : position(position), velocity(velocity), radius(radius) {
		computeMassAndInertia();
	}

	const Vector2D &getPosition() const { return position; }
	const Vector2D &getVelocity() const { return velocity; }
	float getAngularVelocity() const { return angularVelocity; }
	float getAngle() const { return angle; }
	float getRadius() const { return radius; }
	float getMass() const { return mass; }
	float getInertia() const { return inertia; }
	const Material &getMaterial() const { return material; }
	Color getColor() const { return color; }
	bool isSleeping() const { return sleeping; }
	float getSleepTimer() const { return sleepTimer; }

	void setPosition(const Vector2D &pos) { position = pos; }
	void setVelocity(const Vector2D &vel) { velocity = vel; }
	void setAngularVelocity(float av) { angularVelocity = av; }
	void setAngle(float a) { angle = a; }

	void setRadius(float r) {
		radius = r;
		computeMassAndInertia();
	}

	void setMaterial(const Material &mat) { material = mat; }
	void setColor(Color c) { color = c; }
	void setSleeping(bool s) { sleeping = s; }
	void setSleepTimer(float t) { sleepTimer = t; }

	void wake() {
		sleeping = false;
		sleepTimer = 0.0f;
	}
};
