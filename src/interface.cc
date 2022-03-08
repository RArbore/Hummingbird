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

/*
 * So that we can write our shaders in normal
 * text files without having to load them at
 * runtime via the filesystem, we compile our
 * shaders into object code containing just text.
 * Then, we link those objects with our code
 * and get the symbols representing the first
 * characters below. We will take the address of
 * these chars later to access the full shader
 * texts.
 */
extern "C" char _binary_shaders_vertex_glsl_start;
extern "C" char _binary_shaders_fragment_glsl_start;

/*
 * Unfortunately, we have to store global state
 * to deal with user input, as GLFW callbacks
 * cannot access member functions / data.
 * Consequently, our callbacks will update this
 * global state, which will then be seen by the
 * graphics context in handle_input.
 */
static int width = DEFAULT_WIDTH, height = DEFAULT_HEIGHT;
static bool resized = true, mouse_moved = false, first_mouse = true;
static float last_x = 0.0f, last_y = 0.0f, recent_x = 0.0f, recent_y = 0.0f;

/*
 * Constant for the icosphere calculations.
 * Stores an icosahedron for which we will
 * subdivide and project new triangles to
 * create a sphere mesh.
 */
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

/*
 * Some constants. ICOSPHERE_ITERS defines how refined
 * our sphere mesh will be. UNIFORM_SIZE defines how
 * many uniform model and normal matrices we can pass
 * to our shaders at once for instanced rendering.
 * MOVE_SPEED is how fast the camera moves in space.
 * SENSISITIVITY is how fast the camera turns in
 * response to mouse movement.
 */
static constexpr unsigned int ICOSPHERE_ITERS = 2;
static constexpr unsigned int UNIFORM_SIZE = 120;
static constexpr float MOVE_SPEED = 40.0f;
static constexpr float SENSITIVITY = 1.0f;

/*
 * Graphics constructor. Dead simple since actual
 * initialization happens in initialize.
 */
Graphics::Graphics(const Engine &engine_i): window(nullptr), engine(engine_i), identity(1.0f),
					    cup(0.0f, 1.0f, 0.0f), cx(0.0f), cy(0.0f), cz(0.0f), cphi(0.0f), ctheta(0.0f) {}

Graphics::~Graphics() {
  glfwDestroyWindow(window);
  glfwTerminate();
  delete[] model_cache;
  delete[] normal_cache;
}

/*
 * Helper function for determining how large
 * icosphere refinements are. The return type
 * represents the number of vertices needed
 * and the number of triangles needed, in that
 * order.
 */
std::pair<unsigned int, unsigned int> Graphics::calc_icosphere_size(const unsigned int iters) const {
  if (iters == 0) return {12, 20};
  auto prev = calc_icosphere_size(iters  - 1);
  return {prev.first + prev.second * 3, prev.second * 4};
}

/*
 * Our initialization function.
 */
int Graphics::initialize() {
  /*
   * Initialize GLFW and some GL stuff
   */
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
  //glEnable(GL_CULL_FACE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /*
   * Load in shaders from linked object files,
   * and compile them.
   */
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
    delete[] error_log;
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
    delete[] error_log;
    return -1;
  }

  /*
   * Link compiled shaders into a shader program.
   */
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
    delete[] error_log;
    return -1;
  }
  glUseProgram(shader_program);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  proj_view_loc = glGetUniformLocation(shader_program, "proj_view");
  model_loc = glGetUniformLocation(shader_program, "model");
  normal_loc = glGetUniformLocation(shader_program, "normal");
  
  /*
   * Allocate our caches and create our OpenGL buffers
   * and arrays. These hold information about the various
   * meshes we use in our simulation. Each VAO / VBO / 
   * optionally EBO combo represents a single mesh.
   * Initialize our meshes.
   */
  model_cache = new glm::mat4[engine.get_num_bodies() + UNIFORM_SIZE];
  normal_cache = new glm::mat4[engine.get_num_bodies() + UNIFORM_SIZE];

  initialize_sphere_mesh();
  initialize_walls_mesh();

  return 0;
}

/*
 * Calculate refined icosphere. For each iteration,
 * we add points at the midpoints of each triangle
 * in our previous icosphere. Then, we create
 * 4 triangles where each triangle in the previous
 * icosphere was. We also project the new vertices 
 * onto the unit sphere. During this process, to
 * inserting points multiple times, we keep a map
 * of already inserted point, where each point's
 * value is its index in the points array.
 */
