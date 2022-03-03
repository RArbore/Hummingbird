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

#include <vector>
#include <stdlib.h>
/**
    LinearAllocator allocates a block of memory first and uses doubling technique
    when all available spaces are used. In this way, we can reduce the number of 
    system calls and increase the performance.  
    @tparam T the type to allocate memory for
*/

template<typename T>
class LinearAllocator{
private:
   std::vector<T*> heads_;
   std::vector<size_t> capacities_;
   size_t index;
   const size_t INITIAL_CAPACITY = 16;
public:
    LinearAllocator();
    ~LinearAllocator();
    T* allocate();
};

template<typename T> LinearAllocator<T> :: LinearAllocator(){
    T* ptr = static_cast<T*>(malloc(INITIAL_CAPACITY * sizeof(T)));
    heads_.push_back(ptr);
    capacities_.push_back(INITIAL_CAPACITY);
    index = 0;
}

template<typename T> LinearAllocator<T> :: ~LinearAllocator(){
    for (T* ptr : heads_){
        free(ptr);
    }
}

template<typename T> T* LinearAllocator<T> :: allocate(){
    size_t curr_capacity = capacities_[capacities_.size() - 1];
    if (index == curr_capacity){
        T* ptr = static_cast<T*>(malloc (curr_capacity * 2 * sizeof(T)));
        heads_.push_back(ptr);
        capacities_.push_back(curr_capacity * 2);
        index = 0;
    }
    T* curr = heads_[heads_.size() - 1] + index;
    ++ index;
    return curr;
}