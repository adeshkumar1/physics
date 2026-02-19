#pragma once

#include "Ball.h"
#include "Box.h"
#include "Vector2D.h"
#include <vector>

constexpr float GRAVITY = 400.0f;
constexpr float BALL_RESTITUTION = 0.8f;
constexpr float WALL_RESTITUTION = 0.85f;
constexpr float AIR_DRAG = 0.3f;
constexpr float MAX_SPEED = 800.0f;

bool checkCollision(const Ball &ball1, const Ball &ball2);

void resolveCollision(Ball &ball1, Ball &ball2);

void resolveBoxCollision(Ball &ball, const Box &box);

void updatePhysics(std::vector<Ball> &balls, const Box &box, float timeStep);
