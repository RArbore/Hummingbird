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
#include <math.h>
#include <tuple>

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <physics/engine.h>

/*
 * The GLFW callbacks cannot be members of the graphics
 * context, as GLFW is fundamentally a C library, and
 * thus does not accept member functions as callback
 * functions.
 */
void mouse_callback(GLFWwindow* window, double x, double y);
void resize_callback(GLFWwindow* window, int width, int height);

/*
 * Represents the graphics context for the program.
 * This class is responsible for initializing all
 * windows, contexts, buffers, arrays, shaders, and
 * other objects needed for rendering 3D graphics.
 * Much of the initializing is done in a member
 * function rather than a constructor so that we
 * can retrieve an error code. The public interface
 * is sparse - all that our main function needs to
 * do is call render_tick and check if we should_close
 * the simulation. 
 */
class Graphics {
public:
  explicit Graphics(Engine &engine_i);
  ~Graphics();
  int initialize();
  void render_tick(const float dt);
  bool should_close() const;
private:
  GLFWwindow *window;
  Engine &engine;

  /*
   * Part of our initialization is creating an icosphere
   * mesh - this is used for rendering approximate 
   * spheres.
   */
  unsigned int num_pts, num_tris;
  std::pair<unsigned int, unsigned int> calc_icosphere_size(const unsigned int iters) const;
  std::pair<std::vector<float>, std::vector<unsigned int>> create_icosphere_mesh();

  /*
   * Helpers for initializing meshes.
   */
  void initialize_sphere_mesh();
  void initialize_walls_mesh();

  /*
   * Members representing parts of our OpenGL pipeline.
   * OpenGL represents these objects externally as integers.
   */
  unsigned int sphereVAO, sphereVBO, sphereEBO;
  unsigned int wallsVAO, wallsVBO, wallsEBO;
  unsigned int vertex_shader, fragment_shader, shader_program;
  int proj_view_loc, model_loc, normal_loc;

  /*
   * Members representing math constants - that is, elements
   * that don't change frequently.
   */
  glm::mat4 proj, identity;
  glm::vec3 cup;
  
  /*
   * Arrays where we store our model and normal matrix
   * calculations each frame, so that we can multithread
   * this calculation and refer to them later when we
   * make OpenGL API calls.
   */
  glm::mat4* model_cache;
  glm::mat4* normal_cache;

  /*
   * Camera position & rotation.
   */
  float cx, cy, cz, cphi, ctheta;

  bool released_enter = true;
  void handle_input(float dt);
};
