#include "Physics.h"
#include "Vector2D.h"

bool checkCollision(const Ball &ball1, const Ball &ball2) {
	Vector2D difference = ball1.getPosition() - ball2.getPosition();

	float distance = difference.length();

	return distance <= ball1.getRadius() + ball2.getRadius();
}

void resolveCollision(Ball &ball1, Ball &ball2) {
	if (checkCollision(ball1, ball2)) {
		Vector2D normal = ball2.getPosition() - ball1.getPosition();
		normal = normal.normalization();

		Vector2D relativeVel = ball2.getVelocity() - ball1.getVelocity();

		float velocityAlongNormal = relativeVel.getX() * normal.getX() + relativeVel.getY() * normal.getY();
		if (velocityAlongNormal > 0) {
			return;
		}
		float impulseScalar = -(2.1) * velocityAlongNormal / (1 / ball1.getRadius() + 1 / ball2.getRadius());
		Vector2D impulseVector = normal * impulseScalar;

		ball1.setVelocity(ball1.getVelocity() - impulseVector / ball1.getRadius());
		ball2.setVelocity(ball2.getVelocity() + impulseVector / ball2.getRadius());
	}
}

void checkBoxCollision(Ball &ball, const Box &box) {
	Vector2D pos = ball.getPosition();
	Vector2D velocity = ball.getVelocity();

	if (pos.getX() - ball.getRadius() <= 400 - box.getWidth() / 2) {
		velocity.setX(-velocity.getX());
		ball.setVelocity(velocity);
		ball.setPosition({ball.getRadius() + 400 - box.getWidth() / 2, pos.getY()});
	}

	if (pos.getX() + ball.getRadius() >= 400 + box.getWidth() / 2) {
		velocity.setX(-velocity.getX());
		ball.setVelocity(velocity);
		ball.setPosition({400 + box.getWidth() / 2 - ball.getRadius(), pos.getY()});
	}

	if (pos.getY() - ball.getRadius() <= 300 - box.getHeight() / 2) {
		velocity.setY(-velocity.getY());
		ball.setVelocity(velocity);
		ball.setPosition({pos.getX(), ball.getRadius() + 300 - box.getHeight() / 2});
	}

	if (pos.getY() + ball.getRadius() >= 300 + box.getHeight() / 2) {
		velocity.setY(-velocity.getY());
		ball.setVelocity(velocity);
		ball.setPosition({pos.getX(), 300 + box.getHeight() / 2 - ball.getRadius()});
	}
}

void updatePhysics(std::vector<Ball> &balls, const Box &box, float timeStep) {
	for (unsigned i = 0; i < balls.size(); i++) {
		auto &ball = balls.at(i);
		checkBoxCollision(ball, box);
		for (unsigned j = i + 1; j < balls.size(); j++) {
			bool collision = checkCollision(balls.at(i), balls.at(j));
			if (collision) {
				resolveCollision(balls.at(i), balls.at(j));
			}
		}

		Vector2D vel = ball.getVelocity();
		Vector2D pos = ball.getPosition();

		float newX = pos.getX() + timeStep * vel.getX();
		float newY = pos.getY() + timeStep * vel.getY();

		float newVelY = vel.getY() + 0.15;

		Vector2D newPos(newX, newY);
		Vector2D newVel({vel.getX(), newVelY});

		ball.setPosition(newPos);
		ball.setVelocity(newVel);
	}
}
