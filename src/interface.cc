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
static bool resized = true, mouse_moved = false, first_mouse = true;

static float last_x = 0.0f, last_y = 0.0f, recent_x = 0.0f, recent_y = 0.0f;
static omp_lock_t mouse_lock;

static constexpr float golden_ratio = 1.6180339887f;
static constexpr float inv_golden_ratio = 1.0f / golden_ratio;
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
static constexpr unsigned int ICOSPHERE_ITERS = 2;

static constexpr unsigned int UNIFORM_POW_2_SIZE = 6;

static constexpr float MOVE_SPEED = 40.0f;
static constexpr float SENSITIVITY = 1.0f;

Graphics::Graphics(const Engine &engine_i): window(nullptr), engine(engine_i), identity(1.0f),
					    cup(0.0f, 1.0f, 0.0f), cx(0.0f), cy(0.0f), cz(0.0f), cphi(0.0f), ctheta(0.0f) {}

Graphics::~Graphics() {
  if (window) glfwDestroyWindow(window);
  glfwTerminate();
}

std::pair<unsigned int, unsigned int> Graphics::calc_icosphere_size(const unsigned int iters) const {
  if (iters == 0) return {12, 20};
  auto prev = calc_icosphere_size(iters  - 1);
  return {prev.first + prev.second * 3, prev.second * 4};
}

