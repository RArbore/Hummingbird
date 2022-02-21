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
