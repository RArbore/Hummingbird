#include "physics/quartenion.h"

void Quartenion::normalize() {
  // inverse square root of length of vector
  float factor = invSquareRoot(x*x + y*y + z*z + w*w); 
  x *= factor; 
  y *= factor; 
  z *= factor; 
  w *= factor; 
}

float Quartenion::invSquareRoot(float n) {
  float threehalfs = 1.5f; 
  float y = n;
   
  long i = * ( long * ) &y;

  i = 0x5f3759df - ( i >> 1 );
  y = * ( float * ) &i;
  
  y = y * ( threehalfs - ( (n * 0.5F) * y * y ) );
  
  return y;
}

Matrix Quartenion::convert() {
  Matrix m; 
  
  m.row0[0] = 1 - 2*y*y - 2*z*z; 
  m.row0[1] = 2*x*y - 2*w*z; 
  m.row0[2] = 2*z*x + 2*w*y; 

  m.row1[0] = 2*x*y + 2*w*z; 
  m.row1[1] = 1 - 2*x*x - 2*z*z; 
  m.row1[2] = 2*y*z - 2*w*x; 

  m.row2[0] = 2*z*x - 2*w*y; 
  m.row2[1] = 2*y*z + 2*w*x; 
  m.row2[2] = 1 - 2*x*x - 2*y*y; 

  return m; 
}

Matrix::Matrix() {
  row0 = new float[3]; 
  row1 = new float[3]; 
  row2 = new float[3]; 
}

Matrix::~Matrix() {
  delete[] row0; 
  delete[] row1; 
  delete[] row2; 
}