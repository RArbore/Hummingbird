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
WallCollider::WallCollider(float nx_i, float ny_i, float nz_i): nx(nx_i), ny(ny_i), nz(nz_i) {}

/*
 * Double callback functions to deduce other collider type.
 */
CollisionResponse SphereCollider::checkCollision(const Collider& other, const Transform& myPos, const Transform& otherPos) const {
  return other.checkCollision(*this, otherPos, myPos);
}

CollisionResponse WallCollider::checkCollision(const Collider& other, const Transform& myPos, const Transform& otherPos) const {
  return other.checkCollision(*this, otherPos, myPos);
}

/*
 * Check collision between 2 spheres.
 */
CollisionResponse SphereCollider::checkCollision(const SphereCollider& other, const Transform& myPos, const Transform& otherPos) const {
  CollisionResponse result;
  Transform BmA = otherPos - myPos;
  float depth2 = BmA * BmA;
  result.collides = depth2 <= (radius + other.radius) * (radius + other.radius);
  if (result.collides) {
    float sqrt_depth2 = sqrt(depth2);
    result.depth = radius + other.radius - sqrt_depth2;
    result.normal = BmA / sqrt_depth2;
  }
  return result;
}

CollisionResponse SphereCollider::checkCollision(const WallCollider& other, const Transform& myPos, const Transform& otherPos) const {
  CollisionResponse result;
  result.normal = Transform{other.nx, other.ny, other.nz};
  float sphere_dist = myPos * result.normal;
  float plane_dist = otherPos * result.normal;
  float dist_diff = sphere_dist - plane_dist;
  result.collides = dist_diff < radius;
  if (result.collides) {
    result.depth = radius - dist_diff;
  }
  return result;
}

CollisionResponse WallCollider::checkCollision(const SphereCollider& other, const Transform& myPos, const Transform& otherPos) const {
  return other.checkCollision(*this, otherPos, myPos);
}

CollisionResponse WallCollider::checkCollision([[maybe_unused]] const WallCollider& other, [[maybe_unused]] const Transform& myPos, [[maybe_unused]] const Transform& otherPos) const {
  CollisionResponse result;
  result.collides = false;
  return result;
}

void SphereCollider::serialize(std::fstream& fs) const {
  ColliderType type = ColliderType::Sphere;
  fs.write(reinterpret_cast<const char*>(&type), static_cast<std::streamsize>(sizeof(ColliderType)));
  fs.write(reinterpret_cast<const char*>(&radius), static_cast<std::streamsize>(sizeof(float)));
}

void WallCollider::serialize(std::fstream& fs) const {
  ColliderType type = ColliderType::Wall;
  fs.write(reinterpret_cast<const char*>(&type), static_cast<std::streamsize>(sizeof(ColliderType)));
  fs.write(reinterpret_cast<const char*>(&nx), static_cast<std::streamsize>(sizeof(float)));
  fs.write(reinterpret_cast<const char*>(&ny), static_cast<std::streamsize>(sizeof(float)));
  fs.write(reinterpret_cast<const char*>(&nz), static_cast<std::streamsize>(sizeof(float)));
}

std::unique_ptr<Collider> deserialize_collider(std::fstream& fs) {
  ColliderType type;
  fs.read(reinterpret_cast<char*>(&type), static_cast<std::streamsize>(sizeof(ColliderType)));
  switch (type) {
  case ColliderType::Sphere: {
    float radius;
    fs.read(reinterpret_cast<char*>(&radius), static_cast<std::streamsize>(sizeof(float)));
    return std::make_unique<SphereCollider>(radius);
  }
  case ColliderType::Wall: {
    float nx, ny, nz;
    fs.read(reinterpret_cast<char*>(&nx), static_cast<std::streamsize>(sizeof(float)));
    fs.read(reinterpret_cast<char*>(&ny), static_cast<std::streamsize>(sizeof(float)));
    fs.read(reinterpret_cast<char*>(&nz), static_cast<std::streamsize>(sizeof(float)));
    return std::make_unique<WallCollider>(nx, ny, nz);
  }
  default: return nullptr;
  }
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
