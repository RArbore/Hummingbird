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

CXX_FLAGS=-std=c++17 -Ofast -flto -fno-signed-zeros -fno-trapping-math -frename-registers -funroll-loops -fopenmp -D_GLIBCXX_PARALLEL -mavx -march=native -Iinclude $(W_FLAGS)

COV_FLAGS=$(CXX_FLAGS) --coverage

L_FLAGS=-L/usr/lib/x86_64-linux-gnu -lglfw -lGL -ljsoncpp -fopenmp -flto

hummingbird: build/main.o build/interface.o build/cli.o build/engine.o build/collider.o build/quaternion.o build/octree.o build/vertex.o build/fragment.o
	$(LD) -o $@ $^ $(L_FLAGS)
build/main.o: src/main.cc include/physics/engine.h include/interface.h include/cli.h
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
build/interface.o: src/interface.cc include/interface.h include/physics/engine.h
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
build/cli.o: src/cli.cc include/cli.h
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
build/engine.o: src/physics/engine.cc include/physics/engine.h include/physics/collider.h include/physics/quaternion.h include/physics/octree.h include/cli.h
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

test: build/cli.o build/engine.o build/quattests.o build/tests.o build/quaternion.o build/collidertests.o build/collider.o build/serializationtest.o
	$(LD) $(L_FLAGS) -o $@ $^
build/tests.o: tests/cli_tests.cc
	$(CXX) $(CXX_FLAGS) -c $^ -o $@
build/quattests.o: tests/physics_tests/quat_tests.cc
	$(CXX) $(CXX_FLAGS) -c $^ -o $@
build/collidertests.o: tests/physics_tests/collider_tests.cc
	$(CXX) $(CXX_FLAGS) -c $^ -o $@
build/serializationtest.o: tests/physics_tests/serialization_tests.cc
	$(CXX) $(CXX_FLAGS) -c $^ -o $@


coverage: build/coverage/cli.o build/coverage/engine.o build/coverage/quattests.o build/coverage/tests.o build/coverage/quaternion.o build/coverage/collidertests.o build/coverage/collider.o build/coverage/playback.o build/coverage/serializationtest.o
	$(LD) $(L_FLAGS) --coverage -o $@ $^
build/coverage/tests.o: tests/cli_tests.cc
	$(CXX) $(COV_FLAGS) -c $^ -o $@ --coverage
build/coverage/quattests.o: tests/physics_tests/quat_tests.cc
	$(CXX) $(COV_FLAGS) -c $^ -o $@ --coverage
build/coverage/collidertests.o: tests/physics_tests/collider_tests.cc
	$(CXX) $(COV_FLAGS) -c $^ -o $@ --coverage
build/coverage/serializationtest.o: tests/physics_tests/serialization_tests.cc
	$(CXX) $(COV_FLAGS) -c $^ -o $@ --coverage
build/coverage/main.o: src/main.cc include/physics/engine.h include/interface.h include/cli.h
	$(CXX) $(COV_FLAGS) -c -o $@ $< --coverage
build/coverage/interface.o: src/interface.cc include/interface.h include/physics/engine.h
	$(CXX) $(COV_FLAGS) -c -o $@ $< --coverage
build/coverage/cli.o: src/cli.cc include/cli.h
	$(CXX) $(COV_FLAGS) -c -o $@ $<
build/coverage/engine.o: src/physics/engine.cc include/physics/engine.h include/physics/collider.h include/physics/quaternion.h include/physics/octree.h include/cli.h
	$(CXX) $(COV_FLAGS) -c -o $@ $< --coverage
build/coverage/collider.o: src/physics/collider.cc include/physics/collider.h
	$(CXX) $(COV_FLAGS) -c -o $@ $< --coverage
build/coverage/quaternion.o: src/physics/quaternion.cc include/physics/quaternion.h
	$(CXX) $(COV_FLAGS) -c -o $@ $< --coverage
build/coverage/octree.o: src/physics/octree.cc include/physics/octree.h
	$(CXX) $(COV_FLAGS) -c -o $@ $< --coverage

exe: hummingbird
	__GL_SYNC_TO_VBLANK=0 ./hummingbird example.json
exe_test: test
	./test
exe_coverage: coverage
	./coverage
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory out
clean:
	rm -rf build/*.o
	rm -rf build/coverage/*.o
	rm -rf hummingbird
	rm -rf test
	rm -rf coverage
	rm -rf out/*
	rm -rf *.gcno
	rm -rf coverage.info
	rm -rf *.rec

.DEFAULT: hummingbird
.PHONY: exe exe_test exe_coverage clean
