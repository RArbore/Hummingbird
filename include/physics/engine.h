#pragma once

#include <cstddef>
#include <vector>

#include "cli.h"

/**
 * @brief Context for physics calculations
 * Keeps track of physics objects positions / dynamics, checks for and responds to collisions
 */

class Engine {
public:
  Engine(const Config& cfg);

  void update(float dt);
private:
  // Constants / configuration
  float grav_constant;
  std::size_t num_bodies;

  // Dynamics data
  std::vector<float> x;
  std::vector<float> y;
  std::vector<float> z;

  std::vector<float> vx;
  std::vector<float> vy;
  std::vector<float> vz;

  std::vector<float> ax;
  std::vector<float> ay;
  std::vector<float> az;
  
};
