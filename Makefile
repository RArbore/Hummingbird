CXX=g++
LD=g++

CXX_FLAGS=-std=c++20 -fopenmp

L_FLAGS=-lGL -lglfw -ljsoncpp

hummingbird: build/main.o
	$(LD) $(L_FLAGS) -o $@ $^
build/main.o: src/main.cc
	$(CXX) $(CXX_FLAGS) -c -o $@ $^
clean:
	rm build/*.o
	rm hummingbird

.DEFAULT: hummingbird
.PHONY: clean
