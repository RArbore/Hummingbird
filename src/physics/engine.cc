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

using vector32f = std::vector<float, boost::alignment::aligned_allocator<float, 32>>;

static constexpr float MAX_FORCE = 100.0f;

Engine::Engine(const Config& cfg): grav_constant(cfg.grav_constant),
				   num_bodies(cfg.num_bodies),
				   force{vector32f(num_bodies, 0.0f), vector32f(num_bodies, 0.0f), vector32f(num_bodies, 0.0f)},
				   cog_x(0.0f), cog_y(0.0f), cog_z(0.0f), cog_vx(0.0f), cog_vy(0.0f), cog_vz(0.0f), cog_m(0.0f) {
  pos.x.reserve(num_bodies);
  pos.y.reserve(num_bodies);
  pos.z.reserve(num_bodies);

  vel.x.reserve(num_bodies);
  vel.y.reserve(num_bodies);
  vel.z.reserve(num_bodies);

  mass.reserve(num_bodies);
  ang_pos.reserve(num_bodies);
  colliders.reserve(num_bodies);

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

	cog_x += body.x * body.m / static_cast<float>(num_bodies);
	cog_y += body.y * body.m / static_cast<float>(num_bodies);
	cog_z += body.z * body.m / static_cast<float>(num_bodies);
	cog_vx += body.vx * body.m / static_cast<float>(num_bodies);
	cog_vy += body.vy * body.m / static_cast<float>(num_bodies);
	cog_vz += body.vz * body.m / static_cast<float>(num_bodies);
	cog_m += body.m / static_cast<float>(num_bodies);

	mass.push_back(body.m);
	ang_pos.push_back(Quaternion{0.0f, 0.0f, 0.0f, 0.0f});
	colliders.push_back(std::make_unique<SphereCollider>(body.r));
      }
    }, vari);
  }
  cog_x /= cog_m;
  cog_y /= cog_m;
  cog_z /= cog_m;
  cog_vx /= cog_m;
  cog_vy /= cog_m;
  cog_vz /= cog_m;
}

const Engine::Vec3x<float, 32> &Engine::get_pos() const { return pos; }
const Engine::Vec3x<float, 32> &Engine::get_vel() const { return vel; }
const Engine::Vec3x<float, 32> &Engine::get_force() const { return force; }
const std::vector<float> &Engine::get_mass() const { return mass; }
const std::vector<Quaternion> &Engine::get_ang_pos() const { return ang_pos; }
const std::vector<std::unique_ptr<Collider>> &Engine::get_colliders() const { return colliders; }
std::size_t Engine::get_num_bodies() const { return num_bodies; }

void Engine::update(const float dt) {
  gravity_force_update(dt);
  dynamics_update(dt);
}

void Engine::gravity_force_update(const float dt) {
  cog_x += cog_vx * dt;
  cog_y += cog_vy * dt;
  cog_z += cog_vz * dt;
  for (std::size_t i = 0; i < num_bodies; ++i) {
    float r_x = cog_x - pos.x.at(i);
    float r_y = cog_y - pos.y.at(i);
    float r_z = cog_z - pos.z.at(i);
    float r2 = r_x * r_x + r_y * r_y + r_z * r_z;
    float grav = grav_constant * cog_m  / (r2 * r2);
    force.x.at(i) = std::min(grav * r_x, MAX_FORCE);
    force.y.at(i) = std::min(grav * r_y, MAX_FORCE);
    force.z.at(i) = std::min(grav * r_z, MAX_FORCE);
  }
}

void Engine::dynamics_update(const float dt) {
  __m256 dt_a = _mm256_set_ps(dt, dt, dt, dt, dt, dt, dt, dt);
  fused_multiply_add(dt, dt_a, vel.x.data(), force.x.data(), mass.data());
  fused_multiply_add(dt, dt_a, vel.y.data(), force.y.data(), mass.data());
  fused_multiply_add(dt, dt_a, vel.z.data(), force.z.data(), mass.data());
  fused_multiply_add(dt, dt_a, pos.x.data(), vel.x.data(), mass.data());
  fused_multiply_add(dt, dt_a, pos.y.data(), vel.y.data(), mass.data());
  fused_multiply_add(dt, dt_a, pos.z.data(), vel.z.data(), mass.data());
}

void Engine::fused_multiply_add(const float dt, __m256& dt_a, float *const a, const float *const b, const float *const m) {
  std::size_t i = 0;
  if (num_bodies >= 8) {
    for (; i <= num_bodies - 8; i += 8) {
      __m256 a_a = _mm256_set_ps(a[i + 7], a[i + 6], a[i + 5], a[i + 4], a[i + 3], a[i + 2], a[i + 1], a[i]);
      __m256 b_a = _mm256_set_ps(b[i + 7], b[i + 6], b[i + 5], b[i + 4], b[i + 3], b[i + 2], b[i + 1], b[i]);
      __m256 m_a = _mm256_set_ps(m[i + 7], m[i + 6], m[i + 5], m[i + 4], m[i + 3], m[i + 2], m[i + 1], m[i]);
      __m256 result = _mm256_fmadd_ps(_mm256_div_ps(b_a, m_a), dt_a, a_a);
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
