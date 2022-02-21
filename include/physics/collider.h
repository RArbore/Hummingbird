#pragma once

struct Transform {
  float x, y, z;
};

struct CollisionResponse {

};

class Collider {
public:
  virtual CollisionResponse checkCollision(const Collider& other, Transform myPos, Transform otherPos);
};
