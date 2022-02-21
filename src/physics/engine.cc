#include "physics/engine.h"

Engine::Engine(const Config& cfg): grav_constant(cfg.grav_constant),
				   num_bodies(cfg.num_bodies) {
  x.reserve(num_bodies);
  y.reserve(num_bodies);
  z.reserve(num_bodies);

  vx.reserve(num_bodies);
  vy.reserve(num_bodies);
  vz.reserve(num_bodies);

  ax.reserve(num_bodies);
  ay.reserve(num_bodies);
  az.reserve(num_bodies);
}

void update(float dt) {

}
