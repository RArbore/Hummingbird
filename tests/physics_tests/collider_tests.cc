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

TEST_CASE("Two sphere no collide test 3","[SphereCollider]"){
    // Sum of radii is 64.0
    SphereCollider collider1(32.0f);
    SphereCollider collider2(32.0f);
    // Distance between centres is 65.0
    Transform pos1 {30.0f, 60.0f, 90.0f};
    Transform pos2 {37.0f, 84.0f, 150.0f};
    CollisionResponse response = collider1.checkCollision(collider2, pos1, pos2);
    REQUIRE(!response.collides);
}

TEST_CASE("Two sphere no collide test 4","[SphereCollider]"){
    // Sum of radii is 12.0
    SphereCollider collider1(6.0f);
    SphereCollider collider2(6.0f);
    // Distance between centres is more than 15.0
    Transform pos1 {18.0f, 0.0f, 0.0f};
    Transform pos2 {3.0f, 4.0f, 12.0f};
    CollisionResponse response = collider1.checkCollision(collider2, pos1, pos2);
    REQUIRE(!response.collides);
}

TEST_CASE("Two sphere no collide test 5","[SphereCollider]"){
    // Sum of radii is 10.0
    SphereCollider collider1(4.0f);
    SphereCollider collider2(6.0f);
    // Distance between centres is 11.0
    Transform pos1 {30.0f, 25.0f, 25.0f};
    Transform pos2 {41.0f, 25.0f, 25.0f};
    CollisionResponse response = collider1.checkCollision(collider2, pos1, pos2);
    REQUIRE(!response.collides);
}

TEST_CASE("Two sphere collides test","[SphereCollider]"){
    // Sum of radii is 65.1
    SphereCollider collider1(33.1f);
    SphereCollider collider2(32.0f);
    // Distance between centres is 65.0
    Transform pos1 {30.0f, 60.0f, 90.0f};
    Transform pos2 {37.0f, 84.0f, 150.0f};
    CollisionResponse response = collider1.checkCollision(collider2, pos1, pos2);
    REQUIRE(response.collides);
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
        0.0f,
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
        0.116954026f,
        true,
    };
    REQUIRE_SAME_RESPONSE(response, expected_response);
}

TEST_CASE("Sphere and wall no collide","[WallCollider]"){
    SphereCollider collider1(2.0f);
    WallCollider collider2(0.707106781f, 0.707106781f, 0.0f);
    Transform pos1 {2.0f, 2.0f, 2.0f};
    Transform pos2 {0.0f, 0.0f, 0.0f};
    CollisionResponse response = collider1.checkCollision(collider2, pos1, pos2);
    REQUIRE(!response.collides);
}

TEST_CASE("Sphere and wall collide edge case","[WallCollider]"){
    SphereCollider collider1(2.05f);
    WallCollider collider2(1.0f, 0.0f, 0.0f);
    Transform pos1 {2.0f, 0.0f, 0.0f};
    Transform pos2 {0.0f, 0.0f, 0.0f};
    CollisionResponse response = collider1.checkCollision(collider2, pos1, pos2);
    CollisionResponse expected_response {
        {
            1.0f,
            0.0f,
            0.0f    
        },
        0.05f,
        true,
    };
    REQUIRE_SAME_RESPONSE(response, expected_response);
}

TEST_CASE("Sphere and wall collides test","[WallCollider]"){
    SphereCollider collider1(2.0f);
    WallCollider collider2(1.0f, 0.0f, 0.0f);
    Transform pos1 {1.0f, 0.0f, 0.0f};
    Transform pos2 {0.0f, 0.0f, 0.0f};
    CollisionResponse response = collider1.checkCollision(collider2, pos1, pos2);
    CollisionResponse expected_response {
        {
            1.0f,
            0.0f,
            0.0f    
        },
        1.0f,
        true,
    };
    REQUIRE_SAME_RESPONSE(response, expected_response);
}


TEST_CASE("Two wall no collide test","[WallCollider]"){
    // Two walls will never collide
    WallCollider collider1(1.0f, 0.0f, 0.0f);
    WallCollider collider2(1.0f, 3.0f, 2.0f);
    Transform pos1 {0.0f, 0.0f, 0.0f};
    Transform pos2 {3.0f, 4.0f, 12.0f};
    CollisionResponse response = collider1.checkCollision(collider2, pos1, pos2);
    REQUIRE(!response.collides);
}

TEST_CASE("Two wall no collide test 2","[WallCollider]"){
    // Two walls will never collide
    WallCollider collider1(3.0f, 0.0f, 0.0f);
    WallCollider collider2(10.0f, 3.0f, 3.0f);
    Transform pos1 {0.0f, 0.0f, 0.0f};
    Transform pos2 {32.0f, 34.0f, 22.0f};
    CollisionResponse response = collider1.checkCollision(collider2, pos1, pos2);
    REQUIRE(!response.collides);
}