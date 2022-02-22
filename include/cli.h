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

#pragma once

#include <iostream>
#include <fstream>
#include <cstddef>
#include <variant>
#include <vector>

#include <json/json.h>

struct ConfigSphere {
  float x, y, r;
};

struct Config {
  Config(char *json_file_name) : json_file_name(json_file_name) {}
  int initialize();
  char *json_file_name;
  float grav_constant;
  std::size_t num_bodies;
  std::vector<std::variant<ConfigSphere>> bodies;
};
