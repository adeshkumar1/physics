#include "physics/Physics.h"
#include "physics/ObstacleCollision.h"
#include "physics/SpatialGrid.h"
#include <algorithm>
#include <cmath>
#include <unordered_set>

// --- Helper: combine materials for a collision pair ---

static Material combineMaterials(const Material &a, const Material &b) {
	return {
	    std::min(a.restitution, b.restitution),
	    std::sqrt(a.friction * b.friction),
	};
}

// --- Helper: 2D cross product (scalar result for 2D) ---

static float cross2D(const Vector2D &a, const Vector2D &b) {
	return a.getX() * b.getY() - a.getY() * b.getX();
}

// --- Phase 1: Integrate forces (gravity + quadratic drag) → velocity ---

static void integrateForces(std::vector<Ball> &balls, float dt,
                            const PhysicsConfig &config) {
	for (auto &ball : balls) {
		if (ball.isSleeping()) continue;

		Vector2D vel = ball.getVelocity();

		// Gravity
		vel.setY(vel.getY() + config.gravity * dt);

		// Quadratic air drag: F = -c * |v| * v
		float speed = vel.length();
		if (speed > 1e-6f) {
			float dragMag = config.airDrag * speed * speed;
			Vector2D dragForce = vel.normalized() * (-dragMag);
			vel += dragForce * (dt / ball.getMass());
		}

		ball.setVelocity(vel);
	}
}

// --- Phase 2: Integrate positions using updated velocity ---

static void integratePositions(std::vector<Ball> &balls, float dt) {
	for (auto &ball : balls) {
		if (ball.isSleeping()) continue;

		ball.setPosition(ball.getPosition() + ball.getVelocity() * dt);
		ball.setAngle(ball.getAngle() + ball.getAngularVelocity() * dt);
	}
}

// --- Phase 3: Resolve wall collisions (impulse-based with friction) ---

static void resolveWallCollisions(std::vector<Ball> &balls, const Box &box,
                                  std::vector<ContactInfo> &contacts,
                                  float wallRestitution) {
	float halfW = box.getWidth() / 2.0f;
	float halfH = box.getHeight() / 2.0f;
	float cx = box.getX();
	float cy = box.getY();

	float left = cx - halfW;
	float right = cx + halfW;
	float top = cy - halfH;
	float bottom = cy + halfH;

	for (auto &ball : balls) {
		if (ball.isSleeping()) continue;

		Vector2D pos = ball.getPosition();
		Vector2D vel = ball.getVelocity();
		float r = ball.getRadius();
		float angVel = ball.getAngularVelocity();
		const Material &mat = ball.getMaterial();

		auto applyWallImpulse = [&](Vector2D normal, float penetration) {
			pos += normal * penetration;

			Vector2D contactOffset = normal * (-r);
			Vector2D rotVel(-contactOffset.getY() * angVel,
			                contactOffset.getX() * angVel);
			Vector2D contactVel = vel + rotVel;

			float velAlongNormal = contactVel.dot(normal);
			if (velAlongNormal >= 0.0f) return;

			float invMass = 1.0f / ball.getMass();
			float invInertia = 1.0f / ball.getInertia();

			float rCrossN = cross2D(contactOffset, normal);
			float effectiveInvMass =
			    invMass + rCrossN * rCrossN * invInertia;

			float j = -(1.0f + wallRestitution) * velAlongNormal /
			          effectiveInvMass;

			vel += normal * (j * invMass);
			angVel += cross2D(contactOffset, normal * j) * invInertia;

			// Friction
			Vector2D tangent = contactVel - normal * contactVel.dot(normal);
			float tangentSpeed = tangent.length();
			if (tangentSpeed > 1e-6f) {
				tangent = tangent / tangentSpeed;
				float jt = std::min(mat.friction * std::abs(j),
				                    tangentSpeed / effectiveInvMass);
				vel -= tangent * (jt * invMass);
				angVel += cross2D(contactOffset, tangent * (-jt)) * invInertia;
			}

			// Report contact for particles
			Vector2D contactPt = pos - normal * r;
			contacts.push_back({contactPt, normal, std::abs(j), ball.getColor()});
		};

		if (pos.getX() - r < left) {
			applyWallImpulse(Vector2D(1.0f, 0.0f), left - (pos.getX() - r));
		} else if (pos.getX() + r > right) {
			applyWallImpulse(Vector2D(-1.0f, 0.0f), (pos.getX() + r) - right);
		}

		if (pos.getY() - r < top) {
			applyWallImpulse(Vector2D(0.0f, 1.0f), top - (pos.getY() - r));
		} else if (pos.getY() + r > bottom) {
			applyWallImpulse(Vector2D(0.0f, -1.0f), (pos.getY() + r) - bottom);
		}

		ball.setPosition(pos);
		ball.setVelocity(vel);
		ball.setAngularVelocity(angVel);
	}
}

