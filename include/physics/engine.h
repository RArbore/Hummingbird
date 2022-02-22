/*  This file is part of Hummingbird.
    Hummingbird is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
    Hummingbird is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Hummingbird. If not, see <https://www.gnu.org/licenses/>.  */

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
