#pragma once

#include "entities/Ball.h"
#include "entities/Obstacle.h"
#include "math/Vector2D.h"
#include "physics/Forces.h"
#include <algorithm>
#include <cmath>
#include <vector>

// Helper: 2D cross product
inline float obsCross2D(const Vector2D &a, const Vector2D &b) {
	return a.getX() * b.getY() - a.getY() * b.getX();
}

inline void resolveCircleVsRect(Ball &ball, const RectObstacle &rect,
                                std::vector<ContactInfo> &contacts) {
	if (ball.isSleeping()) return;

	Vector2D pos = ball.getPosition();
	float r = ball.getRadius();

	// Find closest point on AABB to ball center
	float halfW = rect.width / 2.0f;
	float halfH = rect.height / 2.0f;
	float closestX =
	    std::clamp(pos.getX(), rect.position.getX() - halfW, rect.position.getX() + halfW);
	float closestY =
	    std::clamp(pos.getY(), rect.position.getY() - halfH, rect.position.getY() + halfH);

	Vector2D closest(closestX, closestY);
	Vector2D delta = pos - closest;
	float distSq = delta.lengthSquared();

	if (distSq >= r * r || distSq < 1e-8f) return;

	float dist = std::sqrt(distSq);
	Vector2D normal = delta / dist;
	float penetration = r - dist;

	// Position correction
	ball.setPosition(pos + normal * penetration);

	// Impulse (same as wall collision)
	Vector2D vel = ball.getVelocity();
	float angVel = ball.getAngularVelocity();
	Vector2D contactOffset = normal * (-r);
	Vector2D rotVel(-contactOffset.getY() * angVel,
	                contactOffset.getX() * angVel);
	Vector2D contactVel = vel + rotVel;

	float velAlongNormal = contactVel.dot(normal);
	if (velAlongNormal >= 0.0f) return;

	const Material &mat = ball.getMaterial();
	float invMass = 1.0f / ball.getMass();
	float invInertia = 1.0f / ball.getInertia();
	float rCrossN = obsCross2D(contactOffset, normal);
	float effectiveInvMass = invMass + rCrossN * rCrossN * invInertia;

	float j = -(1.0f + mat.restitution) * velAlongNormal / effectiveInvMass;

	vel += normal * (j * invMass);
	angVel += obsCross2D(contactOffset, normal * j) * invInertia;

	// Friction
	Vector2D tangent = contactVel - normal * contactVel.dot(normal);
	float tangentSpeed = tangent.length();
	if (tangentSpeed > 1e-6f) {
		tangent = tangent / tangentSpeed;
		float jt =
		    std::min(mat.friction * std::abs(j), tangentSpeed / effectiveInvMass);
		vel -= tangent * (jt * invMass);
		angVel += obsCross2D(contactOffset, tangent * (-jt)) * invInertia;
	}

	ball.setVelocity(vel);
	ball.setAngularVelocity(angVel);
	ball.wake();

	contacts.push_back({closest, normal, std::abs(j), ball.getColor()});
}

inline void resolveCircleVsLine(Ball &ball, const LineObstacle &line,
                                std::vector<ContactInfo> &contacts) {
	if (ball.isSleeping()) return;

	Vector2D pos = ball.getPosition();
	float r = ball.getRadius();

	// Project ball center onto line segment
	Vector2D lineDir = line.end - line.start;
	float lineLen = lineDir.length();
	if (lineLen < 1e-6f) return;
	Vector2D lineNorm = lineDir / lineLen;

	float t = (pos - line.start).dot(lineNorm);
	t = std::clamp(t, 0.0f, lineLen);

	Vector2D closest = line.start + lineNorm * t;
	Vector2D delta = pos - closest;
	float distSq = delta.lengthSquared();

	if (distSq >= r * r || distSq < 1e-8f) return;

	float dist = std::sqrt(distSq);
	Vector2D normal = delta / dist;
	float penetration = r - dist;

	ball.setPosition(pos + normal * penetration);

	// Impulse resolution (same pattern as rect)
	Vector2D vel = ball.getVelocity();
	float angVel = ball.getAngularVelocity();
	Vector2D contactOffset = normal * (-r);
	Vector2D rotVel(-contactOffset.getY() * angVel,
	                contactOffset.getX() * angVel);
	Vector2D contactVel = vel + rotVel;

	float velAlongNormal = contactVel.dot(normal);
	if (velAlongNormal >= 0.0f) return;

	const Material &mat = ball.getMaterial();
	float invMass = 1.0f / ball.getMass();
	float invInertia = 1.0f / ball.getInertia();
	float rCrossN = obsCross2D(contactOffset, normal);
	float effectiveInvMass = invMass + rCrossN * rCrossN * invInertia;

	float j = -(1.0f + mat.restitution) * velAlongNormal / effectiveInvMass;

	vel += normal * (j * invMass);
	angVel += obsCross2D(contactOffset, normal * j) * invInertia;

	Vector2D tangent = contactVel - normal * contactVel.dot(normal);
	float tangentSpeed = tangent.length();
	if (tangentSpeed > 1e-6f) {
		tangent = tangent / tangentSpeed;
		float jt =
		    std::min(mat.friction * std::abs(j), tangentSpeed / effectiveInvMass);
		vel -= tangent * (jt * invMass);
		angVel += obsCross2D(contactOffset, tangent * (-jt)) * invInertia;
	}

	ball.setVelocity(vel);
	ball.setAngularVelocity(angVel);
	ball.wake();

	contacts.push_back({closest, normal, std::abs(j), ball.getColor()});
}

inline void resolveObstacleCollisions(
    std::vector<Ball> &balls,
    const std::vector<RectObstacle> &rects,
    const std::vector<LineObstacle> &lines,
    std::vector<ContactInfo> &contacts) {
	for (auto &ball : balls) {
		for (const auto &rect : rects) {
			resolveCircleVsRect(ball, rect, contacts);
		}
		for (const auto &line : lines) {
			resolveCircleVsLine(ball, line, contacts);
		}
	}
}
