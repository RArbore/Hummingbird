#pragma once

struct Transform {
  float x, y, z;
};

struct CollisionResponse {

};

class SphereCollider;

class Collider {
public:
  virtual CollisionResponse checkCollision(const Collider& other, const Transform& myPos, const Transform& otherPos) const = 0;
  virtual CollisionResponse checkCollision(const SphereCollider& other, const Transform& myPos, const Transform& otherPos) const = 0;
};

class SphereCollider : public Collider {
public:
  SphereCollider(float radius);
  virtual CollisionResponse checkCollision(const Collider& other, const Transform& myPos, const Transform& otherPos) const override;
  virtual CollisionResponse checkCollision(const SphereCollider& other, const Transform& myPos, const Transform& otherPos) const override;
private:
  float radius;
};
