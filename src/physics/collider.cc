#include "physics/collider.h"

SphereCollider::SphereCollider(float radius): radius(radius) {}

CollisionResponse SphereCollider::checkCollision(const Collider& other, const Transform& myPos, const Transform& otherPos) const {
  return other.checkCollision(*this, otherPos, myPos);
};

CollisionResponse SphereCollider::checkCollision(const SphereCollider& other, const Transform& myPos, const Transform& otherPos) const {
  Transform BmA = otherPos - myPos;
  CollisionResponse result;
  result.depth = sqrt(BmA * BmA);
  result.collides = result.depth <= radius + other.radius;
  result.normal = BmA / result.depth;
  result.AinB = myPos + (result.normal * radius);
  result.BinA = otherPos + (result.normal * -radius);
  return result;
};

Transform operator+(const Transform& a, const Transform& b) {
  return {.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
}

Transform operator-(const Transform& a, const Transform& b) {
  return {.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z};
}

float operator*(const Transform& a, const Transform& b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

Transform operator*(const Transform& a, float b) {
  return {.x = a.x * b, .y = a.y * b, .z = a.z * b};
}

Transform operator/(const Transform& a, float b) {
  return {.x = a.x / b, .y = a.y / b, .z = a.z / b};
}
