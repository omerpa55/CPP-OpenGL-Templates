#include <cmath>
#include <cstddef>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <string>
#include <tinygltf/tiny_gltf.h>
#include <tinygltf/json.hpp>
//#include <vector>
#include <iostream>
//#define STB_IMAGE_IMPLEMENTATION
#include <tinygltf/stb_image.h>

int width, height, nrChannels;

unsigned int programId;

const char *vssource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0f);
  TexCoord = aTexCoord;
}
)";

const char *fssource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform bool useTexture;
uniform vec3 uColor;

void main() {
  if (useTexture) {
    FragColor = texture(ourTexture, TexCoord);
  } else {
    FragColor = vec4(uColor, 1.0f);
  }
}
)";

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 300;
float lastY = 300;
float sensivity = 0.1f;

glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

float speed = 0.05f;

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  xoffset *= sensivity;
  yoffset *= sensivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  glm::vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(front);
}

//X-MIN : -0.2f;
//X-MAX : 0.2f;
//Y-MIN : -0.2f;
//Y-MAX : 0.2f;
//Z-MIN : -0.5f;
//Z-MAX : 0.0f;

glm::vec3 kareMax(0.3f, 0.3f, 0.1f);
glm::vec3 kareMin(-0.3f, -0.3f, -0.6f);

float kare[] {
  -0.2f, -0.2f, 0.0f, 0.0f, 0.0f,
  0.2f, -0.2f, 0.0f, 1.0f, 0.0f,
  -0.2f, 0.2f, 0.0f, 0.0f, 1.0f,

  0.2f, -0.2f, 0.0f, 1.0f, 0.0f,
  0.2f, 0.2f, 0.0f, 1.0f, 1.0f,
  -0.2f, 0.2f, 0.0f, 0.0f, 1.0f,

  -0.2f, -0.2f, -0.5f, 0.0f, 0.0f,
  0.2f, -0.2f, -0.5f, 1.0f, 0.0f,
  -0.2f, 0.2f, -0.5f, 0.0f, 1.0f,

  0.2f, -0.2f, -0.5f, 1.0f, 0.0f,
  0.2f, 0.2f, -0.5f, 1.0f, 1.0f,
  -0.2f, 0.2f, -0.5f, 0.0f, 1.0f,


  //    X          Y           Z           U          V
  -0.2f, -0.2f, -0.5f, 0.0f, 0.0f,
  -0.2f, 0.2f, -0.5f, 0.0f, 1.0f, //Sol yüz 1
  -0.2f, -0.2f, 0.0f, 1.0f, 0.0f,

  -0.2f, -0.2f, 0.0f, 1.0f, 0.0f,
  -0.2f, 0.2f, -0.5f, 0.0f, 1.0f, //Sol yüz 2
  -0.2f, 0.2f, 0.0f, 1.0f, 1.0f,

  -0.2f, 0.2f, -0.5f, 0.0f, 0.0f,
  0.2f, 0.2f, -0.5f, 1.0f, 0.0f, //Üst yüz 1
  -0.2f, 0.2f, 0.0f, 0.0f, 1.0f,

  0.2f, 0.2f, -0.5f, 1.0f, 0.0f,
  -0.2f, 0.2f, 0.0f, 0.0f, 1.0f, //Üst yüz 2
  0.2f, 0.2f, 0.0f, 1.0f, 1.0f,

  0.2f, 0.2f, -0.5f, 0.0f, 1.0f,
  0.2f, 0.2f, 0.0f, 1.0f, 1.0f, // Sağ yüz 1
  0.2f, -0.2f, -0.5f, 0.0f, 0.0f,

  0.2f, 0.2f, 0.0f, 1.0f, 1.0f,
  0.2f, -0.2f, -0.5f, 0.0f, 0.0f, //Sağ yüz 2
  0.2f, -0.2f, 0.0f, 1.0f, 0.0f
};

float plane[] = {
  -2.0f, -0.9f, -2.0f,
  2.0f, -0.9f, -2.0f,
  2.0f, -0.9f, 2.0f,

  -2.0f, -0.9f, 2.0f,
  2.0f, -0.9f, 2.0f,
  -2.0f, -0.9f, -2.0f
};

float ucgen[] = {
  -0.2f, -0.2f, 0.0f,
  0.2f, -0.2f, 0.0f,
  0.0f, 0.2f, 0.0f
};

bool checkCollision(glm::vec3 camPos, glm::vec3 min, glm::vec3 max) {
  return (camPos.x > min.x && camPos.x < max.x)&&
    (camPos.y > min.y && camPos.y < max.y) &&
    (camPos.z > min.z && camPos.z < max.z);
}

