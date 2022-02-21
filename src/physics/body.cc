#include "body.h"

int Body::getXPos() const { return xPos_; }
int Body::getYPos() const { return yPos_; }
int Body::getZPos() const { return zPos_; }

double Body::getMass() const { return mass_; }

void Body::update() {
  calcForceVector();
  Vector3d acceleration =
      (Vector3d){force_.x / mass_, force_.y / mass_, force_.z / mass_};
  // change velocity
  velocity_.x += acceleration.x * timeScale;
  velocity_.y += acceleration.y * timeScale;
  velocity_.z += acceleration.z * timeScale;
  // change position
  xPos_ += velocity_.x * timeScale;
  yPos_ += velocity_.y * timeScale;
  zPos_ += velocity_.z * timeScale;
}

bool Body::collided(Body &body) { return false; }