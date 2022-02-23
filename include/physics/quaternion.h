#pragma once

#include <immintrin.h>

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
  Matrix3(const Quaternion& q); 
  ~Matrix3() = default; 
};