int Graphics::initialize() {
  omp_init_lock(&mouse_lock);

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
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback); 
  glViewport(0, 0, width, height);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glEnable(GL_CULL_FACE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
    return -1;
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
    return -1;
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
    return -1;
  }
  glUseProgram(shader_program);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  proj_view_loc = glGetUniformLocation(shader_program, "proj_view");
  model_loc = glGetUniformLocation(shader_program, "model");
  normal_loc = glGetUniformLocation(shader_program, "normal");

  auto icosphere_size = calc_icosphere_size(ICOSPHERE_ITERS);
  std::vector<float> icosphere_pts;
  std::vector<unsigned int> icosphere_tris;
  num_pts = icosphere_size.first;
  num_tris = icosphere_size.second;
  icosphere_pts.reserve(num_pts * 3);
  icosphere_pts.resize(12 * 3);
  memcpy(icosphere_pts.data(), icosphere_base_pts, 12 * 3 * sizeof(float));
  for (auto iter = icosphere_pts.begin(); iter != icosphere_pts.end(); ++iter) {
    *iter = *iter / sqrt(golden_ratio * golden_ratio + 1);
  }
  icosphere_tris.reserve(num_tris * 3);
  icosphere_tris.resize(20 * 3);
  memcpy(icosphere_tris.data(), icosphere_base_tris, 20 * 3 * sizeof(unsigned int));

  std::map<std::tuple<float, float, float>, unsigned int> already_inserted_pts;
  for (unsigned int i = 0; i < 12; ++i) {
    already_inserted_pts.insert({{icosphere_base_pts[i][0], icosphere_base_pts[i][1], icosphere_base_pts[i][2]}, i * 3});
  }
  for (unsigned int i = 0; i < ICOSPHERE_ITERS; ++i) {
    std::vector<unsigned int> new_tris;
    new_tris.reserve(num_tris * 3);

    unsigned int old_pts_end = static_cast<unsigned int>(icosphere_pts.size() / 3);
    for (unsigned int t = 0; t < icosphere_tris.size(); t += 3) {
      float x1 = (icosphere_pts.at(3 * icosphere_tris.at(t)) + icosphere_pts.at(3 * icosphere_tris.at(t + 1))) / 2.0f;
      float x2 = (icosphere_pts.at(3 * icosphere_tris.at(t + 1)) + icosphere_pts.at(3 * icosphere_tris.at(t + 2))) / 2.0f;
      float x3 = (icosphere_pts.at(3 * icosphere_tris.at(t + 2)) + icosphere_pts.at(3 * icosphere_tris.at(t))) / 2.0f;
      float y1 = (icosphere_pts.at(3 * icosphere_tris.at(t) + 1) + icosphere_pts.at(3 * icosphere_tris.at(t + 1) + 1)) / 2.0f;
      float y2 = (icosphere_pts.at(3 * icosphere_tris.at(t + 1) + 1) + icosphere_pts.at(3 * icosphere_tris.at(t + 2) + 1)) / 2.0f;
      float y3 = (icosphere_pts.at(3 * icosphere_tris.at(t + 2) + 1) + icosphere_pts.at(3 * icosphere_tris.at(t) + 1)) / 2.0f;
      float z1 = (icosphere_pts.at(3 * icosphere_tris.at(t) + 2) + icosphere_pts.at(3 * icosphere_tris.at(t + 1) + 2)) / 2.0f;
      float z2 = (icosphere_pts.at(3 * icosphere_tris.at(t + 1) + 2) + icosphere_pts.at(3 * icosphere_tris.at(t + 2) + 2)) / 2.0f;
      float z3 = (icosphere_pts.at(3 * icosphere_tris.at(t + 2) + 2) + icosphere_pts.at(3 * icosphere_tris.at(t) + 2)) / 2.0f;
      float m1 = 1.0f / sqrt(x1 * x1 + y1 * y1 + z1 * z1);
      float m2 = 1.0f / sqrt(x2 * x2 + y2 * y2 + z2 * z2);
      float m3 = 1.0f / sqrt(x3 * x3 + y3 * y3 + z3 * z3);
      x1 *= m1;
      y1 *= m1;
      z1 *= m1;
      x2 *= m2;
      y2 *= m2;
      z2 *= m2;
      x3 *= m3;
      y3 *= m3;
      z3 *= m3;

      if (!already_inserted_pts.count({x1, y1, z1})) {
	icosphere_pts.push_back(x1);
	icosphere_pts.push_back(y1);
	icosphere_pts.push_back(z1);
	already_inserted_pts.insert({{x1, y1, z1}, old_pts_end});
	++old_pts_end;
      }
      if (!already_inserted_pts.count({x2, y2, z2})) {
	icosphere_pts.push_back(x2);
	icosphere_pts.push_back(y2);
	icosphere_pts.push_back(z2);
	already_inserted_pts.insert({{x2, y2, z2}, old_pts_end});
	++old_pts_end;
      }
      if (!already_inserted_pts.count({x3, y3, z3})) {
	icosphere_pts.push_back(x3);
	icosphere_pts.push_back(y3);
	icosphere_pts.push_back(z3);
	already_inserted_pts.insert({{x3, y3, z3}, old_pts_end});
	++old_pts_end;
      }

      new_tris.push_back(icosphere_tris.at(t));
      new_tris.push_back(already_inserted_pts.at({x1, y1, z1}));
      new_tris.push_back(already_inserted_pts.at({x3, y3, z3}));
      new_tris.push_back(icosphere_tris.at(t + 1));
      new_tris.push_back(already_inserted_pts.at({x2, y2, z2}));
      new_tris.push_back(already_inserted_pts.at({x1, y1, z1}));
      new_tris.push_back(icosphere_tris.at(t + 2));
      new_tris.push_back(already_inserted_pts.at({x3, y3, z3}));
      new_tris.push_back(already_inserted_pts.at({x2, y2, z2}));
      new_tris.push_back(already_inserted_pts.at({x1, y1, z1}));
      new_tris.push_back(already_inserted_pts.at({x2, y2, z2}));
      new_tris.push_back(already_inserted_pts.at({x3, y3, z3}));
    }
    
    icosphere_tris = std::move(new_tris);
  }
  
  model_cache = new glm::mat4[engine.get_num_bodies() + (1 << UNIFORM_POW_2_SIZE)];
  normal_cache = new glm::mat4[engine.get_num_bodies() + (1 << UNIFORM_POW_2_SIZE)];

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  std::vector<float> icosphere_pts_with_norms;
  icosphere_pts_with_norms.reserve(icosphere_pts.size());
  for (std::size_t i = 0; i < icosphere_pts.size(); i += 3) {
      icosphere_pts_with_norms.push_back(icosphere_pts.at(i));
      icosphere_pts_with_norms.push_back(icosphere_pts.at(i+1));
      icosphere_pts_with_norms.push_back(icosphere_pts.at(i+2));
      icosphere_pts_with_norms.push_back(icosphere_pts.at(i));
      icosphere_pts_with_norms.push_back(icosphere_pts.at(i+1));
      icosphere_pts_with_norms.push_back(icosphere_pts.at(i+2));
  }

  glBufferData(GL_ARRAY_BUFFER, icosphere_size.first * 3 * sizeof(float), icosphere_pts_with_norms.data(), GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, icosphere_size.second * 3 * sizeof(unsigned int), icosphere_tris.data(), GL_STATIC_DRAW);

  return 0;
}