// --- Phase 4: Broad phase + narrow phase + resolve ---

static bool checkCollision(const Ball &ball1, const Ball &ball2) {
	Vector2D delta = ball1.getPosition() - ball2.getPosition();
	float radiiSum = ball1.getRadius() + ball2.getRadius();
	return delta.lengthSquared() <= radiiSum * radiiSum;
}

static void resolveCollision(Ball &ball1, Ball &ball2,
                             std::vector<ContactInfo> &contacts) {
	Vector2D delta = ball2.getPosition() - ball1.getPosition();
	float dist = delta.length();
	float radiiSum = ball1.getRadius() + ball2.getRadius();

	if (dist >= radiiSum || dist < 1e-6f) return;

	Vector2D normal = delta / dist;

	float overlap = radiiSum - dist;
	float totalMass = ball1.getMass() + ball2.getMass();
	ball1.setPosition(ball1.getPosition() -
	                  normal * (overlap * ball2.getMass() / totalMass));
	ball2.setPosition(ball2.getPosition() +
	                  normal * (overlap * ball1.getMass() / totalMass));

	Material mat =
	    combineMaterials(ball1.getMaterial(), ball2.getMaterial());

	float invMass1 = 1.0f / ball1.getMass();
	float invMass2 = 1.0f / ball2.getMass();
	float invInertia1 = 1.0f / ball1.getInertia();
	float invInertia2 = 1.0f / ball2.getInertia();

	Vector2D r1 = normal * ball1.getRadius();
	Vector2D r2 = normal * (-ball2.getRadius());

	Vector2D rotVel1(-r1.getY() * ball1.getAngularVelocity(),
	                 r1.getX() * ball1.getAngularVelocity());
	Vector2D rotVel2(-r2.getY() * ball2.getAngularVelocity(),
	                 r2.getX() * ball2.getAngularVelocity());
	Vector2D contactVel1 = ball1.getVelocity() + rotVel1;
	Vector2D contactVel2 = ball2.getVelocity() + rotVel2;
	Vector2D relativeVel = contactVel1 - contactVel2;

	float velAlongNormal = relativeVel.dot(normal);
	if (velAlongNormal < 0) return;

	float normalImpulse = -(1.0f + mat.restitution) * velAlongNormal /
	                       (invMass1 + invMass2);

	Vector2D impulse = normal * normalImpulse;
	ball1.setVelocity(ball1.getVelocity() + impulse * invMass1);
	ball2.setVelocity(ball2.getVelocity() - impulse * invMass2);

	ball1.setAngularVelocity(ball1.getAngularVelocity() +
	                         cross2D(r1, impulse) * invInertia1);
	ball2.setAngularVelocity(ball2.getAngularVelocity() -
	                         cross2D(r2, impulse) * invInertia2);

	// Friction
	Vector2D tangent = relativeVel - normal * velAlongNormal;
	float tangentSpeed = tangent.length();
	if (tangentSpeed > 1e-6f) {
		tangent = tangent / tangentSpeed;
		float maxFriction = mat.friction * std::abs(normalImpulse);
		float frictionImpulse =
		    std::min(maxFriction, tangentSpeed / (invMass1 + invMass2));

		Vector2D fImpulse = tangent * (-frictionImpulse);
		ball1.setVelocity(ball1.getVelocity() + fImpulse * invMass1);
		ball2.setVelocity(ball2.getVelocity() - fImpulse * invMass2);

		ball1.setAngularVelocity(ball1.getAngularVelocity() +
		                         cross2D(r1, fImpulse) * invInertia1);
		ball2.setAngularVelocity(ball2.getAngularVelocity() -
		                         cross2D(r2, fImpulse) * invInertia2);
	}

	ball1.wake();
	ball2.wake();

	// Report contact for particles
	Vector2D contactPt =
	    ball1.getPosition() + normal * ball1.getRadius();
	contacts.push_back(
	    {contactPt, normal, std::abs(normalImpulse), ball1.getColor()});
}

