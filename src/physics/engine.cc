#include "engine.h"

Engine::Engine() {}

double gravity;

void Engine::init(Config cfg) { gravity = cfg.grav_constant; }

void Engine::addBody(Body &body) { bodies_.push_back(body); }

std::vector<Body> &Engine::getBodies() { return bodies_; }

void Engine::update() {
  for (Body &body : bodies_)
    body.update();
}