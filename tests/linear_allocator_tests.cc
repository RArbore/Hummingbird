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

#define CATCH_CONFIG_MAIN

#include "catch2/catch.hpp"

#include "../include/linear_allocator.h"

TEST_CASE("LinearAllocator for int"){
    LinearAllocator<int> allocator;
    int* arr[20];
    for (int i = 0; i < 20; i++){
        arr[i] = allocator.allocate();
        *arr[i] = i;
    }
    for (int i = 0; i < 20; i++){
        REQUIRE(*arr[i] == i);
    }
}

typedef struct TestStruct{
    int a;
    int b;
} TestStruct;

TEST_CASE("LinearAllocator for struct"){
    LinearAllocator<TestStruct> allocator;
    TestStruct* arr[20];
    for (int i = 0; i < 20; i++){
        arr[i] = allocator.allocate();
        arr[i] -> a = i;
        arr[i] -> b = 2 * i;
    }
    for (int i = 0; i < 20; i++){
        REQUIRE(arr[i] -> a == i);
        REQUIRE(arr[i] -> b == 2 * i);
    }
}