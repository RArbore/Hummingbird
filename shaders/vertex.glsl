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

#version 330 core
layout (location = 0) in vec3 a_pos;

uniform mat4 proj_view;
uniform mat4 model[8];

void main() {
    gl_Position = proj_view * model[gl_InstanceID] * vec4(a_pos.x, a_pos.y, a_pos.z, 1.0);
} 
