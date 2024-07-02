#pragma once
#include "Vector2D.h"

class Box {
 private:
	float width;
	float height;
	float x;
	float y;

 public:
	Box(float width, float height, Vector2D pos) : width(width), height(height) {
		x = pos.getX();
		y = pos.getY();
	}

	float getWidth() const {
		return width;
	}

	float getHeight() const {
		return height;
	}

	void setWidth(float w) {
		width = w;
	}

	void setHeight(float h) {
		height = h;
	}

	float getX() const {
		return x;
	}

	float getY() const {
		return y;
	}
};
