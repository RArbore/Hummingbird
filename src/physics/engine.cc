#include "physics/engine.h"

Engine::Engine(const Config& cfg): grav_constant(cfg.grav_constant),
				   num_bodies(cfg.num_bodies) {
  pos.x.reserve(num_bodies);
  pos.y.reserve(num_bodies);
  pos.z.reserve(num_bodies);

  vel.x.reserve(num_bodies);
  vel.y.reserve(num_bodies);
  vel.z.reserve(num_bodies);

  acc.x.reserve(num_bodies);
  acc.y.reserve(num_bodies);
  acc.z.reserve(num_bodies);

  mass.reserve(num_bodies);
}

void update(float dt) {

}
