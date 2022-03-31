# Hummingbird
Hummingbird is a simple rigid body physics engine.

## Dependencies
To build the engine, you will need:
- g++ (with OpenMP support)
- GNU Make
- OpenGL
- GLFW
- JsonCpp
- Boost

Additionally, Hummingbird uses various AVX instructions to improve performance. As a result, you will need to have a modern x86-64 machine to run Hummingbird.

Currently, Hummingbird can be built for Linux. It has not been tested on MacOS, BSDs, or Windows.

## Build
To build Hummingbird, run the following:
```
make
```

## Usage
To run Hummingbird on a configuration file, run the following:
```
./hummingbird <json_file>
```

You may use the following flags:
* -h: prints help info
* -r: record the simulation for playback
* -p: playback a simulation

Here are some example usages:
```
./hummingbird <json_file>      # runs Hummingbird on the provided json
./hummingbird -r <json_file>   # runs Hummingbird on the provided json, and records the simulation (will write to a file called <json_file>.rec)
./hummingbird -p <recording>   # plays back a recording file
./hummingbird -h               # prints help info
```

We have provided an example json file. To run Hummingbird with it, you can run:
```
make exe
```

To build and run the test suite, run the following:
```
make exe_test
```
