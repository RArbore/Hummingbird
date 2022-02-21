#pragma once

#include <math.h>

struct Transform {
  float x, y, z;
};

Transform operator+(const Transform& a, const Transform& b);
Transform operator-(const Transform& a, const Transform& b);
float operator*(const Transform& a, const Transform& b);
Transform operator*(const Transform& a, float b);
Transform operator/(const Transform& a, float b);

struct CollisionResponse {
  Transform AinB;
  Transform BinA;
  Transform normal;
  float depth;
  bool collides;
};

class SphereCollider;

class Collider {
public:
  virtual CollisionResponse checkCollision(const Collider& other, const Transform& myPos, const Transform& otherPos) const = 0;
  virtual CollisionResponse checkCollision(const SphereCollider& other, const Transform& myPos, const Transform& otherPos) const = 0;
  virtual ~Collider();
};

class SphereCollider : public Collider {
public:
  SphereCollider(float radius);
  virtual CollisionResponse checkCollision(const Collider& other, const Transform& myPos, const Transform& otherPos) const override;
  virtual CollisionResponse checkCollision(const SphereCollider& other, const Transform& myPos, const Transform& otherPos) const override;
private:
  float radius;
};
