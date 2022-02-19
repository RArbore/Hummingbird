#include <iostream>

#include <interface.h>
#include <cli.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <json_file>" << std::endl;
    return -1;
  }

  Config config(argv[1]);
  
  Graphics graphics;
  if (graphics.initialize()) return -1;

  while (!graphics.should_close()) {
    graphics.render_tick();
  }

  return 0;
}
