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

  return 0;
}
