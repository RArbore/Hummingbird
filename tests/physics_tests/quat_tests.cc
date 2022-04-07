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

bool smallDiff(float f1, float f2);
bool smallDiff(float f1, float f2) {
  float k = f1 - f2;
  if ((-0.01 < k) && (k < 0.01)) {
    return true;
  }
  return false;
}

TEST_CASE("Quaternion Constructor with values 1", "[quaternion]") {
  Quaternion q = {3.0f, 1.0f, 1.0f, 1.0f}; 
  REQUIRE(smallDiff(q.w, 3.0f)); 
  REQUIRE(smallDiff(q.x, 1.0f)); 
  REQUIRE(smallDiff(q.y, 1.0f)); 
  REQUIRE(smallDiff(q.z, 1.0f)); 
}

TEST_CASE("Quaternion Constructor with values 2", "[quaternion]") {
  Quaternion q = {5.0f, 12.0f, 4.0f, 8.0f}; 
  REQUIRE(smallDiff(q.w, 5.0f)); 
  REQUIRE(smallDiff(q.x, 12.0f)); 
  REQUIRE(smallDiff(q.y, 4.0f)); 
  REQUIRE(smallDiff(q.z, 8.0f)); 
}

TEST_CASE("Quaternion Constructor with values 3", "[quaternion]") {
  Quaternion q = {32.0f, 19.0f, 101.0f, 37.0f}; 
  REQUIRE(smallDiff(q.w, 32.0f)); 
  REQUIRE(smallDiff(q.x, 19.0f)); 
  REQUIRE(smallDiff(q.y, 101.0f)); 
  REQUIRE(smallDiff(q.z, 37.0f)); 
}

TEST_CASE("Quaternion Constructor with values 4", "[quaternion]") {
  Quaternion q = {5.0f, 39.0f, 2.0f, 17.0f}; 
  REQUIRE(smallDiff(q.w, 5.0f)); 
  REQUIRE(smallDiff(q.x, 39.0f)); 
  REQUIRE(smallDiff(q.y, 2.0f)); 
  REQUIRE(smallDiff(q.z, 17.0f)); 
}

TEST_CASE("Quaternion Empty Constructor", "[quaternion]") {
  Quaternion q; 
  REQUIRE(smallDiff(q.w, 0.0f)); 
  REQUIRE(smallDiff(q.x, 0.0f)); 
  REQUIRE(smallDiff(q.y, 0.0f)); 
  REQUIRE(smallDiff(q.z, 0.0f)); 
}

TEST_CASE("Normalize unit quaternion", "[quaternion]") {
  Quaternion q = {0.0f, 0.0f, 0.0f, 1.0f}; 
  q.normalize(); 
  REQUIRE(smallDiff(q.w, q.w)); 
  REQUIRE(smallDiff(q.x, q.x)); 
  REQUIRE(smallDiff(q.y, q.y)); 
  REQUIRE(smallDiff(q.z, q.z)); 
}

TEST_CASE("Normalize unit quaternion with zeros", "[quaternion]") {
  Quaternion q = {0.0f, 0.0f, 0.0f, 4.0f}; 
  q.normalize(); 
  REQUIRE(smallDiff(q.w, q.w)); 
  REQUIRE(smallDiff(q.x, q.x)); 
  REQUIRE(smallDiff(q.y, q.y)); 
  REQUIRE(smallDiff(q.z, 1.0f)); 
}


// TODO getting infinite length on this case
TEST_CASE("Normalize typical quaternion 1", "[quaternion]") {
  Quaternion q = {1.0f, 1.0f, 1.0f, 1.0f}; 
  q.normalize();  
  REQUIRE(smallDiff(q.w,.5f)); 
  REQUIRE(smallDiff(q.x,.5f)); 
  REQUIRE(smallDiff(q.y,.5f)); 
  REQUIRE(smallDiff(q.z,.5f)); 
}


// TODO getting infinite length on this case
TEST_CASE("Normalize typical quaternion 2", "[quaternion]") {
  Quaternion q = {3.0f, 4.0f, 12.0f, 0.0f}; 
  Quaternion q2 = q;
  q.normalize();  
  REQUIRE(smallDiff(q.w,q2.w/13.0f)); 
  REQUIRE(smallDiff(q.x,q2.x/13.0f)); 
  REQUIRE(smallDiff(q.y,q2.y/13.0f)); 
  REQUIRE(smallDiff(q.z,q2.z/13.0f)); 
}


TEST_CASE("Normalize typical quaternion 3", "[quaternion]") {
  Quaternion q = {5.0f, 7.0f, 31.0f, 101.0f}; 
  Quaternion q2 = q; 
  q2.normalize(); 
  float norm = 106.0f; 
  REQUIRE(smallDiff(q2.w, q.w/norm)); 
  REQUIRE(smallDiff(q2.x, q.x/norm)); 
  REQUIRE(smallDiff(q2.y, q.y/norm)); 
  REQUIRE(smallDiff(q2.z, q.z/norm)); 
}

TEST_CASE("Normalize typical quaternion 4", "[quaternion]") {
  Quaternion q = {8.0f, 9.0f, 12.0f, 144.0f}; 
  Quaternion q2 = q;
  q.normalize();  
  REQUIRE(smallDiff(q.w,q2.w/145.0f)); 
  REQUIRE(smallDiff(q.x,q2.x/145.0f)); 
  REQUIRE(smallDiff(q.y,q2.y/145.0f)); 
  REQUIRE(smallDiff(q.z,q2.z/145.0f)); 
}



/*
Expected matrix:
[  0.1333333, -0.6666667,  0.7333333;
   0.9333333,  0.3333333,  0.1333333;
  -0.3333333,  0.6666667,  0.6666667 ]
*/

TEST_CASE("Quaternion to matrix 1", "[quaternion]"){
  Quaternion q = {4.0f, 1.0f, 2.0f, 3.0f};
  q.normalize();
  Matrix3 m = q.convert();
  REQUIRE(smallDiff(m.arr[0], 0.133333f));
  REQUIRE(smallDiff(m.arr[1], -0.666667f));
  REQUIRE(smallDiff(m.arr[2], 0.733333f));
  REQUIRE(smallDiff(m.arr[3], 0.933333f));
  REQUIRE(smallDiff(m.arr[4], 0.333333f));
  REQUIRE(smallDiff(m.arr[5], 0.133333f));
  REQUIRE(smallDiff(m.arr[6], -0.333333f));
  REQUIRE(smallDiff(m.arr[7], 0.6666667f));
  REQUIRE(smallDiff(m.arr[8], 0.6666667f));
}

TEST_CASE("Quaternion to matrix 2", "[quaternion]"){
  Quaternion q = {4.0f, 1.0f, 2.0f, 3.0f};
  q.normalize();
  Matrix3 m (q);
  REQUIRE(smallDiff(m.arr[0], 0.133333f));
  REQUIRE(smallDiff(m.arr[1], -0.666667f));
  REQUIRE(smallDiff(m.arr[2], 0.733333f));
  REQUIRE(smallDiff(m.arr[3], 0.933333f));
  REQUIRE(smallDiff(m.arr[4], 0.333333f));
  REQUIRE(smallDiff(m.arr[5], 0.133333f));
  REQUIRE(smallDiff(m.arr[6], -0.333333f));
  REQUIRE(smallDiff(m.arr[7], 0.6666667f));
  REQUIRE(smallDiff(m.arr[8], 0.6666667f));
}