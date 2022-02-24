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

#include <iostream>
#include <variant>

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <physics/engine.h>

void resize_callback(GLFWwindow* window, int width, int height);

class Graphics {
public:
  Graphics(const Engine &engine_i);
  ~Graphics();
  int initialize();
  void render_tick();
  bool should_close() const;
private:
  GLFWwindow *window;
  const Engine &engine;
  unsigned int VAO, VBO, EBO;
  unsigned int vertex_shader, fragment_shader, shader_program;
  int transform_loc;
  glm::mat4 proj;
};
