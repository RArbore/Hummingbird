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

#pragma once

#include <fstream>
#include <memory>

#include <math.h>

/*
 * Since we're using data oriented design, each
 * collider doesn't store its own transform. When
 * checking collisions, we get our transforms passed
 * in as well.
 */
struct Transform {
  float x, y, z;
};

Transform operator+(const Transform& a, const Transform& b);
Transform operator-(const Transform& a, const Transform& b);
float operator*(const Transform& a, const Transform& b);
Transform operator*(const Transform& a, float b);
Transform operator/(const Transform& a, float b);

/*
 * A response to a collision query -  tells us more
 * than just whether a collision occurred so that we
 * can properly respond to the collision.
 */
struct CollisionResponse {
  Transform normal;
  float depth;
  bool collides;
};

enum class ColliderType { Sphere, Wall };
struct SphereCollider;
struct WallCollider;

/*
 * Every collider inherits virtual methods for
 * performing collision checks with every other
 * collider. We use double callback to deduce 
 * the type of both colliders (so that we can
 * make a concrete collision check). Colliders
 * store attributes unique to the type of body.
 * For example, sphere colliders store a radius.
 */
struct Collider {
  virtual CollisionResponse checkCollision(const Collider& other, const Transform& myPos, const Transform& otherPos) const = 0;
  virtual CollisionResponse checkCollision(const SphereCollider& other, const Transform& myPos, const Transform& otherPos) const = 0;
  virtual CollisionResponse checkCollision(const WallCollider& other, const Transform& myPos, const Transform& otherPos) const = 0;
  virtual void serialize(std::fstream& fs) const = 0;
  virtual ~Collider() = default;
};

struct SphereCollider : public Collider {
  float radius;
  explicit SphereCollider(float radius_i);
  virtual CollisionResponse checkCollision(const Collider& other, const Transform& myPos, const Transform& otherPos) const override;
  virtual CollisionResponse checkCollision(const SphereCollider& other, const Transform& myPos, const Transform& otherPos) const override;
  virtual CollisionResponse checkCollision(const WallCollider& other, const Transform& myPos, const Transform& otherPos) const override;
  virtual void serialize(std::fstream& fs) const override;
};

struct WallCollider : public Collider {
  float nx, ny, nz;
  WallCollider(float nx_i, float ny_i, float nz_i);
  virtual CollisionResponse checkCollision(const Collider& other, const Transform& myPos, const Transform& otherPos) const override;
  virtual CollisionResponse checkCollision(const SphereCollider& other, const Transform& myPos, const Transform& otherPos) const override;
  virtual CollisionResponse checkCollision(const WallCollider& other, const Transform& myPos, const Transform& otherPos) const override;
  virtual void serialize(std::fstream& fs) const override;
};

std::unique_ptr<Collider> deserialize_collider(std::fstream& fs);
