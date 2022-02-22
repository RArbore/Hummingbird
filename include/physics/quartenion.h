#pragma once

/**
 * @brief Represents rotations
 * Note that x, y, z represent components of a normal vector
 * w represents the scale of the rotation
 */
struct Quartenion {
  float w, x, y, z; 

  void normalize(); 

  Matrix convert(); 

private:
  float invSquareRoot(float n); 
};

/**
 * @brief Takes a quartenion and converts to matrix
 * in order to take advantage of a graphics hardware
 */
struct Matrix {
  float *row0;
  float *row1; 
  float *row2; 

  Matrix(); 
  Matrix(Quartenion& q); 

  ~Matrix(); 
};
