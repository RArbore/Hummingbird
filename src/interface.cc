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

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <interface.h>

static constexpr int DEFAULT_WIDTH = 800;
static constexpr int DEFAULT_HEIGHT = 600;

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
  glClear(GL_COLOR_BUFFER_BIT);

  return 0;
}

void Graphics::render_tick() {
  glfwPollEvents();
  glClear(GL_COLOR_BUFFER_BIT);



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
