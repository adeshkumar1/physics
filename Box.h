#pragma once

class Box {
 private:
	float width;
	float height;

 public:
	Box(float width, float height) : width(width), height(height) {}

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
};
