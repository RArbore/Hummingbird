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

#include <immintrin.h>
#include <cstddef>
#include <variant>
#include <vector>
#include <memory>

#include <boost/align/aligned_allocator.hpp>

#include <physics/quaternion.h>
#include <physics/collider.h>
#include <cli.h>

/**
 * @brief Context for physics calculations
 * Keeps track of physics objects positions / dynamics, checks for and responds to collisions
 */

class Engine {
public:
  explicit Engine(const Config& cfg);

  void update(const float dt);

  template <typename T, std::size_t align>
  struct Vec3x {
    std::vector<T, boost::alignment::aligned_allocator<T, align>> x;
    std::vector<T, boost::alignment::aligned_allocator<T, align>> y;
    std::vector<T, boost::alignment::aligned_allocator<T, align>> z;
  };

  const Vec3x<float, 32> &get_pos() const;
  const Vec3x<float, 32> &get_vel() const;
  const Vec3x<float, 32> &get_acc() const;
  const std::vector<float> &get_mass() const;
  const std::vector<Quaternion> &get_ang_pos() const;
  const std::vector<std::unique_ptr<Collider>> &get_colliders() const;
  std::size_t get_num_bodies() const;
private:
  // Constants / configuration
  float grav_constant;
  std::size_t num_bodies;

  // Dynamics data
  Vec3x<float, 32> pos;
  Vec3x<float, 32> vel;
  Vec3x<float, 32> acc;
  float cog_x, cog_y, cog_z, cog_vx, cog_vy, cog_vz, cog_m;
  std::vector<float> mass;
  std::vector<Quaternion> ang_pos;
  std::vector<std::unique_ptr<Collider>> colliders;

  Transform get_transform_at(const std::size_t i);
  void gravity_acc_update(const float dt);
  void dynamics_update(const float dt);
  void fused_multiply_add(const float dt, __m256& dt_a, float *const a, const float *const b);
};
