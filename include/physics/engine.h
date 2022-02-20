#pragma once

#include "body.h"
#include "cli.h"

#include <vector>

/**
 * @brief Outer shell of the physics engine
 * Holds the objects and acts as the way
 */

extern double gravity;

class Engine {
public:
  Engine();

  /**
   * @brief Initializes the starting environment from the given data
   *
   * @param cfg parsed data from json file
   */
  void init(Config cfg);

  void addBody(Body &body);
  std::vector<Body> &getBodies();

  void update();

private:
  std::vector<Body> bodies_;
};