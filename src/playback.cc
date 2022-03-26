#include "playback.h"

void dump_to_file(std::ofstream &stream, const Engine& engine) {
  auto pos = engine.get_pos();
  stream.write(reinterpret_cast<const char*>(pos.x.data()), static_cast<std::streamsize>(pos.x.size() * sizeof(float)));
  stream.write(reinterpret_cast<const char*>(pos.y.data()), static_cast<std::streamsize>(pos.y.size() * sizeof(float)));
  stream.write(reinterpret_cast<const char*>(pos.z.data()), static_cast<std::streamsize>(pos.z.size() * sizeof(float)));
}

void load_from_file(std::ifstream &stream, Engine& engine) {
  auto pos = engine.get_pos();
  stream.read(reinterpret_cast<char*>(pos.x.data()), static_cast<std::streamsize>(pos.x.size() * sizeof(float)));
  stream.read(reinterpret_cast<char*>(pos.y.data()), static_cast<std::streamsize>(pos.y.size() * sizeof(float)));
  stream.read(reinterpret_cast<char*>(pos.z.data()), static_cast<std::streamsize>(pos.z.size() * sizeof(float)));
}
