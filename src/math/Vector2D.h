#pragma once
#include <cmath>

class Vector2D {
 private:
	float xPos, yPos;

 public:
	Vector2D() : xPos(0.0f), yPos(0.0f) {}
	Vector2D(float x, float y) : xPos(x), yPos(y) {}

	Vector2D operator+(const Vector2D &other) const;
	Vector2D operator-(const Vector2D &other) const;
	Vector2D operator*(float scalar) const;
	Vector2D operator/(float scalar) const;

	Vector2D &operator+=(const Vector2D &other);
	Vector2D &operator-=(const Vector2D &other);
	Vector2D &operator*=(float scalar);

	float dot(const Vector2D &other) const;
	float lengthSquared() const;
	float length() const;
	Vector2D normalized() const;

	float getX() const { return xPos; }
	float getY() const { return yPos; }

	void setX(float num) { xPos = num; }
	void setY(float num) { yPos = num; }
};
