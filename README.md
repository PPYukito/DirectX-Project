# DirectX Project

## About

**DirectX Project is my personal learning hub for C++, DirectX, and the fundamentals of game programming.**

My goal is to understand how games and game engines work under the hood by implementing systems myself, experimenting with different techniques, and learning from both successes and failures.

Rather than relying entirely on an existing game engine, I want to explore the underlying mathematics, rendering techniques, physics, and architecture that make real-time interactive applications possible.

This project will grow alongside my learning. Whenever I discover a new concept I want to understand, I want to experiment with it here, implement it, and see how it works in practice.

## Technology

- **Language:** C++
- **Graphics API:** DirectX (planned)
- **Shader Language:** HLSL (planned)
- **Platform:** Windows
- **IDE:** Visual Studio
- **Window Management:** Win32 API
- **Version Control:** Git

## What I Want to Learn

### C++ and Software Architecture
- Object-oriented programming and memory management
- Classes, inheritance, interfaces, and composition
- Resource management and application architecture
- Game loops, input handling, and time management
- How to structure and maintain a growing C++ project

### Mathematics for Game Programming
- Vectors and vector operations
- Matrices and matrix transformations
- Translation, rotation, and scaling
- Coordinate systems and 3D transformations
- World, view, and projection matrices
- Linear algebra and its applications in graphics and physics

### Graphics Programming and DirectX
- How the graphics pipeline works
- Vertex and index buffers
- Vertex and pixel shaders
- HLSL shader programming
- Camera systems and 3D rendering
- Lighting models, normals, and material properties
- Shadows, reflections, and water rendering
- Terrain rendering and environmental effects

### Physics and Simulation
- Newtonian mechanics
- Gravity, acceleration, velocity, and forces
- Collision detection and response
- Rigid-body dynamics
- Rotational motion and angular velocity
- Torque, friction, and suspension
- Fixed time steps and numerical integration

### Game and Engine Development
- Designing a basic game engine
- Separating rendering, physics, input, and gameplay
- Scene and object management
- Resource loading and management
- Performance profiling and optimization
- Understanding how existing engines solve common problems

## Long-Term Experiment: ATV Physics

One of my long-term goals is to develop an ATV traversal experience that can serve as a practical testbed for the concepts I learn.

A particular area of interest is vehicle physics. Inspired by a teacher who developed his own car torque engine, I want to understand and eventually implement my own torque-based vehicle simulation.

I want to investigate how steering, engine torque, wheel rotation, traction, friction, suspension, gravity, and terrain interact to produce believable ATV movement.

Rather than treating vehicle movement as a simple animation, I want to understand the mathematics and physical principles behind it.

## Development Philosophy

- **Understand before abstracting.** Learn how a system works before wrapping it in layers of architecture.
- **Build incrementally.** Start with small, testable features and expand them over time.
- **Experiment freely.** Use this project to test ideas, compare approaches, and investigate unfamiliar concepts.
- **Learn by implementing.** Prefer hands-on experiments that reveal how the underlying systems behave.
- **Keep improving.** Refactor earlier implementations as my understanding develops.

The objective is not to build a complete engine immediately. It is to gradually develop a deeper understanding of the technology by building increasingly sophisticated systems.

## Current Status

The project is in its early stages.

- [x] Set up the Visual Studio C++ project
- [x] Create a Win32 application window
- [x] Implement basic Windows message handling
- [x] Add a window-close confirmation dialog
- [ ] Initialize DirectX
- [ ] Render the first frame
- [ ] Explore the graphics pipeline
- [ ] Implement basic 2D and 3D mathematics
- [ ] Render meshes and build a camera
- [ ] Experiment with shaders and lighting
- [ ] Develop terrain and physics experiments
- [ ] Begin implementing ATV movement and torque simulation

## Final Goal

I want DirectX Project to become a growing collection of practical experiments, implementations, and discoveries about how games work.

It is not just about making a game. It is about understanding the systems behind the game — from a matrix transforming a vertex to a shader calculating light, from gravity affecting a vehicle to torque making its wheels move.

**The project is my workshop for learning how games are built, one system at a time.**
