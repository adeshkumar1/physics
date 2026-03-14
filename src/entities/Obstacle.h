#pragma once

#include "math/Vector2D.h"

struct RectObstacle {
	Vector2D position;  // center
	float width;
	float height;
};

struct LineObstacle {
	Vector2D start;
	Vector2D end;
};
