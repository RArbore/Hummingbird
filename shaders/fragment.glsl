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
in vec3 pos;
in vec3 norm;

out vec4 frag_color;

const vec3 light1 = vec3(1000.0f, 50.0f, 0.0f);
const vec3 light2 = vec3(-500.0f, 50.0f, 866.0f);
const vec3 light3 = vec3(-500.0f, 50.0f, -866.0f);

void main() {
    vec3 norm_norm = normalize(norm);
    vec3 light1_dir = normalize(light1-pos);
    vec3 light2_dir = normalize(light2-pos);
    vec3 light3_dir = normalize(light3-pos);
    float diff = max(dot(norm_norm, light1_dir), 0.0f) + max(dot(norm_norm, light2_dir), 0.0f) + max(dot(norm_norm, light3_dir), 0.0f) / 3.0f;
    frag_color = vec4((0.95 * diff + 0.05) * vec3(1.0f, 1.0f, 1.0f), 1.0f);
}  
