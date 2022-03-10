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

#include <iostream>
#include <cstddef>
#include <chrono>

#include <physics/engine.h>
#include <interface.h>
#include <cli.h>

/*
 * Get the current Unix time in microseconds.
 */
__attribute__((always_inline))
inline unsigned long long micro_sec() {
    return static_cast<unsigned long long>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
}

/*
 * Entry point for our program. Initialize a config,
 * our engine, and a graphics context. Updates the
 * N times per frame, according to user configuration.
 * We use return codes for error handling - if an error
 * happens, a message is printed to stderr at the error
 * site, and -1 is returned up the stack.
 */
int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <json_file>" << std::endl;
    return -1;
  }

  srand(static_cast<unsigned int>(micro_sec()));

  Config config(argv[1]);
  if (config.initialize()) return -1;

  Engine engine(config);
  
  Graphics graphics(engine);
  if (graphics.initialize()) return -1;
  
  float dt = 0., ticks_per_frame = static_cast<float>(config.ticks_per_frame);

  unsigned long long before = 0, after = 0;

  while (!graphics.should_close()) {
    before = micro_sec();
    
    for (std::size_t i = 0; i < config.ticks_per_frame; ++i)
      engine.update(config.speed * dt / ticks_per_frame);
    graphics.render_tick(dt);

    after = micro_sec();
    dt = static_cast<float>(after - before) / 1000000.0f;
    std::cout << "FPS: " << 1. / dt << '\n';
  }

  return 0;
}