int main() {
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *window = glfwCreateWindow(600, 600, "Pencere", NULL, NULL);

  glfwMakeContextCurrent(window);

  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
/*
  tinygltf::Model model;
  tinygltf::TinyGLTF loader;
  std::string err, warn;

  bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, "low-poly_door.glb");
  if (!err.empty()) {
    std::cout << "Err: " << err;
    return 0;
  }
  if (!warn.empty()) {
    std::cout << "Warn: " << warn;
    return 0;
  }
  if (!ret) {
    std::cout << "Failed to load GLB";
    return 0;
  }

  std::vector<glm::vec3> vertices;
  size_t numOfVertex;
  GLenum mode = GL_TRIANGLES;

  const tinygltf::Mesh& mesh = model.meshes[0];
  for (const auto& primitive : mesh.primitives) {
    const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
    const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
    const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];

    const float *positions = reinterpret_cast<const float*>(
      &posBuffer.data[posView.byteOffset + posAccessor.byteOffset]
    );

    for (size_t i = 0; i < posAccessor.count; i++) {
      glm::vec3 v(
        positions[i * 3 + 0],
        positions[i * 3 + 1],
        positions[i * 3 + 2]
      );
      vertices.push_back(v);
    }

    numOfVertex = posAccessor.count;

    if (primitive.mode == TINYGLTF_MODE_TRIANGLE_STRIP) mode = GL_TRIANGLE_STRIP;
    else if (primitive.mode == TINYGLTF_MODE_LINE) mode = GL_LINE;
  } */
  unsigned char *data = stbi_load("grass.jpeg", &width, &height, &nrChannels, 0);


  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }

  glEnable(GL_DEPTH_TEST);

  programId = glCreateProgram();

  unsigned int vertexSh = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexSh, 1, &vssource, NULL);
  glCompileShader(vertexSh);

  unsigned int fragmentSh = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentSh, 1, &fssource, NULL);
  glCompileShader(fragmentSh);

  glAttachShader(programId, vertexSh);
  glAttachShader(programId, fragmentSh);

  glLinkProgram(programId);

  //kare

  GLuint kareVBO, kareVAO;
  glGenVertexArrays(1, &kareVAO);
  glGenBuffers(1, &kareVBO);

  glBindVertexArray(kareVAO);
  glBindBuffer(GL_ARRAY_BUFFER, kareVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kare), kare, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
  glEnableVertexAttribArray(0);

  const void* offset = (void*)(3 * sizeof(float));

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, offset);
  glEnableVertexAttribArray(1);

  //kare Texture
  GLuint karetexture;
  glGenTextures(1, &karetexture);
  glBindTexture(GL_TEXTURE_2D, karetexture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (data) {
    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    if (format == GL_RGB) {
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cerr << "ERROR REPORTED";
  }

  stbi_image_free(data);

  //ucgen

  GLuint ucgenVBO, ucgenVAO;
  glGenVertexArrays(1, &ucgenVAO);
  glGenBuffers(1, &ucgenVBO);

  glBindVertexArray(ucgenVAO);
  glBindBuffer(GL_ARRAY_BUFFER, ucgenVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ucgen), ucgen, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
  glEnableVertexAttribArray(0);

  //plane

  GLuint planeVBO, planeVAO;
  glGenVertexArrays(1, &planeVAO);
  glGenBuffers(1, &planeVBO);

  glBindVertexArray(planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  /*

  GLuint doorVBO, doorVAO;
  glGenVertexArrays(1, &doorVAO);
  glGenBuffers(1, &doorVBO);

  glBindVertexArray(doorVAO);
  glBindBuffer(GL_ARRAY_BUFFER, doorVBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
  glEnableVertexAttribArray(0);

  */

  glm::vec3 move1(0.0f, 0.0f, 0.0f);

  while(!glfwWindowShouldClose(window)) {
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      cameraPos += speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      cameraPos -= speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;

    glUseProgram(programId);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(move1.x, move1.y, move1.z));

    glm::mat4 view = glm::lookAt(
      cameraPos, cameraPos + cameraFront, cameraUp
    );

    glm::mat4 projection = glm::perspective(
      glm::radians(60.0f),
      600.0f / 600.0f,
      0.1f,
      100.0f
    );

    if (checkCollision(cameraPos, kareMin, kareMax)) {
      cameraPos -= cameraFront * speed;
    }

    glUniformMatrix4fv(glGetUniformLocation(programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(programId, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
/*  
    glBindVertexArray(ucgenVAO);

    glUniform3f(glGetUniformLocation(programId, "uColor"), 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    */

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, karetexture);

    glUniform1i(glGetUniformLocation(programId, "useTexture"), GL_TRUE);
    glUniform3f(glGetUniformLocation(programId, "uColor"), 0.0f, 0.0f, 0.0f);
    glUniform1i(glGetUniformLocation(programId, "ourTexture"), 0);

    glBindVertexArray(kareVAO);


    //model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 0.5f, 1.0f));

    //glUniformMatrix4fv(glGetUniformLocation(programId, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glDrawArrays(GL_TRIANGLES, 0, 30);

    /*

    glBindVertexArray(doorVAO);

    glUniform3f(glGetUniformLocation(programId, "uColor"), 1.0f, 0.3f, 0.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));
    glDrawArrays(mode, 0, numOfVertex);

    */

    glBindVertexArray(planeVAO);

    glUniform1i(glGetUniformLocation(programId, "useTexture"), GL_FALSE);
    glUniform3f(glGetUniformLocation(programId, "uColor"), 1.0f, 1.0f, 0.0f);
    glUniform1i(glGetUniformLocation(programId, "ourTexture"), 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);

    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}