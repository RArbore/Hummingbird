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

Octree::Octree(const unsigned int max_depth_i, const AABB& aabb): max_depth(max_depth_i), root_bound(aabb) {}

void Octree::insert(const unsigned int to_store, const AABB& aabb) {
  insert(to_store, aabb, 0, root_bound);
}

void Octree::insert(const unsigned int to_store, const AABB& aabb, const unsigned int root, const AABB& node_aabb) {
  auto& node = nodes.at(root);
  if (node.num_stored < C_MAX_DEPTH) node.bodies[node.num_stored++] = to_store;
  else {
    if (!node.first_child) {
      node.first_child = static_cast<unsigned int>(nodes.size());
      for (unsigned int i = 0; i < 8; ++i) nodes.emplace_back();
    }
    AABB sub_aabb;
    if (intersects(aabb, sub_aabb = get_sub_aabb<false, false, false>(node_aabb))) insert(to_store, aabb, node.first_child, sub_aabb);
    if (intersects(aabb, sub_aabb = get_sub_aabb<true, false, false>(node_aabb))) insert(to_store, aabb, node.first_child + 1, sub_aabb);
    if (intersects(aabb, sub_aabb = get_sub_aabb<false, true, false>(node_aabb))) insert(to_store, aabb, node.first_child + 2, sub_aabb);
    if (intersects(aabb, sub_aabb = get_sub_aabb<true, true, false>(node_aabb))) insert(to_store, aabb, node.first_child + 3, sub_aabb);
    if (intersects(aabb, sub_aabb = get_sub_aabb<false, false, true>(node_aabb))) insert(to_store, aabb, node.first_child + 4, sub_aabb);
    if (intersects(aabb, sub_aabb = get_sub_aabb<true, false, true>(node_aabb))) insert(to_store, aabb, node.first_child + 5, sub_aabb);
    if (intersects(aabb, sub_aabb = get_sub_aabb<false, true, true>(node_aabb))) insert(to_store, aabb, node.first_child + 6, sub_aabb);
    if (intersects(aabb, sub_aabb = get_sub_aabb<true, true, true>(node_aabb))) insert(to_store, aabb, node.first_child + 7, sub_aabb);
  }
}

bool Octree::Node::is_leaf() {
  return !first_child;
}
