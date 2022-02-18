#include <iostream>

#include <interface.h>

static int width, height;

int main(int argc, char **argv) {

  // TODO Parse command line args
  Graphics graphics;
  if (graphics.initialize()) return -1;

  while (!graphics.should_close()) {
    graphics.render_tick();
  }

  return 0;
}
