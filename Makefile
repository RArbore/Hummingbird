#   This file is part of Hummingbird. \
\
    Hummingbird is free software: you can redistribute it and/or modify \
    it under the terms of the GNU Lesser General Public License as published by \
    the Free Software Foundation, either version 3 of the License, or \
    any later version. \
\
    Hummingbird is distributed in the hope that it will be useful, \
    but WITHOUT ANY WARRANTY; without even the implied warranty of \
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \
    GNU Lesser General Public License for more details. \
\
    You should have received a copy of the GNU Lesser General Public License \
    along with Hummingbird. If not, see <https://www.gnu.org/licenses/>.

CXX=g++
LD=g++

W_FLAGS=-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wswitch-default -Wundef -Werror -Wno-unused -Wconversion

CXX_FLAGS=-std=c++17 -Ofast -fopenmp -mavx -march=native -Iinclude $(W_FLAGS)

L_FLAGS=-L/usr/lib/x86_64-linux-gnu -lglfw -lGL -ljsoncpp -fopenmp

hummingbird: build/main.o build/interface.o build/cli.o build/engine.o build/collider.o build/quaternion.o build/octree.o build/vertex.o build/fragment.o
	$(LD) -o $@ $^ $(L_FLAGS)
build/main.o: src/main.cc include/physics/engine.h include/interface.h include/cli.h
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
build/interface.o: src/interface.cc include/interface.h include/physics/engine.h
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
build/cli.o: src/cli.cc include/cli.h
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
build/engine.o: src/physics/engine.cc include/physics/engine.h include/physics/collider.h include/physics/quaternion.h include/cli.h
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
build/collider.o: src/physics/collider.cc include/physics/collider.h
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
build/quaternion.o: src/physics/quaternion.cc include/physics/quaternion.h
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
build/octree.o: src/physics/octree.cc include/physics/octree.h
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
build/vertex.o: shaders/vertex.glsl
	objcopy --input binary --output elf64-x86-64 $< $@
build/fragment.o: shaders/fragment.glsl
	objcopy --input binary --output elf64-x86-64 $< $@

test: build/cli.o build/quattests.o build/tests.o build/quaternion.o
	$(LD) $(L_FLAGS) -o $@ $^
build/tests.o: tests/cli_tests.cc
	$(CXX) $(CXX_FLAGS) -c $^ -o $@
build/quattests.o: tests/physics_tests/quat_tests.cc
	$(CXX) $(CXX_FLAGS) -c $^ -o $@

exe: hummingbird
	__GL_SYNC_TO_VBLANK=0 ./hummingbird example.json
exe_test: test
	./test
clean:
	rm -rf build/*.o
	rm -rf hummingbird
	rm -rf test

.DEFAULT: hummingbird
.PHONY: exe exe_test clean
