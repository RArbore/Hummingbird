#pragma once

#include <iostream>

#include "physics/engine.h"

void save_to_file(std::ofstream &stream, size_t num, Engine::Vec3x<float, 32> position);
Engine::Vec3x<float, 32> read_from_file(std::ifstream &stream);