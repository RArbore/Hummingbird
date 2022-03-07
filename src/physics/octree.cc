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

#include <physics/octree.h>

bool intersects(const AABB& aabb1, const AABB& aabb2) {
  return aabb1.x1 <= aabb2.x2 && aabb2.x1 <= aabb1.x2
    && aabb1.y1 <= aabb2.y2 && aabb2.y1 <= aabb1.y2
    && aabb1.z1 <= aabb2.z2 && aabb2.z1 <= aabb1.z2;
}

Octree::Octree(const AABB& aabb): root_bound(aabb), nodes(1) {}

void Octree::insert(const unsigned int to_store, const AABB& aabb) {
  insert(to_store, aabb, 0, root_bound);
}

void Octree::insert(const unsigned int to_store, const AABB& aabb, const unsigned int root, const AABB& node_aabb) {
  if (!intersects(aabb, node_aabb)) return;
  auto& node = nodes[root];
  if (node.num_stored < NODE_SIZE) {
    node.bodies[node.num_stored++] = to_store;
    return;
  }
  unsigned int first_child = node.first_child;
  if (!node.first_child) {
    first_child = static_cast<unsigned int>(nodes.size());
    node.first_child = first_child;
    for (unsigned int i = 0; i < 8; ++i) nodes.emplace_back(); // Reference node is invalid after this point
  }
  insert(to_store, aabb, first_child, get_sub_aabb<false, false, false>(node_aabb));
  insert(to_store, aabb, first_child + 1, get_sub_aabb<true, false, false>(node_aabb));
  insert(to_store, aabb, first_child + 2, get_sub_aabb<false, true, false>(node_aabb));
  insert(to_store, aabb, first_child + 3, get_sub_aabb<true, true, false>(node_aabb));
  insert(to_store, aabb, first_child + 4, get_sub_aabb<false, false, true>(node_aabb));
  insert(to_store, aabb, first_child + 5, get_sub_aabb<true, false, true>(node_aabb));
  insert(to_store, aabb, first_child + 6, get_sub_aabb<false, true, true>(node_aabb));
  insert(to_store, aabb, first_child + 7, get_sub_aabb<true, true, true>(node_aabb));
}

void Octree::possibilities(const AABB& aabb, std::unordered_set<unsigned int>& dest) {
  possibilities(aabb, dest, 0, root_bound);
}

void Octree::possibilities(const AABB& aabb, std::unordered_set<unsigned int>& dest, const unsigned int root, const AABB& node_aabb) {
  if (!intersects(aabb, node_aabb)) return;
  auto& node = nodes[root];
  for (unsigned int i = 0; i < node.num_stored; ++i) {
    dest.insert(node.bodies[i]);
  }
  unsigned int first_child = node.first_child;
  if (first_child) {
    possibilities(aabb, dest, first_child, get_sub_aabb<false, false, false>(node_aabb));
    possibilities(aabb, dest, first_child + 1, get_sub_aabb<true, false, false>(node_aabb));
    possibilities(aabb, dest, first_child + 2, get_sub_aabb<false, true, false>(node_aabb));
    possibilities(aabb, dest, first_child + 3, get_sub_aabb<true, true, false>(node_aabb));
    possibilities(aabb, dest, first_child + 4, get_sub_aabb<false, false, true>(node_aabb));
    possibilities(aabb, dest, first_child + 5, get_sub_aabb<true, false, true>(node_aabb));
    possibilities(aabb, dest, first_child + 6, get_sub_aabb<false, true, true>(node_aabb));
    possibilities(aabb, dest, first_child + 7, get_sub_aabb<true, true, true>(node_aabb));
  }
}

bool Octree::Node::is_leaf() {
  return !first_child;
}
