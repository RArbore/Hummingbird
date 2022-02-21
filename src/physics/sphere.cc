#include "sphere.h"

int Sphere::getRadius() const { return radius_; }

void Sphere::calcForceVector() {
  // only gravity for now
  force_ = (Vector3d){0, -9.81, 0};
}