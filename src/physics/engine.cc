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

#include <physics/engine.h>

/*
 * For vector instructions, we need to allocate 
 * our memory aligned on 32-byte boundaries.
 */
using vector32f = std::vector<float, boost::alignment::aligned_allocator<float, 32>>;

/*
 * Construct engine based on configuration,
 * which provides some constants and bodies.
 */
Engine::Engine(const Config& cfg): grav_constant(cfg.grav_constant),
				   boundary{cfg.boundary[0], cfg.boundary[1], cfg.boundary[2], cfg.boundary[3], cfg.boundary[4], cfg.boundary[5]},
				   num_bodies(cfg.num_bodies),
				   force{vector32f(num_bodies, 0.0f), vector32f(num_bodies, 0.0f), vector32f(num_bodies, 0.0f)} {
  /*
   * Reserve space so  that we don't waste
   * heap allocations.
   */
  pos.x.reserve(num_bodies);
  pos.y.reserve(num_bodies);
  pos.z.reserve(num_bodies);

  vel.x.reserve(num_bodies);
  vel.y.reserve(num_bodies);
  vel.z.reserve(num_bodies);

  mass.reserve(num_bodies);
  ang_pos.reserve(num_bodies);
  colliders.reserve(num_bodies);

  /*
   * Config bodies are stored as variants,
   * so we use std::visit to deduce types.
   */
  for (auto vari : cfg.bodies) {
    std::visit([&](auto&& body) {
      using T = std::decay_t<decltype(body)>;
      if constexpr (std::is_same_v<T, ConfigSphere>) {
	pos.x.push_back(body.x);
	pos.y.push_back(body.y);
	pos.z.push_back(body.z);

	vel.x.push_back(body.vx);
	vel.y.push_back(body.vy);
	vel.z.push_back(body.vz);

	mass.push_back(body.m);
	ang_pos.push_back(Quaternion{0.0f, 0.0f, 0.0f, 0.0f});
	colliders.push_back(std::make_unique<SphereCollider>(body.r));
      }
    }, vari);
  }

  /*
   * Initialize y force vector w/ gravitational 
   * constant.
   */
  const __m256 grav_constant_a = _mm256_set_ps(-grav_constant, -grav_constant, -grav_constant, -grav_constant, -grav_constant, -grav_constant, -grav_constant, -grav_constant);
  multiply_with_mass(force.y.data(), mass.data(), -grav_constant, grav_constant_a);
}

/*
 * Getters for body data (used by graphics).
 */
const Engine::Vec3x<float, 32> &Engine::get_pos() const { return pos; }
const Engine::Vec3x<float, 32> &Engine::get_vel() const { return vel; }
const Engine::Vec3x<float, 32> &Engine::get_force() const { return force; }
const std::vector<float> &Engine::get_mass() const { return mass; }
const std::vector<Quaternion> &Engine::get_ang_pos() const { return ang_pos; }
const std::vector<std::unique_ptr<Collider>> &Engine::get_colliders() const { return colliders; }
std::size_t Engine::get_num_bodies() const { return num_bodies; }

void Engine::update(const float dt) {
  /*
   * Update positions / velocities / forces of 
   * bodies.
   */
  dynamics_update(dt);

  /*
   * Construct octree for collision detection.
   */
  auto octree = std::make_unique<Octree>(C_MAX_DEPTH, AABB{-100.0f, 100.0f, -100.0f, 100.0f, -100.0f, 100.0f});
  for (unsigned int i = 0; i < num_bodies; ++i) {
    octree->insert(i, get_aabb_at(i));
  }
}

void Engine::dynamics_update(const float dt) {
  /*
   * Initialize vector register filled with
   * dt, so that we can avoid reallocating
   * it for each operation.
   */
  const __m256 dt_a = _mm256_set_ps(dt, dt, dt, dt, dt, dt, dt, dt);

  /*
   * Update velocities from forces.
   */
  fused_multiply_add_with_mass(dt, dt_a, vel.x.data(), force.x.data(), mass.data());
  fused_multiply_add_with_mass(dt, dt_a, vel.y.data(), force.y.data(), mass.data());
  fused_multiply_add_with_mass(dt, dt_a, vel.z.data(), force.z.data(), mass.data());

  /*
   * Update positions from velocities.
   */
  fused_multiply_add(dt, dt_a, pos.x.data(), vel.x.data());
  fused_multiply_add(dt, dt_a, pos.y.data(), vel.y.data());
  fused_multiply_add(dt, dt_a, pos.z.data(), vel.z.data());
}

