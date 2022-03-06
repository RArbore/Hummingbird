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
#include <tuple>

#include <boost/align/aligned_allocator.hpp>

#include <physics/quaternion.h>
#include <physics/collider.h>
#include <physics/octree.h>
#include <cli.h>

/*
 * Engine represents the physics world we are
 * simulating. We are using data oriented design,
 * so each attribute common amongst bodies
 * (position, velocity, mass, etc) is stored as
 * its own vector - this way, we can very
 * efficiently update each attribute using vector
 * instructions. We provided getters for these
 * vectors so that the graphics context can access
 * bodies.
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
  const Vec3x<float, 32> &get_force() const;
  const std::vector<float> &get_mass() const;
  const std::vector<Quaternion> &get_ang_pos() const;
  const std::vector<std::unique_ptr<Collider>> &get_colliders() const;
  std::size_t get_num_bodies() const;
private:
  /*
   * Constants / configuration.
   */
  float grav_constant, boundary[6];
  std::size_t num_bodies;

  /*
   * Dynamics data, organized using data
   * oriented design.
   */
  Vec3x<float, 32> pos;
  Vec3x<float, 32> vel;
  Vec3x<float, 32> force;
  std::vector<float> mass;
  std::vector<Quaternion> ang_pos;
  std::vector<std::unique_ptr<Collider>> colliders;

  Transform get_transform_at(const std::size_t i);
  AABB get_aabb_at(const std::size_t i);
  void dynamics_update(const float dt);

  /*
   * Utility functions for performing vector operations.
   */
  void multiply_with_mass(float *const a, const float *const b, const float c, const __m256& c_a);
  void fused_multiply_add(const float dt, const __m256& dt_a, float *const a, const float *const b);
  void fused_multiply_add_with_mass(const float dt, const __m256& dt_a, float *const a, const float *const b, const float *const m);
};
