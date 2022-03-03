# pragma once

#include "physics/engine.h"

// this maxDepth subject to change, 
// specifically potentially as a function of engine.get_num_bodies(); 
static constexpr long c_maxDepth = 5; 

/**
 * @brief Class to efficiently find potential collisions
 * 
 */
class Octree {
public:
  Octree(size_t maxDepth = c_maxDepth); 

private:
  struct OctreeNode {
    OctreeNode* children; // pointer to the first child
    size_t indexBody; // in engine vectors for body
    bool isLeaf; 
  }; 

  OctreeNode* root; 
  size_t maxDepth; 
};