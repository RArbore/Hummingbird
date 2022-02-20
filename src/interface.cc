#include <iostream>

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <interface.h>

static constexpr int DEFAULT_WIDTH = 800;
static constexpr int DEFAULT_HEIGHT = 600;

Graphics::Graphics(): window(nullptr), width(DEFAULT_WIDTH), height(DEFAULT_HEIGHT) {}

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
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT);
  glfwSwapBuffers(window);

  return 0;
}

void Graphics::render_tick() {
  glfwWaitEvents();
}

bool Graphics::should_close() {
  return !window || glfwWindowShouldClose(window);
}
