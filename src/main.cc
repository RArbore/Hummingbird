#include <iostream>

#include <GLFW/glfw3.h>
#include <GL/gl.h>

int main(int argc, char **argv) {

    // TODO Parse command line args
    
    if (!glfwInit()) {
	std::cerr << "ERROR: Couldn't initialize GLFW. Aborting." << std::endl;
	return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Hummingbird", NULL, NULL);
    if (!window) {
	std::cerr << "ERROR: Couldn't initialize GLFW. Aborting." << std::endl;
	glfwTerminate();
	return -1;
    }

    while (true);

    glfwTerminate();
    return 0;
}
