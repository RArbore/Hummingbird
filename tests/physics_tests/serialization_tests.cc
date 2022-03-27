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

#include "catch2/catch.hpp"

#include "playback.h"

TEST_CASE("Serialization and deserialization test", "[engine]") {
  Config config(nullptr);
  config.num_bodies = 1000;
  config.boundary[0] = 0.0;
  config.boundary[1] = 100.0f;
  config.boundary[2] = 0.0;
  config.boundary[3] = 100.0f;
  config.boundary[4] = 0.0;
  config.boundary[5] = 100.0f;
  std::size_t i;
  for (i = 0; i < config.num_bodies; ++i)
    config.bodies.push_back(ConfigSphere{static_cast<float>(i), 0.0f, static_cast<float>(i) / 2.0f, 1.3f, 1.3f, 1.3f, 3.2f, 0.7f});
  Engine engine_src(config);
  config.bodies.resize(config.num_bodies, ConfigSphere{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 00.0f, 0.0f, .0f});
  Engine engine_dst(config);

  {
    std::ofstream outfile("binary.txt", std::ofstream::binary);
    dump_tick_to_file(outfile, engine_src);
  }
  
  {
    std::ifstream infile("binary.txt", std::ifstream::binary);
    load_tick_from_file(infile, engine_dst);
  }

  for (i = 0; i < config.num_bodies; ++i) {
    auto pos_src = engine_src.get_pos();
    auto pos_dst = engine_dst.get_pos();
    if (pos_src.x.at(i) != pos_dst.x.at(i) ||
	pos_src.y.at(i) != pos_dst.y.at(i) ||
	pos_src.z.at(i) != pos_dst.z.at(i))
      break;
  }

  REQUIRE(i == config.num_bodies);
}
