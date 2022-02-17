CXX=g++
LD=LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu g++

CXX_FLAGS=-std=c++20 -fopenmp

L_FLAGS=-L/usr/lib/x86_64-linux-gnu -lGL -lglfw -ljsoncpp

hummingbird: build/main.o
	$(LD) $(L_FLAGS) -o $@ $^
build/main.o: src/main.cc
	$(CXX) $(CXX_FLAGS) -c -o $@ $^
test:
	./hummingbird
clean:
	rm build/*.o
	rm hummingbird

.DEFAULT: hummingbird
.PHONY: clean test
