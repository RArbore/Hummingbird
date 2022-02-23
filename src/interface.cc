/*  This file is part of Hummingbird.
    Hummingbird is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
    Hummingbird is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Hummingbird. If not, see <https://www.gnu.org/licenses/>.  */

#define GL_GLEXT_PROTOTYPES

#include <interface.h>

static constexpr int DEFAULT_WIDTH = 800;
static constexpr int DEFAULT_HEIGHT = 600;

extern "C" char _binary_shaders_vertex_glsl_start;
extern "C" char _binary_shaders_fragment_glsl_start;

static int width = DEFAULT_WIDTH, height = DEFAULT_HEIGHT;
static bool resized = true;

static constexpr float golden_ratio = 1.6180339887f;
static constexpr float icosphere_base_pts[12][3] = {
  {-1.0f, golden_ratio, 0.0f},
  {1.0f, golden_ratio, 0.0f},
  {-1.0f, -golden_ratio, 0.0f},
  {1.0f, -golden_ratio, 0.0f},
  {0.0f, -1.0f, golden_ratio},
  {0.0f, 1.0f, golden_ratio},
  {0.0f, -1.0f, -golden_ratio},
  {0.0f, 1.0f, -golden_ratio},
  {golden_ratio, 0.0f, -1.0f},
  {golden_ratio, 0.0f, 1.0f},
  {-golden_ratio, 0.0f, -1.0f},
  {-golden_ratio, 0.0f, 1.0f},
};
static constexpr unsigned int icosphere_base_tris[20][3] = {
  {0, 11, 5},
  {0, 5, 1},
  {0, 1, 7},
  {0, 7, 10},
  {0, 10, 11},
  {1, 5, 9},
  {5, 11, 4},
  {11, 10, 2},
  {10, 7, 6},
  {7, 1, 8},
  {3, 9, 4},
  {3, 4, 2},
  {3, 2, 6},
  {3, 6, 8},
  {3, 8, 9},
  {4, 9, 5},
  {2, 4, 11},
  {6, 2, 10},
  {8, 6, 7},
  {9, 8, 1},
};

Graphics::Graphics(const Engine &engine): window(nullptr), engine(engine) {}

Graphics::~Graphics() {
  if (window) glfwDestroyWindow(window);
  glfwTerminate();
}

int Graphics::initialize() {
  if (!glfwInit()) {
    std::cerr << "ERROR: Couldn't initialize GLFW. Aborting." << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  window = glfwCreateWindow(width, height, "Hummingbird", NULL, NULL);
  if (!window) {
    std::cerr << "ERROR: Couldn't create window. Aborting." << std::endl;
    return -1;
  }

  glfwShowWindow(window);
  glfwMakeContextCurrent(window);
  glfwGetFramebufferSize(window, &width, &height);
  glfwSetFramebufferSizeCallback(window, resize_callback);
  glViewport(0, 0, width, height);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glEnable(GL_CULL_FACE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  const char* const vertex_shader_text = &_binary_shaders_vertex_glsl_start;
  const char* const fragment_shader_text = &_binary_shaders_fragment_glsl_start;
  int shader_comp_success = 0;

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, nullptr);
  glCompileShader(vertex_shader);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_comp_success);
  if (!shader_comp_success) {
    char* const error_log = new char[1024];
    glGetShaderInfoLog(vertex_shader, 1024, nullptr, error_log);
    std::cerr << "ERROR: Couldn't compile the vertex shader. Here's the GL error log:" << std::endl;
    std::cerr << error_log << std::endl;
  }

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, nullptr);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_comp_success);
  if (!shader_comp_success) {
    char* const error_log = new char[1024];
    glGetShaderInfoLog(fragment_shader, 1024, nullptr, error_log);
    std::cerr << "ERROR: Couldn't compile the fragment shader. Here's the GL error log:" << std::endl;
    std::cerr << error_log << std::endl;
  }

  shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  glGetShaderiv(shader_program, GL_LINK_STATUS, &shader_comp_success);
  if (!shader_comp_success) {
    char* const error_log = new char[1024];
    glGetShaderInfoLog(shader_program, 1024, nullptr, error_log);
    std::cerr << "ERROR: Couldn't link the complete shader program. Here's the GL error log:" << std::endl;
    std::cerr << error_log << std::endl;
  }
  glUseProgram(shader_program);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  glBufferData(GL_ARRAY_BUFFER, 12 * 3 * sizeof(float), icosphere_base_pts, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 20 * 3 * sizeof(unsigned int), icosphere_base_tris, GL_STATIC_DRAW);

  return 0;
}

void Graphics::render_tick() {
  glfwPollEvents();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (resized) {
    proj = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
  }
  for (std::size_t i = 0; i < engine.get_num_bodies(); ++i) {
    const auto& quat = engine.get_ang_pos().at(i);
    glm::mat4 model_rot = glm::mat4_cast(glm::quat(quat.w, quat.x, quat.y, quat.z));
    glm::mat4 model_pos = glm::translate(glm::mat4(1.0f),
					 glm::vec3(engine.get_pos().x.at(i), engine.get_pos().y.at(i), engine.get_pos().z.at(i)));
    glm::mat4 view = glm::translate(glm::mat4(1.0f),
				    glm::vec3(0.0f, 0.0f, -10.0f));
    glm::mat4 transform = proj * view * model_pos * model_rot;
    int transform_loc = glGetUniformLocation(shader_program, "transform");
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(transform));
    glDrawElements(GL_TRIANGLES, 20 * 3, GL_UNSIGNED_INT, 0);
  }

  glfwSwapBuffers(window);
  resized = false;
}

bool Graphics::should_close() const {
  return !window || glfwWindowShouldClose(window);
}

void resize_callback([[maybe_unused]] GLFWwindow* window, int new_width, int new_height) {
  width = new_width;
  height = new_height;
  resized = true;
  glViewport(0, 0, width, height);
}
