#include "playback.h"

void save_to_file(std::ofstream &stream, size_t num, Engine::Vec3x<float, 32> position){
  for (size_t i = 0; i < num; i++){
    float pos_x = position.x[i];
    float pos_y = position.y[i];
    float pos_z = position.z[i];
    stream.write(reinterpret_cast<char*> (&pos_x), sizeof(pos_x));
    stream.write(reinterpret_cast<char*> (&pos_y), sizeof(pos_y));
    stream.write(reinterpret_cast<char*> (&pos_z), sizeof(pos_z));
  }
}

Engine::Vec3x<float, 32> read_from_file(std::ifstream &stream){
    Engine::Vec3x<float, 32> result;
    while (!stream.eof()){
      float pos_x = 0;
      float pos_y = 0;
      float pos_z = 0;
      stream.read(reinterpret_cast<char*> (&pos_x), sizeof(pos_x));
      stream.read(reinterpret_cast<char*> (&pos_y), sizeof(pos_y));
      stream.read(reinterpret_cast<char*> (&pos_z), sizeof(pos_z));
      result.x.push_back(pos_x);
      result.y.push_back(pos_y);
      result.z.push_back(pos_z);
    }
    return result;
  }