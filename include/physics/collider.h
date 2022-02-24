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

struct SphereCollider;

struct Collider {
  virtual CollisionResponse checkCollision(const Collider& other, const Transform& myPos, const Transform& otherPos) const = 0;
  virtual CollisionResponse checkCollision(const SphereCollider& other, const Transform& myPos, const Transform& otherPos) const = 0;
  virtual ~Collider() = default;
};

struct SphereCollider : public Collider {
  float radius;
  SphereCollider(float radius_i);
  virtual CollisionResponse checkCollision(const Collider& other, const Transform& myPos, const Transform& otherPos) const override;
  virtual CollisionResponse checkCollision(const SphereCollider& other, const Transform& myPos, const Transform& otherPos) const override;
};
