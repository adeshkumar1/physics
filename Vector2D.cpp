#include "Vector2D.h"

Vector2D Vector2D::operator+(const Vector2D &other) {
	return Vector2D(xPos + other.xPos, yPos + other.yPos);
}

Vector2D Vector2D::operator-(const Vector2D &other) {
	return Vector2D(xPos - other.xPos, yPos - other.yPos);
}

Vector2D Vector2D::operator*(float scalar) const {
	return Vector2D(xPos * scalar, yPos * scalar);
}
Vector2D Vector2D::operator/(float scalar) const {
	return Vector2D(xPos / scalar, yPos / scalar);
}

float Vector2D::length() const {
	return sqrt(xPos * xPos + yPos * yPos);
}

Vector2D Vector2D::normalization() const {
	float len = length();
	return Vector2D(xPos / len, yPos / len);
}
