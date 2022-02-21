#pragma once
/**
 * The abstract class from which all bodies in simulation derive from
 *
 */
#include "engine.h"

class Body {
public:
  int getXPos() const;
  int getYPos() const;
  int getZPos() const;

  double getMass() const;

  void update();

protected:
  struct Vector3d {
    float x, y, z;
  };

  float xPos_, yPos_, zPos_;

  double mass_;

  double gravity_;

  Vector3d velocity_;

  Vector3d force_;

private:
  virtual void calcForceVector() = 0;
  virtual bool collided(Body &body);
};