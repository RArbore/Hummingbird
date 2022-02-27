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

#include <physics/quaternion.h>

void Quaternion::normalize() {
  float factor = invSquareRoot(x*x + y*y + z*z + w*w); 
  x *= factor; 
  y *= factor; 
  z *= factor; 
  w *= factor; 
}

float Quaternion::invSquareRoot(const float n) const {
  float outf[4];
  _mm_store_ps(outf, _mm_rsqrt_ps(_mm_set_ps(n, 0.0f, 0.0f, 0.0f)));
  return outf[0];
}

Matrix3 Quaternion::convert() const {
  Matrix3 m; 
  
  m.arr[0] = 1 - 2*y*y - 2*z*z; 
  m.arr[1] = 2*x*y - 2*w*z; 
  m.arr[2] = 2*z*x + 2*w*y; 
  m.arr[3] = 2*x*y + 2*w*z; 
  m.arr[4] = 1 - 2*x*x - 2*z*z; 
  m.arr[5] = 2*y*z - 2*w*x; 
  m.arr[6] = 2*z*x - 2*w*y; 
  m.arr[7] = 2*y*z + 2*w*x; 
  m.arr[8] = 1 - 2*x*x - 2*y*y; 

  return m; 
}

Matrix3::Matrix3(const Quaternion& q) {
  arr[0] = 1 - 2*q.y*q.y - 2*q.z*q.z; 
  arr[1] = 2*q.x*q.y - 2*q.w*q.z; 
  arr[2] = 2*q.z*q.x + 2*q.w*q.y; 
  arr[3] = 2*q.x*q.y + 2*q.w*q.z; 
  arr[4] = 1 - 2*q.x*q.x - 2*q.z*q.z; 
  arr[5] = 2*q.y*q.z - 2*q.w*q.x; 
  arr[6] = 2*q.z*q.x - 2*q.w*q.y; 
  arr[7] = 2*q.y*q.z + 2*q.w*q.x; 
  arr[8] = 1 - 2*q.x*q.x - 2*q.y*q.y; 
}
