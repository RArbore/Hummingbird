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

#version 460 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_norm;

out vec3 pos;
out vec3 norm;

uniform mat4 proj_view;
uniform mat4 model[120];
uniform mat4 normal[120];

void main() {
    vec4 model_pos = model[gl_InstanceID] * vec4(a_pos.x, a_pos.y, a_pos.z, 1.0);
    gl_Position = proj_view * model_pos;
    pos = vec3(model_pos);
    norm = mat3(transpose(normal[gl_InstanceID])) * a_norm;
} 
