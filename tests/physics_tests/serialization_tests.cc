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


#include "catch2/catch.hpp"
#include <iostream>

#include "../../include/playback.h"

TEST_CASE("Serialization and deserialization test", "[engine]") {
  std::ofstream outfile("binary.txt", std::ofstream::binary);
  std::ifstream infile("binary.txt", std::ifstream::binary);
  Engine::Vec3x<float, 32> original;
  size_t num = 3;
  for (size_t i = 0; i < num; i++){
      original.x.push_back(0.1f);
      original.y.push_back(0.2f);
      original.z.push_back(0.3f);
  }
  std::cout << "Has reached this line";
  save_to_file(outfile, num, original);
  Engine::Vec3x<float, 32> result = read_from_file(infile);
  for (size_t i = 0; i < num; i++){
      REQUIRE(result.x[i] == original.x[i]);
      REQUIRE(result.y[i] == original.y[i]);
      REQUIRE(result.z[i] == original.z[i]);
  }
}
