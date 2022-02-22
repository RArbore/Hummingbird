CXX=g++
LD=LD_LIBRARY_PATH=/usr/lib64 g++

CXX_FLAGS=-std=c++17 -fopenmp -Iinclude

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

test: build/cli.o build/tests.o
	$(LD) $(L_FLAGS) -o $@ $^
build/tests.o: tests/cli_tests.cc
	$(CXX) $(CXX_FLAGS) -c $^ -o $@

exe: hummingbird
	./hummingbird example.json
exe_test: test
	./test
clean:
	rm build/*.o
	rm hummingbird

.DEFAULT: hummingbird
.PHONY: exe exe_test clean