static void broadPhaseAndResolve(std::vector<Ball> &balls, const Box &box,
                                 std::vector<ContactInfo> &contacts) {
	static SpatialGrid grid;
	static bool gridConfigured = false;

	if (!gridConfigured) {
		float maxRadius = 0.0f;
		for (const auto &b : balls) {
			if (b.getRadius() > maxRadius) maxRadius = b.getRadius();
		}
		grid.configure(box.getWidth(), box.getHeight(), box.getX(), box.getY(),
		               maxRadius > 0.0f ? maxRadius : 25.0f);
		gridConfigured = true;
	}

	grid.clear();
	for (size_t i = 0; i < balls.size(); i++) {
		if (balls[i].isSleeping()) continue;
		grid.insert(i, balls[i].getPosition().getX(),
		            balls[i].getPosition().getY(), balls[i].getRadius());
	}

	struct PairHash {
		size_t operator()(std::pair<size_t, size_t> p) const {
			return std::hash<size_t>()(p.first) ^
			       (std::hash<size_t>()(p.second) << 32);
		}
	};
	std::unordered_set<std::pair<size_t, size_t>, PairHash> checked;

	grid.forEachPair([&](size_t a, size_t b) {
		if (!checked.insert({a, b}).second) return;
		if (checkCollision(balls[a], balls[b])) {
			resolveCollision(balls[a], balls[b], contacts);
		}
	});
}

// --- Phase 5: Speed clamping ---

static void clampSpeeds(std::vector<Ball> &balls, float maxSpeed) {
	float maxSpeedSq = maxSpeed * maxSpeed;
	for (auto &ball : balls) {
		if (ball.isSleeping()) continue;

		Vector2D vel = ball.getVelocity();
		if (vel.lengthSquared() > maxSpeedSq) {
			ball.setVelocity(vel.normalized() * maxSpeed);
		}
	}
}

// --- Phase 6: Sleep detection ---

static void updateSleeping(std::vector<Ball> &balls, float dt,
                           const PhysicsConfig &config) {
	float threshold =
	    config.sleepVelocityThreshold * config.sleepVelocityThreshold;
	for (auto &ball : balls) {
		float energy = ball.getVelocity().lengthSquared() +
		               ball.getAngularVelocity() * ball.getAngularVelocity() *
		                   ball.getRadius() * ball.getRadius();
		if (energy < threshold) {
			ball.setSleepTimer(ball.getSleepTimer() + dt);
			if (ball.getSleepTimer() >= config.sleepTimeRequired) {
				ball.setVelocity(Vector2D(0.0f, 0.0f));
				ball.setAngularVelocity(0.0f);
				ball.setSleeping(true);
			}
		} else {
			ball.wake();
		}
	}
}

// --- Main entry point: sub-stepped physics update ---

void updatePhysics(PhysicsWorld &world, const Box &box, float timeStep,
                   const PhysicsConfig &config) {
	world.contacts.clear();
	float subDt = timeStep / static_cast<float>(config.subSteps);

	for (int step = 0; step < config.subSteps; step++) {
		integrateForces(world.balls, subDt, config);
		applyAttractors(world.balls, world.attractors, subDt);
		applySprings(world.balls, world.springs, subDt);
		integratePositions(world.balls, subDt);
		resolveWallCollisions(world.balls, box, world.contacts,
		                      config.wallRestitution);
		resolveObstacleCollisions(world.balls, world.rectObstacles,
		                          world.lineObstacles, world.contacts);
		broadPhaseAndResolve(world.balls, box, world.contacts);
		clampSpeeds(world.balls, config.maxSpeed);
	}

	updateSleeping(world.balls, timeStep, config);
}
