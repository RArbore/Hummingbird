#include <GL/gl.h>
#include <GLFW/glfw3.h>

class Graphics {
public:
  Graphics();
  ~Graphics();
  int initialize();
  void render_tick();
  bool should_close();

private:
  GLFWwindow *window;
  int width, height;
};
