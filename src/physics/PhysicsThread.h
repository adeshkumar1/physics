#pragma once

#include "entities/Ball.h"
#include "entities/Box.h"
#include "physics/Physics.h"
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

class PhysicsThread {
 private:
	std::thread worker;
	mutable std::mutex swapMutex;
	std::atomic<bool> running{false};

	PhysicsWorld backWorld;           // physics writes
	std::vector<Ball> frontSnapshot;  // renderer reads

	Box box;
	PhysicsConfig config;

	// Pending additions (thread-safe)
	mutable std::mutex addMutex;
	std::vector<Ball> pendingBalls;
	std::vector<Attractor> pendingAttractors;
	std::vector<Spring> pendingSprings;
	std::vector<RectObstacle> pendingRects;
	std::vector<LineObstacle> pendingLines;
	bool clearAttractors = false;
	bool clearSprings = false;
	bool clearBallsFlag = false;

 public:
	PhysicsThread(const Box &box, const PhysicsConfig &config)
	    : box(box), config(config) {}

	~PhysicsThread() { stop(); }

	void start() {
		running = true;
		worker = std::thread([this]() {
			auto lastTime = std::chrono::steady_clock::now();
			float targetDt = 1.0f / 120.0f;

			while (running) {
				auto now = std::chrono::steady_clock::now();
				float elapsed =
				    std::chrono::duration<float>(now - lastTime).count();
				lastTime = now;

				// Drain pending items
				{
					std::lock_guard<std::mutex> lock(addMutex);
					for (auto &b : pendingBalls)
						backWorld.balls.push_back(b);
					pendingBalls.clear();

					for (auto &a : pendingAttractors)
						backWorld.attractors.push_back(a);
					pendingAttractors.clear();

					for (auto &s : pendingSprings)
						backWorld.springs.push_back(s);
					pendingSprings.clear();

					for (auto &r : pendingRects)
						backWorld.rectObstacles.push_back(r);
					pendingRects.clear();

					for (auto &l : pendingLines)
						backWorld.lineObstacles.push_back(l);
					pendingLines.clear();

					if (clearAttractors) {
						backWorld.attractors.clear();
						clearAttractors = false;
					}
					if (clearSprings) {
						backWorld.springs.clear();
						clearSprings = false;
					}
					if (clearBallsFlag) {
						backWorld.balls.clear();
						backWorld.springs.clear();
						clearBallsFlag = false;
					}
				}

				if (elapsed > 0.05f) elapsed = 0.05f;

				updatePhysics(backWorld, box, elapsed, config);

				// Swap snapshot
				{
					std::lock_guard<std::mutex> lock(swapMutex);
					frontSnapshot = backWorld.balls;
				}

				// Sleep to target ~120 Hz
				auto frameEnd = std::chrono::steady_clock::now();
				auto frameDuration = frameEnd - now;
				auto target = std::chrono::duration<float>(targetDt);
				if (frameDuration < target) {
					std::this_thread::sleep_for(target - frameDuration);
				}
			}
		});
	}

	void stop() {
		running = false;
		if (worker.joinable()) worker.join();
	}

	void addBall(const Ball &ball) {
		std::lock_guard<std::mutex> lock(addMutex);
		pendingBalls.push_back(ball);
	}

	void addAttractor(const Attractor &a) {
		std::lock_guard<std::mutex> lock(addMutex);
		pendingAttractors.push_back(a);
	}

	void addSpring(const Spring &s) {
		std::lock_guard<std::mutex> lock(addMutex);
		pendingSprings.push_back(s);
	}

	void addRectObstacle(const RectObstacle &r) {
		std::lock_guard<std::mutex> lock(addMutex);
		pendingRects.push_back(r);
	}

	void addLineObstacle(const LineObstacle &l) {
		std::lock_guard<std::mutex> lock(addMutex);
		pendingLines.push_back(l);
	}

	void clearAllAttractors() {
		std::lock_guard<std::mutex> lock(addMutex);
		clearAttractors = true;
	}

	void clearAllSprings() {
		std::lock_guard<std::mutex> lock(addMutex);
		clearSprings = true;
	}

	void clearAllBalls() {
		std::lock_guard<std::mutex> lock(addMutex);
		clearBallsFlag = true;
	}

	std::vector<Ball> getSnapshot() const {
		std::lock_guard<std::mutex> lock(swapMutex);
		return frontSnapshot;
	}

	// Get contact info for particles (consumed once)
	std::vector<ContactInfo> getContacts() const {
		std::lock_guard<std::mutex> lock(swapMutex);
		return backWorld.contacts;
	}

	// Read-only accessors for drawing
	std::vector<Attractor> getAttractors() const {
		std::lock_guard<std::mutex> lock(swapMutex);
		return backWorld.attractors;
	}

	std::vector<Spring> getSprings() const {
		std::lock_guard<std::mutex> lock(swapMutex);
		return backWorld.springs;
	}

	std::vector<RectObstacle> getRectObstacles() const {
		std::lock_guard<std::mutex> lock(swapMutex);
		return backWorld.rectObstacles;
	}

	std::vector<LineObstacle> getLineObstacles() const {
		std::lock_guard<std::mutex> lock(swapMutex);
		return backWorld.lineObstacles;
	}

	size_t getBallCount() const {
		std::lock_guard<std::mutex> lock(swapMutex);
		return frontSnapshot.size();
	}

	PhysicsConfig &getConfig() { return config; }
	const Box &getBox() const { return box; }
};
