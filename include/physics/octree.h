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

#include <vector>

// specifically potentially as a function of engine.get_num_bodies(); 
static constexpr unsigned int C_MAX_DEPTH = 5; 
static constexpr unsigned int C_NODE_SIZE = 64;

struct AABB {
  float x1, x2, y1, y2, z1, z2;
};

bool intersects(const AABB& aabb1, const AABB& aabb2);

template<bool x, bool y, bool z>
__attribute__((always_inline))
inline AABB get_sub_aabb(const AABB& aabb) {
  return {
    x ? 0.5f * (aabb.x1 + aabb.x2) : aabb.x1, x ? aabb.x2 : 0.5f * (aabb.x1 + aabb.x2),
    y ? 0.5f * (aabb.y1 + aabb.y2) : aabb.y1, y ? aabb.y2 : 0.5f * (aabb.y1 + aabb.y2),
    z ? 0.5f * (aabb.z1 + aabb.z2) : aabb.z1, z ? aabb.z2 : 0.5f * (aabb.z1 + aabb.z2)
  };
}

/**
 * @brief Class to efficiently find potential collisions
 * 
 */
class Octree {
public:
  explicit Octree(const unsigned int max_depth_i, const AABB& aabb); 

  void insert(const unsigned int to_store, const AABB& aabb);

private:
  struct Node {
    Node(): num_stored(0), first_child(0), bodies{0, 0, 0, 0, 0, 0, 0, 0} {}
    unsigned int num_stored;
    unsigned int first_child;
    unsigned int bodies[C_NODE_SIZE]; 
    bool is_leaf();
  }; 

  void insert(const unsigned int to_store, const AABB& aabb, const unsigned int root, const AABB& node_aabb);

  unsigned int max_depth; 
  AABB root_bound;
  std::vector<Node> nodes;
};
