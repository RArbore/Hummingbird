#include <fstream>
#include <iostream>

#include <json/json.h>

struct Config {
  Config(char *json_file_name) : json_file_name(json_file_name) {}
  int initialize();
  char *json_file_name;
  float grav_constant;
};
