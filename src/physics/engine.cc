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
				   acc{std::vector<float>(num_bodies, 0.0), std::vector<float>(num_bodies, 0.0), std::vector<float>(num_bodies, 0.0)} {
  pos.x.reserve(num_bodies);
  pos.y.reserve(num_bodies);
  pos.z.reserve(num_bodies);

  vel.x.reserve(num_bodies);
  vel.y.reserve(num_bodies);
  vel.z.reserve(num_bodies);

  mass.reserve(num_bodies);
  colliders.reserve(num_bodies);
}

void update(float dt) {

}
