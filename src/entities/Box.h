#pragma once
#include "math/Vector2D.h"

class Box {
 private:
	float width;
	float height;
	Vector2D position;

 public:
	Box(float width, float height, Vector2D pos)
	    : width(width), height(height), position(pos) {}

	float getWidth() const { return width; }
	float getHeight() const { return height; }
	const Vector2D &getPosition() const { return position; }
	float getX() const { return position.getX(); }
	float getY() const { return position.getY(); }

	void setWidth(float w) { width = w; }
	void setHeight(float h) { height = h; }
	void setPosition(const Vector2D &pos) { position = pos; }
};
