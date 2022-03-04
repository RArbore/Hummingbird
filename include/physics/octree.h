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

#include <physics/engine.h>

// this maxDepth subject to change, 
// specifically potentially as a function of engine.get_num_bodies(); 
static constexpr unsigned int c_max_depth = 5; 

/**
 * @brief Class to efficiently find potential collisions
 * 
 */
class Octree {
public:
  explicit Octree(unsigned int max_depth_i); 

private:
  struct OctreeNode {
    unsigned int children[8];
    unsigned int index_body;
    bool is_leaf; 
  }; 

  std::vector<OctreeNode> nodes;
  size_t max_depth; 
};
