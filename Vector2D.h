#pragma once
#include <cmath>

class Vector2D {
 private:
	float xPos, yPos;

 public:
	Vector2D() : xPos(0.0), yPos(0.0) {}
	Vector2D(float pos1, float pos2) : xPos(pos1), yPos(pos2) {}

	Vector2D operator+(const Vector2D &other);
	Vector2D operator-(const Vector2D &other);
	Vector2D operator*(float scalar) const;
	Vector2D operator/(float scalar) const;

	float length() const;
	Vector2D normalization() const;

	float getX() {
		return xPos;
	}
	float getY() {
		return yPos;
	}

	void setX(float num) {
		xPos = num;
	}
	void setY(float num) {
		yPos = num;
	}
};