void Graphics::render_tick(const float dt) {
  handle_input(dt);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (resized) {
    proj = glm::perspective(glm::radians(80.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
  }
  glm::vec3 cdir = glm::vec3(cos(ctheta) * cos(cphi), sin(cphi), sin(ctheta) * cos(cphi));
  glm::vec3 cpos = glm::vec3(cx, cy, cz);
  glm::mat4 view = glm::lookAt(cpos, cpos + cdir, cup);
  glm::mat4 proj_view = proj * view;

#pragma omp parallel for
  for (std::size_t i = 0; i < engine.get_num_bodies(); ++i) {
    const Collider* coll = engine.get_colliders().at(i).get();
    if (const SphereCollider* sphere_coll = dynamic_cast<const SphereCollider*>(coll)) {
      float scale_factor = inv_golden_ratio * sphere_coll->radius;
      const auto& quat = engine.get_ang_pos().at(i);
      glm::mat4 model_rot = glm::mat4_cast(glm::quat(quat.w, quat.x, quat.y, quat.z));
      glm::mat4 model_pos = glm::translate(identity, glm::vec3(engine.get_pos().x.at(i), engine.get_pos().y.at(i), engine.get_pos().z.at(i)));
      model_cache[i] = model_pos * model_rot * scale_factor;
      normal_cache[i] = glm::inverse(model_cache[i]);
    }
  }

  unsigned int pow2 = 1 << UNIFORM_POW_2_SIZE;
  glUniformMatrix4fv(proj_view_loc, 1, GL_FALSE, glm::value_ptr(proj_view));
  std::size_t i;
  for (i = 0; i <= engine.get_num_bodies() - pow2; i += pow2) {
    glUniformMatrix4fv(model_loc, static_cast<int>(pow2), GL_FALSE, glm::value_ptr(model_cache[i]));
    glUniformMatrix4fv(normal_loc, static_cast<int>(pow2), GL_FALSE, glm::value_ptr(normal_cache[i]));
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<int>(num_tris * 3), GL_UNSIGNED_INT, 0, static_cast<int>(pow2));
  }
  for (; i < engine.get_num_bodies(); ++i) {
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model_cache[i]));
    glUniformMatrix4fv(normal_loc, 1, GL_FALSE, glm::value_ptr(normal_cache[i]));
    glDrawElements(GL_TRIANGLES, static_cast<int>(num_tris * 3), GL_UNSIGNED_INT, 0);
  }
  
  glfwSwapBuffers(window);
  resized = false;
}

void Graphics::handle_input(float dt) {
  glfwPollEvents();
  if (glfwGetKey(window, GLFW_KEY_W)) {
    cx += MOVE_SPEED * cos(ctheta) * dt;
    cz += MOVE_SPEED * sin(ctheta) * dt;
  }
  if (glfwGetKey(window, GLFW_KEY_A)) {
    cx += MOVE_SPEED * sin(ctheta) * dt;
    cz -= MOVE_SPEED * cos(ctheta) * dt;
  }
  if (glfwGetKey(window, GLFW_KEY_S)) {
    cx -= MOVE_SPEED * cos(ctheta) * dt;
    cz -= MOVE_SPEED * sin(ctheta) * dt;
  }
  if (glfwGetKey(window, GLFW_KEY_D)) {
    cx -= MOVE_SPEED * sin(ctheta) * dt;
    cz += MOVE_SPEED * cos(ctheta) * dt;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE)) {
    cy += MOVE_SPEED * dt;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
    cy -= MOVE_SPEED * dt;
  }

  omp_set_lock(&mouse_lock);
  if (mouse_moved && !first_mouse) {
    float offset_x = (recent_x - last_x) * SENSITIVITY * dt;
    float offset_y = (last_y - recent_y) * SENSITIVITY * dt;
    ctheta += offset_x;
    cphi += offset_y;
    if (cphi > 1.5f) cphi = 1.5f;
    else if (cphi < -1.5f) cphi = -1.5f;
    mouse_moved = false;
  }
  else if (mouse_moved) {
    first_mouse = false;
    mouse_moved = false;
  }
  omp_unset_lock(&mouse_lock);
}

bool Graphics::should_close() const {
  return !window || glfwWindowShouldClose(window);
}

void mouse_callback([[maybe_unused]] GLFWwindow* window, double x, double y) {
  omp_set_lock(&mouse_lock);
  last_x = recent_x;
  last_y = recent_y;
  recent_x = static_cast<float>(x);
  recent_y = static_cast<float>(y);
  mouse_moved = true;
  omp_unset_lock(&mouse_lock);
}

void resize_callback([[maybe_unused]] GLFWwindow* window, int new_width, int new_height) {
  width = new_width;
  height = new_height;
  resized = true;
  glViewport(0, 0, width, height);
}
