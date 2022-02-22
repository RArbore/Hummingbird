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
  REQUIRE(cfg.bodies.size() == 1); 
  REQUIRE(cfg.num_bodies == 1);
}

TEST_CASE("Initialize only gravity field", "[cli]") {
  char file_name[]{"tests/cli_jsons/gravity.json"};
  Config cfg(file_name);

  REQUIRE(cfg.initialize() == -1);
}

TEST_CASE("Initialize only bodies field", "[cli]") {
  char file_name[]{"tests/cli_jsons/numbodies.json"};
  Config cfg(file_name);

  REQUIRE(cfg.initialize() == -1);
}

TEST_CASE("Initialize without fields", "[cli]") {
  char file_name[]{"tests/cli_jsons/nothing.json"};
  Config cfg(file_name);

  REQUIRE(cfg.initialize() == -1);
}

TEST_CASE("Initialize NUM_BODIES != size of BODIES", "[cli]") {
  char file_name[]{"tests/cli_jsons/diffbodies.json"};
  Config cfg(file_name);

  REQUIRE(cfg.initialize() == -1); 
}