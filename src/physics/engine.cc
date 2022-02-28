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

	cog_x += body.x / static_cast<float>(num_bodies);
	cog_y += body.y / static_cast<float>(num_bodies);
	cog_z += body.z / static_cast<float>(num_bodies);
	cog_vx += body.vx / static_cast<float>(num_bodies);
	cog_vy += body.vy / static_cast<float>(num_bodies);
	cog_vz += body.vz / static_cast<float>(num_bodies);
	cog_m += body.m / static_cast<float>(num_bodies);

	mass.push_back(body.m);
	ang_pos.push_back(Quaternion{0.0f, 0.0f, 0.0f, 0.0f});
	colliders.push_back(std::make_unique<SphereCollider>(body.r));
      }
    }, vari);
  }
}

const Engine::Vec3x<float, 32> &Engine::get_pos() const { return pos; }
const Engine::Vec3x<float, 32> &Engine::get_vel() const { return vel; }
const Engine::Vec3x<float, 32> &Engine::get_force() const { return force; }
const std::vector<float> &Engine::get_mass() const { return mass; }
const std::vector<Quaternion> &Engine::get_ang_pos() const { return ang_pos; }
const std::vector<std::unique_ptr<Collider>> &Engine::get_colliders() const { return colliders; }
std::size_t Engine::get_num_bodies() const { return num_bodies; }

void Engine::update(const float dt) {
  rk4_update(dt);
}

__attribute__((always_inline))
inline std::tuple<float, float, float> Engine::rk4_accel(const float x, const float y, const float z, const float m) {
    float r_x = cog_x - x;
    float r_y = cog_y - y;
    float r_z = cog_z - z;
    float r2 = r_x * r_x + r_y * r_y + r_z * r_z;
    float grav = grav_constant * cog_m  / (r2 * r2 * m);
    std::tuple<float, float, float> result(grav * r_x, grav * r_y, grav * r_z);
    return result;
};

void Engine::rk4_update(const float dt) {
  cog_x += cog_vx * dt;
  cog_y += cog_vy * dt;
  cog_z += cog_vz * dt;

  for (std::size_t i = 0; i < num_bodies; ++i) {
    float p1x = pos.x.at(i), p1y = pos.y.at(i), p1z = pos.z.at(i);
    float v1x = vel.x.at(i), v1y = vel.y.at(i), v1z = vel.z.at(i);
    auto [a1x, a1y, a1z] = rk4_accel(p1x, p1y, p1z, mass.at(i));
    float p2x = p1x + v1x * dt * 0.5f, p2y = p1y + v1y * dt * 0.5f, p2z = p1z + v1z * dt * 0.5f;
    float v2x = v1x + a1x * dt * 0.5f, v2y = v1y + a1y * dt * 0.5f, v2z = v1z + a1z * dt * 0.5f;
    auto [a2x, a2y, a2z] = rk4_accel(p2x, p2y, p2z, mass.at(i));
    float p3x = p2x + v2x * dt * 0.5f, p3y = p2y + v2y * dt * 0.5f, p3z = p2z + v2z * dt * 0.5f;
    float v3x = v2x + a2x * dt * 0.5f, v3y = v2y + a2y * dt * 0.5f, v3z = v2z + a2z * dt * 0.5f;
    auto [a3x, a3y, a3z] = rk4_accel(p3x, p3y, p3z, mass.at(i));
    float p4x = p3x + v3x * dt, p4y = p3y + v3y * dt, p4z = p3z + v3z * dt;
    float v4x = v3x + a3x * dt, v4y = v3y + a3y * dt, v4z = v3z + a3z * dt;
    auto [a4x, a4y, a4z] = rk4_accel(p4x, p4y, p4z, mass.at(i));
    float dp_x = (v1x + 2 * (v2x + v3x) + v4x) / 6.0f;
    float dp_y = (v1y + 2 * (v2y + v3y) + v4y) / 6.0f;
    float dp_z = (v1z + 2 * (v2z + v3z) + v4z) / 6.0f;
    float dv_x = (a1x + 2 * (a2x + a3x) + a4x) / 6.0f;
    float dv_y = (a1y + 2 * (a2y + a3y) + a4y) / 6.0f;
    float dv_z = (a1z + 2 * (a2z + a3z) + a4z) / 6.0f;
    pos.x.at(i) += dp_x * dt;
    pos.y.at(i) += dp_y * dt;
    pos.z.at(i) += dp_z * dt;
    vel.x.at(i) += dv_x * dt;
    vel.y.at(i) += dv_y * dt;
    vel.z.at(i) += dv_z * dt;
  }
}

void Engine::fused_multiply_add(const float dt, __m256& dt_a, float *const a, const float *const b) {
  std::size_t i = 0;
  if (num_bodies >= 8) {
    for (; i <= num_bodies - 8; i += 8) {
      __m256 a_a = _mm256_set_ps(a[i + 7], a[i + 6], a[i + 5], a[i + 4], a[i + 3], a[i + 2], a[i + 1], a[i]);
      __m256 b_a = _mm256_set_ps(b[i + 7], b[i + 6], b[i + 5], b[i + 4], b[i + 3], b[i + 2], b[i + 1], b[i]);
      __m256 result = _mm256_fmadd_ps(b_a, dt_a, a_a);
      _mm256_store_ps(a + i, result);
    }
  }
  for (; i < num_bodies; ++i) {
    a[i] += b[i] * dt;
  }
}

Transform Engine::get_transform_at(const std::size_t i) {
  return Transform{pos.x.at(i), pos.y.at(i), pos.z.at(i)};
}
