#pragma once

#include <cstddef>
#include <vector>

#include "physics/collider.h"
#include "cli.h"

/**
 * @brief Context for physics calculations
 * Keeps track of physics objects positions / dynamics, checks for and responds to collisions
 */

class Engine {
public:
  Engine(const Config& cfg);

  void update(const float dt);
private:
  // Constants / configuration
  float grav_constant;
  std::size_t num_bodies;

  // Dynamics data
  template <typename T>
  struct Vec3x {
    std::vector<T> x;
    std::vector<T> y;
    std::vector<T> z;
  };

  Vec3x<float> pos;
  Vec3x<float> vel;
  Vec3x<float> acc;
  std::vector<float> mass;
  std::vector<std::unique_ptr<Collider>> colliders;

  Transform getTransformAt(const std::size_t i);
};
