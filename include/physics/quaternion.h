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

#pragma once

#include <math.h>

/**
 * @brief Represents rotations
 * Note that x, y, z represent components of a normal vector
 * w represents the scale of the rotation
 */
struct Matrix3;

struct Quaternion {
  float w, x, y, z; 

  void normalize(); 

  Matrix3 convert() const; 

private:
  float invSquareRoot(const float n) const; 
};

/**
 * @brief Takes a quaternion and converts to matrix
 * in order to take advantage of a graphics hardware
 */
struct Matrix3 {
  float arr[9];

  Matrix3() = default; 
  explicit Matrix3(const Quaternion& q); 
};
