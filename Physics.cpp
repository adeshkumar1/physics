#include "Physics.h"
#include <algorithm>
#include <cmath>

bool checkCollision(const Ball &ball1, const Ball &ball2) {
	Vector2D delta = ball1.getPosition() - ball2.getPosition();
	float radiiSum = ball1.getRadius() + ball2.getRadius();
	return delta.lengthSquared() <= radiiSum * radiiSum;
}

void resolveCollision(Ball &ball1, Ball &ball2) {
	Vector2D delta = ball2.getPosition() - ball1.getPosition();
	float dist = delta.length();
	float radiiSum = ball1.getRadius() + ball2.getRadius();

	if (dist >= radiiSum || dist < 1e-6f) return;

	Vector2D normal = delta / dist;

	// Positional correction: push balls apart so they no longer overlap
	float overlap = radiiSum - dist;
	float totalMass = ball1.getMass() + ball2.getMass();
	ball1.setPosition(ball1.getPosition() - normal * (overlap * ball2.getMass() / totalMass));
	ball2.setPosition(ball2.getPosition() + normal * (overlap * ball1.getMass() / totalMass));

	// Impulse-based velocity resolution
	Vector2D relativeVel = ball1.getVelocity() - ball2.getVelocity();
	float velAlongNormal = relativeVel.dot(normal);

	if (velAlongNormal < 0) return;

	float impulseScalar = -(1.0f + BALL_RESTITUTION) * velAlongNormal
	                      / (1.0f / ball1.getMass() + 1.0f / ball2.getMass());

	Vector2D impulse = normal * impulseScalar;
	ball1.setVelocity(ball1.getVelocity() + impulse / ball1.getMass());
	ball2.setVelocity(ball2.getVelocity() - impulse / ball2.getMass());
}

void resolveBoxCollision(Ball &ball, const Box &box) {
	Vector2D pos = ball.getPosition();
	Vector2D vel = ball.getVelocity();
	float r = ball.getRadius();

	float halfW = box.getWidth() / 2.0f;
	float halfH = box.getHeight() / 2.0f;
	float cx = box.getX();
	float cy = box.getY();

	float left = cx - halfW;
	float right = cx + halfW;
	float top = cy - halfH;
	float bottom = cy + halfH;

	if (pos.getX() - r <= left) {
		vel.setX(std::abs(vel.getX()) * WALL_RESTITUTION);
		pos.setX(left + r);
	} else if (pos.getX() + r >= right) {
		vel.setX(-std::abs(vel.getX()) * WALL_RESTITUTION);
		pos.setX(right - r);
	}

	if (pos.getY() - r <= top) {
		vel.setY(std::abs(vel.getY()) * WALL_RESTITUTION);
		pos.setY(top + r);
	} else if (pos.getY() + r >= bottom) {
		vel.setY(-std::abs(vel.getY()) * WALL_RESTITUTION);
		pos.setY(bottom - r);
	}

	ball.setPosition(pos);
	ball.setVelocity(vel);
}

static void clampSpeed(Ball &ball) {
	Vector2D vel = ball.getVelocity();
	if (vel.lengthSquared() > MAX_SPEED * MAX_SPEED) {
		ball.setVelocity(vel.normalized() * MAX_SPEED);
	}
}

void updatePhysics(std::vector<Ball> &balls, const Box &box, float timeStep) {
	// Sort by X for sweep-and-prune broad phase
	std::sort(balls.begin(), balls.end(), [](const Ball &a, const Ball &b) {
		return a.getPosition().getX() < b.getPosition().getX();
	});

	for (size_t i = 0; i < balls.size(); i++) {
		Ball &ball = balls[i];

		// Semi-implicit Euler: apply forces to velocity first
		Vector2D vel = ball.getVelocity();
		vel.setY(vel.getY() + GRAVITY * timeStep);
		vel *= (1.0f - AIR_DRAG * timeStep);
		ball.setVelocity(vel);

		// Then integrate position using the updated velocity
		ball.setPosition(ball.getPosition() + ball.getVelocity() * timeStep);

		resolveBoxCollision(ball, box);

		// Narrow-phase collision detection against nearby balls
		for (size_t j = i + 1; j < balls.size(); j++) {
			float xGap = balls[j].getPosition().getX() - ball.getPosition().getX();
			if (xGap > ball.getRadius() + balls[j].getRadius()) break;

			if (checkCollision(ball, balls[j])) {
				resolveCollision(ball, balls[j]);
			}
		}

		clampSpeed(ball);
	}
}
