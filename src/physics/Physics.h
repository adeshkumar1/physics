#pragma once

#include "entities/Ball.h"
#include "entities/Box.h"
#include "entities/Obstacle.h"
#include "math/Vector2D.h"
#include "physics/Forces.h"
#include <vector>

struct PhysicsConfig {
	float gravity = 400.0f;
	float airDrag = 0.005f;
	float wallRestitution = 0.9f;
	float maxSpeed = 800.0f;
	float sleepVelocityThreshold = 10.0f;
	float sleepTimeRequired = 0.5f;
	int subSteps = 4;
};

struct PhysicsWorld {
	std::vector<Ball> balls;
	std::vector<Attractor> attractors;
	std::vector<Spring> springs;
	std::vector<RectObstacle> rectObstacles;
	std::vector<LineObstacle> lineObstacles;
	std::vector<ContactInfo> contacts;  // populated each frame, cleared on next
};

void updatePhysics(PhysicsWorld &world, const Box &box, float timeStep,
                   const PhysicsConfig &config);
