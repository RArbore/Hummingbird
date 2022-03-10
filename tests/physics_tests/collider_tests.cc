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
    
#include "catch2/catch.hpp"
#include <iostream>

#include "../../include/physics/collider.h"
bool smallDiff(float f1, float f2);
void REQUIRE_SAME_RESPONSE(CollisionResponse response1, CollisionResponse response2);


void REQUIRE_SAME_RESPONSE(CollisionResponse response1, CollisionResponse response2){
    REQUIRE(response1.collides == response2.collides);
    REQUIRE(smallDiff(response1.depth, response2.depth));
    REQUIRE(smallDiff(response1.normal.x, response2.normal.x));
    REQUIRE(smallDiff(response1.normal.y, response2.normal.y));
    REQUIRE(smallDiff(response1.normal.z, response2.normal.z));
}

TEST_CASE("Two sphere no collide test 1","[SphereCollider]"){
    // Sum of radii is 7.0
    SphereCollider collider1(3.0f);
    SphereCollider collider2(4.0f);
    // Distance between centres is 13.0
    Transform pos1 {0.0f, 0.0f, 0.0f};
    Transform pos2 {3.0f, 4.0f, 12.0f};
    CollisionResponse response = collider1.checkCollision(collider2, pos1, pos2);
    REQUIRE(!response.collides);
}

TEST_CASE("Two sphere no collide test 2","[SphereCollider]"){
    // Sum of radii is 12.9
    SphereCollider collider1(8.6f);
    SphereCollider collider2(4.3f);
    // Distance between centres is 13.0
    Transform pos1 {0.0f, 0.0f, 0.0f};
    Transform pos2 {3.0f, 4.0f, 12.0f};
    CollisionResponse response = collider1.checkCollision(collider2, pos1, pos2);
    REQUIRE(!response.collides);
}

TEST_CASE("Two sphere collide edge case","[SphereCollider]"){
    // Sum of radii is 13.0
    SphereCollider collider1(8.0f);
    SphereCollider collider2(5.0f);
    // Distance between centres is 13.0
    Transform pos1 {0.0f, 0.0f, 0.0f};
    Transform pos2 {3.0f, 4.0f, 12.0f};
    CollisionResponse response = collider1.checkCollision(collider2, pos1, pos2);
    CollisionResponse expected_response {
        {
            3.0f / 13.0f,
            4.0f / 13.0f,
            12.0f / 13.0f    
        },
        13.0f,
        true,
    };
    REQUIRE_SAME_RESPONSE(response, expected_response);
}

TEST_CASE("Two sphere collide case","[SphereCollider]"){
    // Sum of radii is 12.2
    SphereCollider collider1(8.0f);
    SphereCollider collider2(4.2f);
    // Distance between centres is 12.083045974
    Transform pos1 {0.0f, 0.0f, 0.0f};
    Transform pos2 {3.0f, 4.0f, 11.0f};
    CollisionResponse response = collider1.checkCollision(collider2, pos1, pos2);
    CollisionResponse expected_response {
        {
            3.0f / 12.083045974f,
            4.0f / 12.083045974f,
            11.0f / 12.083045974f    
        },
        12.083045974f,
        true,
    };
    REQUIRE_SAME_RESPONSE(response, expected_response);
}