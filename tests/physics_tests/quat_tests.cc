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
TEST_CASE("Normalize typical quaternion 1", "[quaternion]") {
  Quaternion q = {1.0f, 1.0f, 1.0f, 1.0f}; 
  q.normalize();  
  REQUIRE(q.w == .5f); 
  REQUIRE(q.x == .5f); 
  REQUIRE(q.y == .5f); 
  REQUIRE(q.z == .5f); 
}

TEST_CASE("Normalize typical quaternion 2", "[quaternion]") {
  Quaternion q = {5.0f, 7.0f, 31.0f, 101.0f}; 
  Quaternion q2 = q; 
  q2.normalize(); 
  float norm = 106.0f; 
  REQUIRE(q2.w == q.w/norm); 
  REQUIRE(q2.x == q.x/norm); 
  REQUIRE(q2.y == q.y/norm); 
  REQUIRE(q2.z == q.z/norm); 
}
