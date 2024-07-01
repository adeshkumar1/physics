#pragma once

#include "Ball.h"
#include "Box.h"
#include "Vector2D.h"
#include <cmath>
#include <iostream>
#include <vector>

bool checkCollision(const Ball &ball1, const Ball &ball2);

void resolveCollision(Ball &ball1, Ball &ball2);

void checkBoxCollision(Ball &ball, const Box &box);

void updatePhysics(std::vector<Ball> &balls, const Box &box, float timeStep);
