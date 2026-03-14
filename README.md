# 2D Ball Physics Simulation

A real-time 2D physics simulation built from scratch in C++20 with SFML 3. Spawn balls into a bounded box and watch them fall under gravity, bounce off walls, collide, spin, and settle to rest.

## Features

* **Impulse-Based Collision Resolution** -- Mass-proportional normal and tangential impulses for both ball-ball and ball-wall collisions.
* **Friction** -- Coulomb friction model applies tangential impulses at contact points, causing realistic energy loss and spin.
* **Angular Velocity & Rotation** -- Balls spin from off-center collisions and friction torque. Each ball displays a rotation indicator line.
* **Quadratic Air Drag** -- Realistic velocity-squared drag model instead of linear damping.
* **Sub-Stepped Integration** -- 4 sub-steps per frame using semi-implicit Euler for stable stacking and reduced tunneling.
* **Sleeping Bodies** -- Balls at rest deactivate after 0.5s, skipping physics computation. They wake on collision. Sleeping balls render semi-transparent.
* **Per-Ball Materials** -- Each ball has its own restitution and friction. Collisions combine materials using `min(e1, e2)` and `sqrt(f1 * f2)`.
* **Spatial Grid Broad Phase** -- O(n) uniform grid replaces O(n log n) sweep-and-prune for scalable collision detection.
* **Configurable Physics** -- All constants (gravity, drag, friction, sub-steps, sleep threshold) live in a `PhysicsConfig` struct, tunable at runtime.
* **Debug Visualization** -- Toggle velocity vectors and see sleeping state with a keypress.
* **HUD Overlay** -- FPS, ball count, and sleeping count displayed on screen.

## Project Structure

```
src/
├── main.cpp                 Entry point, SFML rendering, event loop, HUD
├── math/
│   ├── Vector2D.h           2D vector class with arithmetic operators
│   └── Vector2D.cpp         Vector math implementation
├── entities/
│   ├── Ball.h               Ball entity with position, velocity, rotation, material
│   └── Box.h                Axis-aligned rectangular boundary
└── physics/
    ├── Physics.h            PhysicsConfig struct and update API
    ├── Physics.cpp           Phased pipeline: integrate, collide, sleep
    └── SpatialGrid.h        Uniform grid for O(n) broad-phase collision
CMakeLists.txt               Build configuration
```

## Prerequisites

* C++20 compiler (GCC 11+, Clang 14+, or MSVC 2022+)
* [CMake](https://cmake.org/) 3.16+
* [SFML 3](https://www.sfml-dev.org/)

## Build & Run

```bash
cmake -B build
cmake --build build
./build/physics_sim
```

## Controls

| Key   | Action                       |
|-------|------------------------------|
| Enter | Spawn a new ball             |
| D     | Toggle debug mode (velocity vectors) |
| Close | Click the window close button |

## Physics Pipeline

Each frame runs N sub-steps (default 4), each executing:

1. **Integrate forces** -- Apply gravity and quadratic drag to velocity
2. **Integrate positions** -- Update position and rotation angle
3. **Wall collisions** -- Impulse-based resolution with friction and torque
4. **Broad phase** -- Insert balls into spatial grid, generate candidate pairs
5. **Narrow phase & resolve** -- Distance check, then normal + friction impulses
6. **Speed clamping** -- Cap velocity to prevent instability

After all sub-steps, sleeping detection runs once per frame.
