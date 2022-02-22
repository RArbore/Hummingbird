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
LD=LD_LIBRARY_PATH=/usr/lib64 g++

CXX_FLAGS=-std=c++17 -Wall -fopenmp -Iinclude

L_FLAGS=-L/usr/lib64 -lglfw -lGL -ljsoncpp

hummingbird: build/main.o build/interface.o build/cli.o build/engine.o build/collider.o
	$(LD) $(L_FLAGS) -o $@ $^
build/main.o: src/main.cc
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
build/interface.o: src/interface.cc include/interface.h
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
build/cli.o: src/cli.cc include/cli.h
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
build/engine.o: src/physics/engine.cc include/physics/engine.h include/physics/collider.h include/cli.h
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
build/collider.o: src/physics/collider.cc include/physics/collider.h
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
test: hummingbird
	./hummingbird example.json
clean:
	rm build/*.o
	rm hummingbird

.DEFAULT: hummingbird
.PHONY: clean test
