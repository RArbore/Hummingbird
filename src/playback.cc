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

#include "playback.h"

void dump_init_to_file(std::ofstream &stream, const Engine& engine) {
  stream.write(reinterpret_cast<const char*>(&engine.num_bodies), static_cast<std::streamsize>(sizeof(std::size_t)));
  for (auto i = 0; i < 6; ++i)
    stream.write(reinterpret_cast<const char*>(&engine.boundary[i]), static_cast<std::streamsize>(sizeof(float)));
}

void load_init_from_file(std::ifstream &stream, Engine& engine) {
  stream.read(reinterpret_cast<char*>(&engine.num_bodies), static_cast<std::streamsize>(sizeof(std::size_t)));
  for (auto i = 0; i < 6; ++i)
    stream.read(reinterpret_cast<char*>(&engine.boundary[i]), static_cast<std::streamsize>(sizeof(float)));
}

void dump_tick_to_file(std::ofstream &stream, const Engine& engine) {
  auto pos = engine.get_pos();
  stream.write(reinterpret_cast<const char*>(pos.x.data()), static_cast<std::streamsize>(pos.x.size() * sizeof(float)));
  stream.write(reinterpret_cast<const char*>(pos.y.data()), static_cast<std::streamsize>(pos.y.size() * sizeof(float)));
  stream.write(reinterpret_cast<const char*>(pos.z.data()), static_cast<std::streamsize>(pos.z.size() * sizeof(float)));
}

void load_tick_from_file(std::ifstream &stream, Engine& engine) {
  auto pos = engine.get_pos();
  stream.read(reinterpret_cast<char*>(pos.x.data()), static_cast<std::streamsize>(pos.x.size() * sizeof(float)));
  stream.read(reinterpret_cast<char*>(pos.y.data()), static_cast<std::streamsize>(pos.y.size() * sizeof(float)));
  stream.read(reinterpret_cast<char*>(pos.z.data()), static_cast<std::streamsize>(pos.z.size() * sizeof(float)));
}
