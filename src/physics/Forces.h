#pragma once

#include "entities/Ball.h"
#include "math/Vector2D.h"
#include <cmath>
#include <vector>

struct Attractor {
	Vector2D position;
	float strength;  // positive = attract, negative = repel
	float radius;    // falloff range
};

struct Spring {
	size_t ballA, ballB;
	float restLength;
	float stiffness;
	float damping;
};

struct ContactInfo {
	Vector2D point;
	Vector2D normal;
	float impulse;
	Color color;
};

inline void applyAttractors(std::vector<Ball> &balls,
                            const std::vector<Attractor> &attractors, float dt) {
	for (auto &ball : balls) {
		if (ball.isSleeping()) continue;

		for (const auto &att : attractors) {
			Vector2D delta = att.position - ball.getPosition();
			float distSq = delta.lengthSquared();
			float minDist = att.radius * 0.1f;
			if (distSq < minDist * minDist) distSq = minDist * minDist;
			if (distSq > att.radius * att.radius) continue;

			float dist = std::sqrt(distSq);
			Vector2D dir = delta / dist;

			// Inverse-square falloff
			float forceMag = att.strength / distSq;
			Vector2D force = dir * forceMag;
			Vector2D vel = ball.getVelocity();
			vel += force * (dt / ball.getMass());
			ball.setVelocity(vel);
			ball.wake();
		}
	}
}

inline void applySprings(std::vector<Ball> &balls,
                         const std::vector<Spring> &springs, float dt) {
	for (const auto &spring : springs) {
		if (spring.ballA >= balls.size() || spring.ballB >= balls.size())
			continue;

		Ball &a = balls[spring.ballA];
		Ball &b = balls[spring.ballB];

		Vector2D delta = b.getPosition() - a.getPosition();
		float dist = delta.length();
		if (dist < 1e-6f) continue;

		Vector2D dir = delta / dist;
		float displacement = dist - spring.restLength;

		// Hooke's law: F = -k * x
		float springForce = spring.stiffness * displacement;

		// Damping: F = -c * relative velocity along spring axis
		Vector2D relVel = b.getVelocity() - a.getVelocity();
		float dampForce = spring.damping * relVel.dot(dir);

		float totalForce = springForce + dampForce;
		Vector2D forceVec = dir * totalForce;

		float invMassA = 1.0f / a.getMass();
		float invMassB = 1.0f / b.getMass();

		a.setVelocity(a.getVelocity() + forceVec * (dt * invMassA));
		b.setVelocity(b.getVelocity() - forceVec * (dt * invMassB));

		a.wake();
		b.wake();
	}
}
