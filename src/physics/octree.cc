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

__attribute__((always_inline))
inline bool intersects(const AABB& aabb1, const AABB& aabb2) {
  return (aabb1.x1 <= aabb2.x2) & (aabb2.x1 <= aabb1.x2)
    & (aabb1.y1 <= aabb2.y2) & (aabb2.y1 <= aabb1.y2)
    & (aabb1.z1 <= aabb2.z2) & (aabb2.z1 <= aabb1.z2);
}

Octree::Octree(const AABB& aabb): root_bound(aabb), nodes(1) {}

/*
 * Insert a body into the octree. While the
 * AABB determines where to insert, only the
 * body's ID is actually stored.
 */
void Octree::insert(const unsigned int to_store, const AABB& aabb) {
  insert(to_store, aabb, 0, root_bound);
}

void Octree::insert(const unsigned int to_store, const AABB& aabb, const unsigned int root, const AABB& node_aabb) {
  /*
   * Only insert if body is in tree node.
   */
  if (!intersects(aabb, node_aabb)) return;

  /*
   * Only insert in current node if there's
   * space.
   */
  auto& node = nodes[root];
  if (node.num_stored < NODE_SIZE) {
    node.bodies[node.num_stored++] = to_store;
    return;
  }

  /*
   * If we haven't already created the current
   * node's children, create them. When we add
   * nodes to the tree's vector, we 
   * potentially invalidate the node reference,
   * as insertions may result in re-allocations.
   */
  unsigned int first_child = node.first_child;
  if (!node.first_child) {
    first_child = static_cast<unsigned int>(nodes.size());
    node.first_child = first_child;
    for (unsigned int i = 0; i < 8; ++i) nodes.emplace_back(); // Reference node is invalid after this point
  }

  /*
   * Attempt to insert into all children that
   * we overlap.
   */
  insert(to_store, aabb, first_child, get_sub_aabb<false, false, false>(node_aabb));
  insert(to_store, aabb, first_child + 1, get_sub_aabb<true, false, false>(node_aabb));
  insert(to_store, aabb, first_child + 2, get_sub_aabb<false, true, false>(node_aabb));
  insert(to_store, aabb, first_child + 3, get_sub_aabb<true, true, false>(node_aabb));
  insert(to_store, aabb, first_child + 4, get_sub_aabb<false, false, true>(node_aabb));
  insert(to_store, aabb, first_child + 5, get_sub_aabb<true, false, true>(node_aabb));
  insert(to_store, aabb, first_child + 6, get_sub_aabb<false, true, true>(node_aabb));
  insert(to_store, aabb, first_child + 7, get_sub_aabb<true, true, true>(node_aabb));
}

/*
 * Query for possible collisions between
 * an input AABB and AABBs inserted into
 * the tree. We also pass in the ID of the
 * body we're querying, as we don't want
 * to double count collisions (we only
 * return bodies whose ID is larger than
 * the one passed in).
 */
void Octree::possibilities(const unsigned int id, const AABB& aabb, std::unordered_set<unsigned int>& dest) {
  possibilities(id, aabb, dest, 0, root_bound);
}

void Octree::possibilities(const unsigned int id, const AABB& aabb, std::unordered_set<unsigned int>& dest, const unsigned int root, const AABB& node_aabb) {
  /*
   * If we don't intersect the body's
   * AABB, we know we're done.
   */
  if (!intersects(aabb, node_aabb)) return;

  /*
   * Add bodies from current node to
   * the vector of possible collisions.
   * We only allow collisions with bodies
   * whose ID is larger than the query.
   * This is an easy way to avoid double
   * counting collisions.
   */
  auto& node = nodes[root];
  for (unsigned int i = 0; i < node.num_stored; ++i) {
    auto body = node.bodies[i];
    if (id < body) dest.insert(body);
  }

  /*
   * If we have children, we must query
   * them too.
   */
  unsigned int first_child = node.first_child;
  if (first_child) {
    possibilities(id, aabb, dest, first_child, get_sub_aabb<false, false, false>(node_aabb));
    possibilities(id, aabb, dest, first_child + 1, get_sub_aabb<true, false, false>(node_aabb));
    possibilities(id, aabb, dest, first_child + 2, get_sub_aabb<false, true, false>(node_aabb));
    possibilities(id, aabb, dest, first_child + 3, get_sub_aabb<true, true, false>(node_aabb));
    possibilities(id, aabb, dest, first_child + 4, get_sub_aabb<false, false, true>(node_aabb));
    possibilities(id, aabb, dest, first_child + 5, get_sub_aabb<true, false, true>(node_aabb));
    possibilities(id, aabb, dest, first_child + 6, get_sub_aabb<false, true, true>(node_aabb));
    possibilities(id, aabb, dest, first_child + 7, get_sub_aabb<true, true, true>(node_aabb));
  }
}

bool Octree::Node::is_leaf() {
  return !first_child;
}
