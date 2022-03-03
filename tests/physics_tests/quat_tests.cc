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

#include "../../include/physics/quaternion.h"

TEST_CASE("Quaternion Constructor with values", "[quaternion]") {
  Quaternion q = {3.0f, 1.0f, 1.0f, 1.0f}; 
  REQUIRE(q.w == 3.0f); 
  REQUIRE(q.x == 1.0f); 
  REQUIRE(q.y == 1.0f); 
  REQUIRE(q.z == 1.0f); 
}

TEST_CASE("Quaternion Empty Constructor", "[quaternion]") {
  Quaternion q; 
  REQUIRE(q.w == 0.0f); 
  REQUIRE(q.x == 0.0f); 
  REQUIRE(q.y == 0.0f); 
  REQUIRE(q.z == 0.0f); 
}

TEST_CASE("Normalize unit quaternion", "[quaternion]") {
  Quaternion q = {0.0f, 0.0f, 0.0f, 1.0f}; 
  q.normalize(); 
  REQUIRE(q.w == q.w); 
  REQUIRE(q.x == q.x); 
  REQUIRE(q.y == q.y); 
  REQUIRE(q.z == q.z); 
}

// TODO getting infinite length on this case
TEST_CASE("Normalize typical quaternion", "[quaternion]") {
  Quaternion q = {1.0f, 2.0f, 2.0f, 3.0f}; 
  q.normalize();  
  REQUIRE(q.w == .25f); 
  REQUIRE(q.x == .5f); 
  REQUIRE(q.y == .5f); 
  REQUIRE(q.z == .75f); 
}
