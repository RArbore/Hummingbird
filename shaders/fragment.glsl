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
in vec3 pos;
in vec3 norm;

out vec4 frag_color;

void main() {
    vec3 norm_norm = normalize(norm);
    vec3 light_dir = normalize(-pos);
    float diff = max(dot(norm_norm, light_dir), 0.0f);
    frag_color = vec4(diff * vec3(1.0f, 1.0f, 1.0f), 1.0f);
}  
