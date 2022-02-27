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

#include <physics/collider.h>

SphereCollider::SphereCollider(float radius_i): radius(radius_i) {}

CollisionResponse SphereCollider::checkCollision(const Collider& other, const Transform& myPos, const Transform& otherPos) const {
  return other.checkCollision(*this, otherPos, myPos);
}

CollisionResponse SphereCollider::checkCollision(const SphereCollider& other, const Transform& myPos, const Transform& otherPos) const {
  Transform BmA = otherPos - myPos;
  CollisionResponse result;
  result.depth = sqrt(BmA * BmA);
  result.collides = result.depth <= radius + other.radius;
  result.normal = BmA / result.depth;
  result.AinB = myPos + (result.normal * radius);
  result.BinA = otherPos + (result.normal * -radius);
  return result;
}

Transform operator+(const Transform& a, const Transform& b) {
  return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Transform operator-(const Transform& a, const Transform& b) {
  return {a.x - b.x, a.y - b.y, a.z - b.z};
}

float operator*(const Transform& a, const Transform& b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

Transform operator*(const Transform& a, float b) {
  return {a.x * b, a.y * b, a.z * b};
}

Transform operator/(const Transform& a, float b) {
  return {a.x / b, a.y / b, a.z / b};
}
