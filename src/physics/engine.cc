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

#include "physics/engine.h"

Engine::Engine(const Config& cfg): grav_constant(cfg.grav_constant),
				   num_bodies(cfg.num_bodies),
				   acc{std::vector<float>(num_bodies, 0.0f), std::vector<float>(num_bodies, 0.0f), std::vector<float>(num_bodies, 0.0f)} {
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

	vel.x.push_back(0.0f);
	vel.y.push_back(0.0f);
	vel.z.push_back(0.0f);

	mass.push_back(body.m);
	ang_pos.push_back(Quaternion{0.0f, 0.0f, 0.0f, 0.0f});
	colliders.push_back(std::make_unique<SphereCollider>(body.r));
      }
    }, vari);
  }
  for (float x = 0; x < 50; ++x) {
    for (float y = 0; y < 50; ++y) {
      for (float z = 0; z < 50; ++z) {
	pos.x.push_back(x * 3);
	pos.y.push_back(y * 3);
	pos.z.push_back(z * 3);
	vel.x.push_back(0.0f);
	vel.y.push_back(0.0f);
	vel.z.push_back(0.0f);
	mass.push_back(1.0f);
	ang_pos.push_back(Quaternion{0.0f, 0.0f, 0.0f, 0.0f});
	colliders.push_back(std::make_unique<SphereCollider>(1.0f));
      }
    }
  }
  num_bodies += 50 * 50 * 50;
}

const Engine::Vec3x<float> &Engine::get_pos() const { return pos; }
const Engine::Vec3x<float> &Engine::get_vel() const { return vel; }
const Engine::Vec3x<float> &Engine::get_acc() const { return acc; }
const std::vector<float> &Engine::get_mass() const { return mass; }
const std::vector<Quaternion> &Engine::get_ang_pos() const { return ang_pos; }
const std::vector<std::unique_ptr<Collider>> &Engine::get_colliders() const { return colliders; }
std::size_t Engine::get_num_bodies() const { return num_bodies; }

void Engine::update(const float dt) {

}

Transform Engine::getTransformAt(const std::size_t i) {
  return Transform{pos.x.at(i), pos.y.at(i), pos.z.at(i)};
}