std::pair<std::vector<float>, std::vector<unsigned int>> Graphics::create_icosphere_mesh() {
  /*
   * Initialize our vectors with the 0 iteration
   * icosphere (a.k.a. an icosahedron).
   */
  auto icosphere_size = calc_icosphere_size(ICOSPHERE_ITERS);
  std::vector<float> icosphere_pts(&icosphere_base_pts[0][0], &icosphere_base_pts[0][0] + 12 * 3);
  std::vector<unsigned int> icosphere_tris(&icosphere_base_tris[0][0], &icosphere_base_tris[0][0] + 20 * 3);
  num_pts = icosphere_size.first;
  num_tris = icosphere_size.second;
  /*
   * Normalize to radius 1.
   */
  for (auto iter = icosphere_pts.begin(); iter != icosphere_pts.end(); ++iter) {
    *iter = *iter / sqrt(golden_ratio * golden_ratio + 1);
  }

  /*
   * Avoid inserting points twice - also,
   * we need to lookup point positions to
   * create triangles for old vertices.
   */
  std::map<std::tuple<float, float, float>, unsigned int> already_inserted_pts;
  for (unsigned int i = 0; i < 12; ++i) {
    already_inserted_pts.insert({{icosphere_base_pts[i][0], icosphere_base_pts[i][1], icosphere_base_pts[i][2]}, i * 3});
  }
  for (unsigned int i = 0; i < ICOSPHERE_ITERS; ++i) {
    std::vector<unsigned int> new_tris;
    new_tris.reserve(num_tris * 3);

    unsigned int old_pts_end = static_cast<unsigned int>(icosphere_pts.size() / 3);
    for (unsigned int t = 0; t < icosphere_tris.size(); t += 3) {
      /*
       * Calculate new points.
       */
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

      /*
       * Insert points only if not already in map.
       */
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

      /*
       * Add triangles.
       */
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
  return std::make_pair(icosphere_pts, icosphere_tris);
}

/*
 * Initialize the GL objects for our meshes.
 */
void Graphics::initialize_sphere_mesh() {
  /*
   * Calculate the mesh for the icosphere.
   */
  auto icosphere = create_icosphere_mesh();
  auto& icosphere_pts = icosphere.first;
  auto& icosphere_tris = icosphere.second;

  /*
   * On an icosphere, each point is its own normal.
   */
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

  glGenVertexArrays(1, &sphereVAO);
  glBindVertexArray(sphereVAO);

  glGenBuffers(1, &sphereVBO);
  glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);

  glGenBuffers(1, &sphereEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBufferData(GL_ARRAY_BUFFER, static_cast<long int>(icosphere_pts_with_norms.size() * sizeof(float)), icosphere_pts_with_norms.data(), GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long int>(icosphere_tris.size() * sizeof(unsigned int)), icosphere_tris.data(), GL_STATIC_DRAW);
}

void Graphics::initialize_walls_mesh() {
  auto boundary = engine.get_boundary();
  float inv_sqrt_3 = 1.0f / sqrt(3.0f);
  
  float vertices[16][3] = {
    {boundary[0], boundary[2], boundary[4]},
    {-inv_sqrt_3, -inv_sqrt_3, -inv_sqrt_3},
    {boundary[1], boundary[2], boundary[4]},
    {inv_sqrt_3, -inv_sqrt_3, -inv_sqrt_3},
    {boundary[0], boundary[3], boundary[4]},
    {-inv_sqrt_3, inv_sqrt_3, -inv_sqrt_3},
    {boundary[1], boundary[3], boundary[4]},
    {inv_sqrt_3, inv_sqrt_3, -inv_sqrt_3},
    {boundary[0], boundary[2], boundary[5]},
    {-inv_sqrt_3, -inv_sqrt_3, inv_sqrt_3},
    {boundary[1], boundary[2], boundary[5]},
    {inv_sqrt_3, -inv_sqrt_3, inv_sqrt_3},
    {boundary[0], boundary[3], boundary[5]},
    {-inv_sqrt_3, inv_sqrt_3, inv_sqrt_3},
    {boundary[1], boundary[3], boundary[5]},
    {inv_sqrt_3, inv_sqrt_3, inv_sqrt_3},
  };

  unsigned int quads[6][4] = {
    {0, 1, 3, 2},
    {4, 5, 7, 6},
    {0, 1, 5, 4},
    {2, 3, 7, 6},
    {0, 2, 6, 4},
    {1, 3, 7, 5},
  };
  
  glGenVertexArrays(1, &wallsVAO);
  glBindVertexArray(wallsVAO);

  glGenBuffers(1, &wallsVBO);
  glBindBuffer(GL_ARRAY_BUFFER, wallsVBO);

  glGenBuffers(1, &wallsEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallsEBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quads), quads, GL_STATIC_DRAW);
}

/*
 * Render tick function that occurs every frame. First,
 * we handle user input. Then, we clear the screen buffer.
 * Next, we calculate various matrices / vectors related
 * to camera projection. Then, we, in parallel, calculate
 * the model and normal matrices for every body in our
 * scene. These matrices modify the position and normals
 * of our bodies, respectively. Next, we perform instanced
 * rendering of our bodies to minimize OpenGL API calls.
 * Finally, we swap buffers.
 */
void Graphics::render_tick(const float dt) {
  handle_input(dt);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (resized) {
    proj = glm::perspective(glm::radians(80.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
  }
  const glm::vec3 cdir = glm::vec3(cos(ctheta) * cos(cphi), sin(cphi), sin(ctheta) * cos(cphi));
  const glm::vec3 cpos = glm::vec3(cx, cy, cz);
  const glm::mat4 view = glm::lookAt(cpos, cpos + cdir, cup);
  const glm::mat4 proj_view = proj * view;
  glUniformMatrix4fv(proj_view_loc, 1, GL_FALSE, glm::value_ptr(proj_view));

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glBindVertexArray(wallsVAO);
  
  model_cache[0] = identity;
  normal_cache[0] = identity;
  glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model_cache[0]));
  glUniformMatrix4fv(normal_loc, 1, GL_FALSE, glm::value_ptr(normal_cache[0]));
  glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, 0);

#pragma omp parallel for
  for (std::size_t i = 0; i < engine.get_num_bodies(); ++i) {
    const Collider* coll = engine.get_colliders()[i].get();
    if (const SphereCollider* sphere_coll = dynamic_cast<const SphereCollider*>(coll)) {
      const float scale_factor = sphere_coll->radius;
      const auto& quat = engine.get_ang_pos()[i];
      const glm::mat4 model_rot = glm::mat4_cast(glm::quat(quat.w, quat.x, quat.y, quat.z));
      const glm::mat4 model_pos = glm::translate(identity, glm::vec3(engine.get_pos().x[i], engine.get_pos().y[i], engine.get_pos().z[i]));
      const glm::mat4 model_scale = glm::scale(identity, glm::vec3(scale_factor, scale_factor, scale_factor));
      model_cache[i] = model_pos * model_rot * model_scale;
      normal_cache[i] = glm::inverse(model_cache[i]);
    }
  }

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBindVertexArray(sphereVAO);

  std::size_t i = 0;
  if (engine.get_num_bodies() >= UNIFORM_SIZE) {
    for (; i <= engine.get_num_bodies() - UNIFORM_SIZE; i += UNIFORM_SIZE) {
      glUniformMatrix4fv(model_loc, static_cast<int>(UNIFORM_SIZE), GL_FALSE, glm::value_ptr(model_cache[i]));
      glUniformMatrix4fv(normal_loc, static_cast<int>(UNIFORM_SIZE), GL_FALSE, glm::value_ptr(normal_cache[i]));
      glDrawElementsInstanced(GL_TRIANGLES, static_cast<int>(num_tris * 3), GL_UNSIGNED_INT, 0, static_cast<int>(UNIFORM_SIZE));
    }
  }
  for (; i < engine.get_num_bodies(); ++i) {
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model_cache[i]));
    glUniformMatrix4fv(normal_loc, 1, GL_FALSE, glm::value_ptr(normal_cache[i]));
    glDrawElements(GL_TRIANGLES, static_cast<int>(num_tris * 3), GL_UNSIGNED_INT, 0);
  }

  glfwSwapBuffers(window);
  resized = false;
}

/*
 * Handle user input. For keyboard input, we move
 * the camera. For mouse input, we turn the camera.
 * This is where we check our global state 
 * periodically in order to respond to mouse input.
 */
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

  if (mouse_moved && !first_mouse) {
    const float offset_x = (recent_x - last_x) * SENSITIVITY * dt;
    const float offset_y = (last_y - recent_y) * SENSITIVITY * dt;
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
}

bool Graphics::should_close() const {
  return !window || glfwWindowShouldClose(window);
}

void mouse_callback([[maybe_unused]] GLFWwindow* window, double x, double y) {
  last_x = recent_x;
  last_y = recent_y;
  recent_x = static_cast<float>(x);
  recent_y = static_cast<float>(y);
  mouse_moved = true;
}

void resize_callback([[maybe_unused]] GLFWwindow* window, int new_width, int new_height) {
  width = new_width;
  height = new_height;
  resized = true;
  glViewport(0, 0, width, height);
}
