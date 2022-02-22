#define CATCH_CONFIG_MAIN

#include "catch2/catch.hpp"

#include "../include/cli.h"

#include <string>

TEST_CASE("Config constructor general case", "[cli]") {
  char file_name[]{"test.json"};
  Config cfg(file_name);
  REQUIRE(strncmp(file_name, "test.json", 20) == 0);
}

TEST_CASE("Config constructor empty file name", "[cli]") {
  char file_name[]{};
  Config cfg(file_name);
  REQUIRE(strncmp(file_name, "", 2) == 0);
}

TEST_CASE("Initialize general case", "[cli]") {
  char file_name[]{"tests/cli_jsons/general.json"};
  Config cfg(file_name);

  REQUIRE(cfg.initialize() == 0);
  REQUIRE(cfg.grav_constant == 1.0f);
  REQUIRE(cfg.num_bodies == 10);
}

TEST_CASE("Initialize without gravity field", "[cli]") {
  char file_name[]{"tests/cli_jsons/no_gravity.json"};
  Config cfg(file_name);

  REQUIRE(cfg.initialize() == -1);
}

TEST_CASE("Initialize without bodies field", "[cli]") {
  char file_name[]{"tests/cli_jsons/no_bodies.json"};
  Config cfg(file_name);

  REQUIRE(cfg.initialize() == -1);
}

TEST_CASE("Initialize without fields", "[cli]") {
  char file_name[]{"tests/cli_jsons/nothing.json"};
  Config cfg(file_name);

  REQUIRE(cfg.initialize() == -1);
}