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

#include <unordered_set>
#include <vector>

/*
 * When checking bounds for objects, we only
 * consider AABBs - we can deal with
 * body-specific complexity after querying
 * possible collisions.
 */
static constexpr unsigned int NODE_SIZE = 16;

struct AABB {
  float x1, x2, y1, y2, z1, z2;
};

/*
 * Get child AABB, splitting parent AABB
 * into eight pieces.
 */
template<bool x, bool y, bool z>
__attribute__((always_inline))
inline AABB get_sub_aabb(const AABB& aabb) {
  return {
    x ? 0.5f * (aabb.x1 + aabb.x2) : aabb.x1, x ? aabb.x2 : 0.5f * (aabb.x1 + aabb.x2),
    y ? 0.5f * (aabb.y1 + aabb.y2) : aabb.y1, y ? aabb.y2 : 0.5f * (aabb.y1 + aabb.y2),
    z ? 0.5f * (aabb.z1 + aabb.z2) : aabb.z1, z ? aabb.z2 : 0.5f * (aabb.z1 + aabb.z2)
  };
}

/*
 * Octree class implements a linear octree.
 * This allows for the efficient insertion
 * and querying of AABBs. Each node is
 * stored in a vector, and "pointers" are
 * indices into this vector.
 */
class Octree {
public:
  explicit Octree(const AABB& aabb); 

  void insert(const unsigned int to_store, const AABB& aabb);
  void possibilities(const unsigned int id, const AABB& aabb, std::unordered_set<unsigned int>& dest);

private:
  struct Node {
    Node(): num_stored(0), first_child(0), bodies{} {}
    unsigned int num_stored;
    unsigned int first_child;
    unsigned int bodies[NODE_SIZE]; 
    bool is_leaf();
  }; 

  void insert(const unsigned int to_store, const AABB& aabb, const unsigned int root, const AABB& node_aabb);
  void possibilities(const unsigned int id, const AABB& aabb, std::unordered_set<unsigned int>& dest, const unsigned int root, const AABB& node_aabb);

  AABB root_bound;
  std::vector<Node> nodes;
};
