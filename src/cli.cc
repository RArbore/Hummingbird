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
int init_constant(const Json::Value &root, const char *index, T &depo, bool (*criteria)(const Json::Value &)) {
  const Json::Value &indexed = root[index];
  if (!criteria(indexed)) {
    std::cerr << "ERROR: Either couldn't find " << index << " in input JSON, or the value of " << index << " is not of the correct type." << std::endl;
    return -1;
  }
  depo = indexed.as<T>();
  return 0;
}

int Config::process_body(const Json::Value &root) {
  std::string type;
  if (init_constant(root, "TYPE", type, [](const Json::Value &jv) { return jv.isString(); })) return -1;
  if (type == "SPHERE") {
    float x = 0.0f, y = 0.0f, z = 0.0f, vx = 0.0f, vy = 0.0f, vz = 0.0f, m = 0.0f, r = 0.0f;
    if (init_constant(root, "x", x, [](const Json::Value &jv) { return jv.isNumeric(); })) return -1;
    if (init_constant(root, "y", y, [](const Json::Value &jv) { return jv.isNumeric(); })) return -1;
    if (init_constant(root, "z", z, [](const Json::Value &jv) { return jv.isNumeric(); })) return -1;
    if (root["vx"].isNumeric()) vx = root["vx"].as<float>();
    if (root["vy"].isNumeric()) vy = root["vy"].as<float>();
    if (root["vz"].isNumeric()) vz = root["vz"].as<float>();
    if (init_constant(root, "m", m, [](const Json::Value &jv) { return jv.isNumeric(); })) return -1;
    if (init_constant(root, "r", r, [](const Json::Value &jv) { return jv.isNumeric(); })) return -1;
    bodies.push_back(ConfigSphere{x, y, z, vx, vy, vz, m, r});
  }
  else if (type == "RANDOM") {
    process_body(root["TEMPLATE"]);

    auto config_body = bodies.back();
    bodies.pop_back();
    
    std::size_t num_bodies_gen = 0;
    float min_x = 0.0f, max_x = 0.0f, min_y = 0.0f, max_y = 0.0f, min_z = 0.0f, max_z = 0.0f;
    if (init_constant(root, "num_bodies", num_bodies_gen, [](const Json::Value &jv) { return jv.isIntegral(); })) return -1;
    if (init_constant(root, "min_x", min_x, [](const Json::Value &jv) { return jv.isNumeric(); })) return -1;
    if (init_constant(root, "min_y", min_y, [](const Json::Value &jv) { return jv.isNumeric(); })) return -1;
    if (init_constant(root, "min_z", min_z, [](const Json::Value &jv) { return jv.isNumeric(); })) return -1;
    if (init_constant(root, "max_x", max_x, [](const Json::Value &jv) { return jv.isNumeric(); })) return -1;
    if (init_constant(root, "max_y", max_y, [](const Json::Value &jv) { return jv.isNumeric(); })) return -1;
    if (init_constant(root, "max_z", max_z, [](const Json::Value &jv) { return jv.isNumeric(); })) return -1;
    for (std::size_t i = 0; i < num_bodies_gen; ++i) {
      float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
      float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
      float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
      x = x * (max_x - min_x) + min_x;
      y = y * (max_y - min_y) + min_y;
      z = z * (max_z - min_z) + min_z;
      std::visit([&](auto arg) {
	arg.x += x;
	arg.y += y;
	arg.z += z;
	bodies.push_back(arg);
      }, config_body);
    }
  }
  else {
    std::cerr << "ERROR: Unrecognized type " << type << "." << std::endl;
    return -1;
  }
  return 0;
}

int Config::initialize() {
  std::ifstream json_file(json_file_name);
  if (!json_file.is_open()) {
    std::cerr << "ERROR: Couldn't open config file " << json_file_name << "." << std::endl;
    return -1;
  }
  
  Json::Value root;

  try {  
    json_file >> root;
  } catch(const Json::RuntimeError& e) {
    std::cerr << "ERROR: Malformed JSON config file " << json_file_name << "." << std::endl; 
    return -1; 
  }

  if (init_constant(root, "GRAVITY", grav_constant, [](const Json::Value &jv) { return jv.isNumeric(); })) return -1;

  if (root["TICKS_PER_FRAME"].isIntegral()) ticks_per_frame = root["TICKS_PER_FRAME"].as<std::size_t>();

  const Json::Value &jv_bodies = root["BODIES"];
  if (!jv_bodies.isArray()) {
    std::cerr << "ERROR: Either couldn't find BODIES in input JSON, or the value of BODIES is not of the correct type." << std::endl;
    return -1;
  }

  for (auto val : root["BODIES"]) {
    if (process_body(val)) return -1;
  }
  num_bodies = bodies.size();

  return 0;
}
