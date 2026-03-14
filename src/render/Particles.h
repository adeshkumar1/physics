#pragma once

#include "entities/Ball.h"
#include "math/Vector2D.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstdint>
#include <random>
#include <vector>

struct Particle {
	Vector2D position;
	Vector2D velocity;
	float lifetime;
	float maxLifetime;
	uint8_t r, g, b;
};

class ParticleSystem {
 private:
	std::vector<Particle> particles;
	std::default_random_engine rng;
	std::uniform_real_distribution<float> angleDist{0.0f, 6.2832f};
	std::uniform_real_distribution<float> speedDist{30.0f, 150.0f};
	std::uniform_real_distribution<float> lifeDist{0.15f, 0.4f};

 public:
	ParticleSystem() {
		std::random_device rd;
		rng.seed(rd());
		particles.reserve(2000);
	}

	void emit(Vector2D pos, Vector2D normal, float impulse, Color color,
	          int count) {
		float baseSpeed = std::min(impulse * 0.3f, 200.0f);
		for (int i = 0; i < count; i++) {
			float angle = angleDist(rng);
			float speed = baseSpeed + speedDist(rng);
			// Bias toward collision normal
			Vector2D vel(
			    normal.getX() * speed * 0.5f + std::cos(angle) * speed * 0.5f,
			    normal.getY() * speed * 0.5f + std::sin(angle) * speed * 0.5f);
			float life = lifeDist(rng);
			particles.push_back({pos, vel, life, life, color.r, color.g, color.b});
		}
	}

	void update(float dt) {
		for (auto &p : particles) {
			p.position = p.position + p.velocity * dt;
			p.velocity = p.velocity * (1.0f - 3.0f * dt);  // quick decay
			p.lifetime -= dt;
		}
		// Remove dead particles
		particles.erase(
		    std::remove_if(particles.begin(), particles.end(),
		                   [](const Particle &p) { return p.lifetime <= 0.0f; }),
		    particles.end());
	}

	void draw(sf::RenderWindow &window) const {
		for (const auto &p : particles) {
			float alpha = (p.lifetime / p.maxLifetime);
			float size = 2.0f * alpha + 0.5f;
			sf::CircleShape shape(size);
			shape.setFillColor(sf::Color(p.r, p.g, p.b,
			                             static_cast<uint8_t>(alpha * 255)));
			shape.setPosition({p.position.getX() - size,
			                   p.position.getY() - size});
			window.draw(shape);
		}
	}

	size_t count() const { return particles.size(); }
};
