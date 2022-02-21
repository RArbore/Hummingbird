#include "engine.h"

Engine::Engine() {}

float gravity;
float timeScale;

void Engine::init(Config cfg) {
  gravity = cfg.grav_constant;
  timeScale = .1;
}

void Engine::addBody(Body &body) { bodies_.push_back(body); }

std::vector<Body> &Engine::getBodies() { return bodies_; }

void Engine::update() {
  for (Body &body : bodies_)
    body.update();
}