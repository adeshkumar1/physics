#include "math/Vector2D.h"

Vector2D Vector2D::operator+(const Vector2D &other) const {
	return {xPos + other.xPos, yPos + other.yPos};
}

Vector2D Vector2D::operator-(const Vector2D &other) const {
	return {xPos - other.xPos, yPos - other.yPos};
}

Vector2D Vector2D::operator*(float scalar) const {
	return {xPos * scalar, yPos * scalar};
}

Vector2D Vector2D::operator/(float scalar) const {
	if (std::abs(scalar) < 1e-8f) return {0.0f, 0.0f};
	return {xPos / scalar, yPos / scalar};
}

Vector2D &Vector2D::operator+=(const Vector2D &other) {
	xPos += other.xPos;
	yPos += other.yPos;
	return *this;
}

Vector2D &Vector2D::operator-=(const Vector2D &other) {
	xPos -= other.xPos;
	yPos -= other.yPos;
	return *this;
}

Vector2D &Vector2D::operator*=(float scalar) {
	xPos *= scalar;
	yPos *= scalar;
	return *this;
}

float Vector2D::dot(const Vector2D &other) const {
	return xPos * other.xPos + yPos * other.yPos;
}

float Vector2D::lengthSquared() const {
	return xPos * xPos + yPos * yPos;
}

float Vector2D::length() const {
	return std::sqrt(lengthSquared());
}

Vector2D Vector2D::normalized() const {
	float len = length();
	if (len < 1e-8f) return {0.0f, 0.0f};
	return {xPos / len, yPos / len};
}
