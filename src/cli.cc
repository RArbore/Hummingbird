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

#include <cli.h>

template <typename T>
int init_constant(const Json::Value &root, const char *index, T &depo,
                  bool (*criteria)(const Json::Value &)) {
  const Json::Value &indexed = root[index];
  if (!criteria(indexed)) {
    std::cerr << "ERROR: Either couldn't find " << index
              << " in input JSON, or the value of " << index
              << " is not of the correct type." << std::endl;
    return -1;
  }
  depo = indexed.as<T>();
  return 0;
}

int Config::initialize() {
  std::ifstream json_file(json_file_name);
  if (!json_file.is_open()) {
    std::cerr << "ERROR: Couldn't open config file " << json_file_name
              << std::endl;
    return -1;
  }

  Json::Value root;
  json_file >> root;

  if (init_constant(root, "GRAVITY", grav_constant,
                    [](const Json::Value &jv) { return jv.isNumeric(); }))
    return -1;

  if (init_constant(root, "NUM_BODIES", num_bodies,
                    [](const Json::Value &jv) { return jv.isIntegral(); }))
    return -1;

  const Json::Value &bodies = root["BODIES"];
  if (!bodies.isArray()) {
    std::cerr << "ERROR: Either couldn't find BODIES in input JSON, or the value of BODIES is not of the correct type." << std::endl;
    return -1;
  }

  return 0;
}
