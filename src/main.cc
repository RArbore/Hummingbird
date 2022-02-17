#include <iostream>

#include <GL/gl.h>
#include <GLFW/glfw3.h>

static int width, height;

int main(int argc, char **argv) {

  // TODO Parse command line args

  if (!glfwInit()) {
    std::cerr << "ERROR: Couldn't initialize GLFW. Aborting." << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  GLFWwindow *window = glfwCreateWindow(800, 600, "Hummingbird", NULL, NULL);
  if (!window) {
    std::cerr << "ERROR: Couldn't create window. Aborting." << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwShowWindow(window);
  glfwMakeContextCurrent(window);
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT);
  glfwSwapBuffers(window);

  while (!glfwWindowShouldClose(window)) {
    glfwWaitEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
