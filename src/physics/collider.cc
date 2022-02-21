#include "physics/collider.h"

SphereCollider::SphereCollider(float radius): radius(radius) {}

CollisionResponse SphereCollider::checkCollision(const Collider& other, const Transform& myPos, const Transform& otherPos) const {
  return other.checkCollision(*this, otherPos, myPos);
};

CollisionResponse SphereCollider::checkCollision(const SphereCollider& other, const Transform& myPos, const Transform& otherPos) const {
  return {};
};
