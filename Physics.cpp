#include "Physics.h"

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

	float halfWidth = box.getWidth() / 2;
	float halfHeight = box.getHeight() / 2;
	float boxCenterX = box.getX();
	float boxCenterY = box.getY();

	// Check collision with the left wall of the box
	if (pos.getX() - ball.getRadius() <= boxCenterX - halfWidth) {
		velocity.setX(-velocity.getX());
		ball.setVelocity(velocity);
		ball.setPosition({boxCenterX - halfWidth + ball.getRadius(), pos.getY()});
	}

	// Check collision with the right wall of the box
	if (pos.getX() + ball.getRadius() >= boxCenterX + halfWidth) {
		velocity.setX(-velocity.getX());
		ball.setVelocity(velocity);
		ball.setPosition({boxCenterX + halfWidth - ball.getRadius(), pos.getY()});
	}

	// Check collision with the top wall of the box
	if (pos.getY() - ball.getRadius() <= boxCenterY - halfHeight) {
		velocity.setY(-velocity.getY());
		ball.setVelocity(velocity);
		ball.setPosition({pos.getX(), boxCenterY - halfHeight + ball.getRadius()});
	}

	// Check collision with the bottom wall of the box
	if (pos.getY() + ball.getRadius() >= boxCenterY + halfHeight) {
		velocity.setY(-velocity.getY());
		ball.setVelocity(velocity);
		ball.setPosition({pos.getX(), boxCenterY + halfHeight - ball.getRadius()});
	}
}

void updatePhysics(std::vector<Ball> &balls, const Box &box, float timeStep) {
	std::sort(balls.begin(), balls.end(), [](const Ball &a, const Ball &b) {
		return a.getPosition().getX() < b.getPosition().getX();
	});
	for (unsigned i = 0; i < balls.size(); i++) {
		auto &ball = balls.at(i);
		checkBoxCollision(ball, box);
		for (unsigned j = i + 1; j < balls.size(); j++) {
			if (balls.at(j).getPosition().getX() - ball.getPosition().getX() > ball.getRadius() + balls.at(j).getRadius()) {
				break;
			}
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
