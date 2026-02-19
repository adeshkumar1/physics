# 2D Ball Physics Simulation

A real-time 2D physics simulation built from scratch in C++20 with SFML 3. Spawn balls into a bounded box and watch them fall under gravity, bounce off walls, and collide with each other.

## Features

* **Circle-Circle Collision Detection** -- Distance-based overlap test between ball pairs, optimized with a sweep-and-prune pass (sort on X-axis, early exit when separation exceeds combined radii).
* **Impulse-Based Collision Resolution** -- Computes collision normals and applies mass-proportional impulse responses so smaller balls react more than larger ones.
* **Wall Bouncing** -- Balls reflect off all four walls of a centered bounding box with velocity clamping to prevent tunneling.
* **Gravity & Integration** -- Constant downward acceleration with per-frame Euler integration for position and velocity updates.
* **Interactive Spawning** -- Press **Enter** to spawn a new ball at the top of the box with a random radius (15--25 px), random velocity, and random color.

## Project Structure

```
Ball.h          Circle entity with position, velocity, radius, mass, and color
Box.h           Axis-aligned rectangular boundary
Vector2D.h/cpp  2D vector math (arithmetic, dot product, normalization)
Physics.h/cpp   Collision detection, resolution, wall bouncing, physics step
main.cpp        SFML window, event loop, rendering
CMakeLists.txt  Build configuration
```

## Prerequisites

* C++20 compiler (GCC 11+, Clang 14+, or MSVC 2022+)
* [CMake](https://cmake.org/) 3.16+
* [SFML 3](https://www.sfml-dev.org/)

## Build & Run

```bash
cmake -B build
cmake --build build
./build/main
```

## Controls

| Key | Action |
|-------|-------------------------------|
| Enter | Spawn a new ball |
| Close | Click the window close button |
