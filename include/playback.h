#pragma once

#include <iostream>

#include "physics/engine.h"

void dump_to_file(std::ofstream &stream, const Engine& engine);
void load_from_file(std::ifstream &stream, Engine& engine);
