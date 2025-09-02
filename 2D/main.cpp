#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>

unsigned int programId;

const char* vssource =
R"(#version 330 core
layout (location = 0) in vec3 aPos;
uniform vec3 uMove;
void main()
{
	gl_Position = vec4(aPos + uMove, 1.0f);
})";

const char* fssource =
R"(#version 330 core
out vec4 FragColor;
void main()
{
	FragColor = vec4(1.0, 0.0f, 0.0f, 1.0f);
})";

float length = 0.08f;

float vertices[] = {
	-length / 2, length / 2, 0.0f,
	-length / 2, -length / 2, 0.0f,
	length / 2, -length / 2, 0.0f,

	-length / 2, length / 2, 0.0f,
	length / 2, -length / 2, 0.0f,
	length / 2, length / 2, 0.0f
};

glm::vec3 move(0.0f, 0.0f, 0.0f);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (glfwGetKey(window, GLFW_KEY_W)) {
			move += glm::vec3(0.0f, length, 0.0f);
		}if (glfwGetKey(window, GLFW_KEY_S)) {
			move += glm::vec3(0.0f, -length, 0.0f);
		}if (glfwGetKey(window, GLFW_KEY_A)) {
			move += glm::vec3(-length, 0.0f, 0.0f);
		}if (glfwGetKey(window, GLFW_KEY_D)) {
			move += glm::vec3(length, 0.0f, 0.0f);
		}
	}
}



int main() {
	glfwInit();

	if (!glfwInit) {
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "Pencere", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		glfwTerminate();
		return -1;
	}

	programId = glCreateProgram();

	unsigned int vsId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsId, 1, &vssource, NULL);
	glCompileShader(vsId);

	unsigned int fsId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsId, 1, &fssource, NULL);
	glCompileShader(fsId);

	glAttachShader(programId, vsId);
	glAttachShader(programId, fsId);

	glLinkProgram(programId);


	unsigned int VBO;
	unsigned int VAO;

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindVertexArray(VAO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glEnableVertexAttribArray(0);


	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO);
		glEnableVertexAttribArray(0);

		glUseProgram(programId);

		glDrawArrays(GL_TRIANGLES, 0, 6);


		GLuint movelocation = glGetUniformLocation(programId, "uMove");

		glUniform3fv(movelocation, 1, glm::value_ptr(move));

		glfwSwapBuffers(window);

		glfwPollEvents();
	}
}
