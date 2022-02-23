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

#include <iostream>

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <interface.h>

static constexpr int DEFAULT_WIDTH = 800;
static constexpr int DEFAULT_HEIGHT = 600;

extern "C" char _binary_shaders_vertex_glsl_start;
extern "C" char _binary_shaders_fragment_glsl_start;

static int width = DEFAULT_WIDTH, height = DEFAULT_HEIGHT;
static bool resized = false;

Graphics::Graphics(const Engine &engine): window(nullptr), engine(engine) {}

Graphics::~Graphics() {
  if (window) glfwDestroyWindow(window);
  glfwTerminate();
}

int Graphics::initialize() {
  if (!glfwInit()) {
    std::cerr << "ERROR: Couldn't initialize GLFW. Aborting." << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  window = glfwCreateWindow(width, height, "Hummingbird", NULL, NULL);
  if (!window) {
    std::cerr << "ERROR: Couldn't create window. Aborting." << std::endl;
    return -1;
  }

  glfwShowWindow(window);
  glfwMakeContextCurrent(window);
  glfwGetFramebufferSize(window, &width, &height);
  glfwSetFramebufferSizeCallback(window, resize_callback);
  glViewport(0, 0, width, height);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glEnable(GL_CULL_FACE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  const char* const vertex_shader_text = &_binary_shaders_vertex_glsl_start;
  const char* const fragment_shader_text = &_binary_shaders_fragment_glsl_start;
  int shader_comp_success = 0;

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, nullptr);
  glCompileShader(vertex_shader);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_comp_success);
  if (!shader_comp_success) {
    char* const error_log = new char[1024];
    glGetShaderInfoLog(vertex_shader, 1024, nullptr, error_log);
    std::cerr << "ERROR: Couldn't compile the vertex shader. Here's the GL error log:" << std::endl;
    std::cerr << error_log << std::endl;
  }

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, nullptr);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_comp_success);
  if (!shader_comp_success) {
    char* const error_log = new char[1024];
    glGetShaderInfoLog(fragment_shader, 1024, nullptr, error_log);
    std::cerr << "ERROR: Couldn't compile the fragment shader. Here's the GL error log:" << std::endl;
    std::cerr << error_log << std::endl;
  }

  shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  glGetShaderiv(shader_program, GL_LINK_STATUS, &shader_comp_success);
  if (!shader_comp_success) {
    char* const error_log = new char[1024];
    glGetShaderInfoLog(shader_program, 1024, nullptr, error_log);
    std::cerr << "ERROR: Couldn't link the complete shader program. Here's the GL error log:" << std::endl;
    std::cerr << error_log << std::endl;
  }
  glUseProgram(shader_program);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  return 0;
}

void Graphics::render_tick() {
  glfwPollEvents();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glfwSwapBuffers(window);
  resized = false;
}

bool Graphics::should_close() const {
  return !window || glfwWindowShouldClose(window);
}

void resize_callback([[maybe_unused]] GLFWwindow* window, int new_width, int new_height) {
  width = new_width;
  height = new_height;
  resized = true;
  glViewport(0, 0, width, height);
}
