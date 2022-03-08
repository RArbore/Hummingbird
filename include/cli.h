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

/*
 * Define types of bodies we can create in our
 * config. These are not child classes of a base
 * config body, since we store config bodies as
 * std::variants - we use std::visit and duck
 * typing to work with them.
 */
struct ConfigSphere {
  float x, y, z, vx, vy, vz, m, r;
};

/*
 * Config struct representing a user config. We
 * don't read our input file on construction as
 * we want to return an error code if we can't
 * parse the input configuration for any reason.
 * Contains a couple fields for simulation
 * constants, and a vector of variants of config
 * bodies (representing the bodies we want to
 * spawn in our simulation).
 */
struct Config {
  explicit Config(char *json_file_name_i) : json_file_name(json_file_name_i), grav_constant(0.0f), elasticity(0.0f), ticks_per_frame(1), num_bodies(0), boundary{} {}
  int process_body(const Json::Value &root);
  int initialize();
  char *json_file_name;
  float grav_constant;
  float elasticity;
  std::size_t ticks_per_frame;
  std::size_t num_bodies;
  float boundary[6];
  std::vector<std::variant<ConfigSphere>> bodies;
};