/*
 * Helper to perform a[i] = b[i] * c using
 * AVX instructions.
 */
void Engine::multiply_with_mass(float *const a, const float *const b, const float c, const __m256& c_a) {
  std::size_t i = 0;
  if (num_bodies >= 8) {
    for (; i <= num_bodies - 8; i += 8) {
      const __m256 b_a = _mm256_set_ps(b[i + 7], b[i + 6], b[i + 5], b[i + 4], b[i + 3], b[i + 2], b[i + 1], b[i]);
      const __m256 result = _mm256_mul_ps(b_a, c_a);
      _mm256_store_ps(a + i, result);
    }
  }
  for (; i < num_bodies; ++i) {
    a[i] = b[i] * c;
  }
}

/*
 * Helper to perform a[i] += b[i] * dt
 * using AVX instructions.
 */
void Engine::fused_multiply_add(const float dt, const __m256& dt_a, float *const a, const float *const b) {
  std::size_t i = 0;
  if (num_bodies >= 8) {
    for (; i <= num_bodies - 8; i += 8) {
      const __m256 a_a = _mm256_set_ps(a[i + 7], a[i + 6], a[i + 5], a[i + 4], a[i + 3], a[i + 2], a[i + 1], a[i]);
      const __m256 b_a = _mm256_set_ps(b[i + 7], b[i + 6], b[i + 5], b[i + 4], b[i + 3], b[i + 2], b[i + 1], b[i]);
      const __m256 result = _mm256_fmadd_ps(b_a, dt_a, a_a);
      _mm256_store_ps(a + i, result);
    }
  }
  for (; i < num_bodies; ++i) {
    a[i] += b[i] * dt;
  }
}

/*
 * Helper to perform a[i] += b[i] * dt / m[i]
 * using AVX instructions.
 */
void Engine::fused_multiply_add_with_mass(const float dt, const __m256& dt_a, float *const a, const float *const b, const float *const m) {
  std::size_t i = 0;
  if (num_bodies >= 8) {
    for (; i <= num_bodies - 8; i += 8) {
      const __m256 a_a = _mm256_set_ps(a[i + 7], a[i + 6], a[i + 5], a[i + 4], a[i + 3], a[i + 2], a[i + 1], a[i]);
      const __m256 b_a = _mm256_set_ps(b[i + 7], b[i + 6], b[i + 5], b[i + 4], b[i + 3], b[i + 2], b[i + 1], b[i]);
      const __m256 m_a = _mm256_set_ps(m[i + 7], m[i + 6], m[i + 5], m[i + 4], m[i + 3], m[i + 2], m[i + 1], m[i]);
      const __m256 result = _mm256_fmadd_ps(_mm256_div_ps(b_a, m_a), dt_a, a_a);
      _mm256_store_ps(a + i, result);
    }
  }
  for (; i < num_bodies; ++i) {
    a[i] += b[i] * dt  / m[i];
  }
}

Transform Engine::get_transform_at(const std::size_t i) {
  return Transform{pos.x.at(i), pos.y.at(i), pos.z.at(i)};
}

AABB Engine::get_aabb_at(const std::size_t i) {
  const auto& unknown_coll = colliders.at(i).get();
  float pos_x = pos.x.at(i);
  float pos_y = pos.y.at(i);
  float pos_z = pos.z.at(i);
  if (SphereCollider* coll = dynamic_cast<SphereCollider*>(unknown_coll)) {
    float rad = coll->radius;
    return AABB{pos_x - rad, pos_x + rad, pos_y - rad, pos_y + rad, pos_z - rad, pos_z + rad};
  }
  else return AABB{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
}
