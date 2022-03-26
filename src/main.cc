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
#include <playback.h>

/*
 * Get the current Unix time in microseconds.
 */
__attribute__((always_inline))
inline unsigned long long micro_sec() {
    return static_cast<unsigned long long>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
}

int runEngine(int argc, char **argv, bool record); 
int runPlayback(int argc, char **argv); 

/**
 * Entry point for the program. 
 * Handles the flags for what to eventually run. 
 */
int main(int argc, char **argv) {
  if (argc != 2 && argc != 3) {
    std::cerr << "Usage: " << argv[0] << "<json_file> (use -h for help)" << std::endl;
    return -1;
  }
  if(argc == 2) {
    if(strcmp(argv[1], "-h") == 0) { // help flag
      std::cout << "Usage: " << argv[0] << " [flags] <json_file>" << std::endl; 
      std::cout << "Flags:\n-h\t help" << std::endl; 
      std::cout << "-p \t playback from provided json_file" << std::endl; 
      std::cout << "-r \t record simulation" << std::endl; 
      return 0; 
    }
    return runEngine(argc, argv, false); 
  }

  // argc == 3
  if(strcmp(argv[1], "-p") == 0) { // playback flag
    return runPlayback(argc, argv); 
  } else if(strcmp(argv[1], "-r") == 0) { // record flag
    return runEngine(argc, argv, true); 
  } else {
    std::cout << "Flag usage: " << argv[0] << " -[p/r] <json_file>" << std::endl; 
    return -1; 
  }

  return 0; 
}

/*
 * Initialize a config, our engine, and a graphics 
 * context. Updates the N times per frame, according 
 * to user configuration. We use return codes for 
 * error handling - if an error happens, a message 
 * is printed to stderr at the error site, and -1 
 * is returned up the stack. We also decide whether 
 * or not to record based on a boolean input. 
 */
int runEngine(int argc, char **argv, bool record) {
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

int runPlayback(int argc, char **argv) {

  return 0; 
}
