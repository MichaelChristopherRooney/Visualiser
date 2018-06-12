#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "globals.hpp"

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include <glm/gtx/transform.hpp>

using namespace glm;

GLint init_shaders();
bool loadOBJ(const char * path, std::vector<glm::vec3> & out_vertices, std::vector<glm::vec3> & out_normals);
int init_parser(char *fp);

static GLFWwindow* window;
std::vector<glm::vec3> verts;
std::vector<glm::vec3> normals;
static glm::mat4 cam;
static glm::mat4 proj;
static GLint sphere_shader_prog;
static GLuint mvp_unif_id;

// Sets up glfw, glew and OpenGL
static int init_gl() {
	if (!glfwInit()) {
		printf("Failed to initialize GLFW\n");
		return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 1);
	window = glfwCreateWindow(1600, 900, "Visualiser", NULL, NULL);
	if (window == NULL) {
		printf("Failed to open GLFW window.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) {
		printf("Failed to initialize GLEW\n");
		glfwTerminate();
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	return 0;
}

// Sets up the window and shaders needed.
// Also reads initial state from the binary file
static int init() {
	if (init_parser("data.bin") == -1) {
		return -1;
	}
	if (init_gl() == -1) {
		return -1;
	}
	sphere_shader_prog = init_shaders();
	if (sphere_shader_prog == -1) {
		glfwTerminate();
		return -1;
	}
	if (loadOBJ("C:\\Users\\Michael\\Desktop\\sphere.obj", verts, normals) == false) {
		glfwTerminate();
		return -1;
	}
	cam = glm::lookAt(glm::vec3(0.0, 0.0, 10.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	proj = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
	return 0;
}

static void draw_sphere_at(float x, float y, float z) {
	glm::mat4 model = glm::translate(glm::vec3(x, y, z));
	glm::mat4 mvp = proj * cam * model;
	glUniformMatrix4fv(mvp_unif_id, 1, GL_FALSE, &mvp[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, verts.size() * sizeof(glm::vec3));
}

int main(void) {
	if (init() != 0) {
		getchar();
		return 1;
	}
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3), &verts[0], GL_STATIC_DRAW);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glUseProgram(sphere_shader_prog);
	mvp_unif_id = glGetUniformLocation(sphere_shader_prog, "mvp");
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(sphere_shader_prog);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		for (int i = 0; i < 5; i++) {
			draw_sphere_at((float)i, 0.0f, (float)i * -1.0f);
		}
		glDisableVertexAttribArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}
