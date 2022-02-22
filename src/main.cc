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

#include <interface.h>
#include <cli.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <json_file>" << std::endl;
    return -1;
  }

  Config config(argv[1]);
  if (config.initialize()) return -1;
  
  Graphics graphics;
  if (graphics.initialize()) return -1;

  while (!graphics.should_close()) {
    graphics.render_tick();
  }

  return 0;
}
