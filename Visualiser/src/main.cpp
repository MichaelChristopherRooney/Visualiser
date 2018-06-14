#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "globals.hpp"

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

GLint init_shaders();
bool loadOBJ(const char * path, std::vector<glm::vec3> & out_vertices, std::vector<glm::vec3> & out_normals);
int init_parser(char *fp);
int get_next_iteration_sphere_state();

static GLFWwindow* window;
std::vector<glm::vec3> verts;
std::vector<glm::vec3> normals;
static glm::mat4 cam;
static glm::mat4 proj;
static GLint sphere_shader_prog;
static GLuint mvp_unif_id;
float simulation_time_of_next_event;
float simulation_time_of_last_event;

// Sets up glfw, glew and OpenGL
static int init_gl() {
	if (!glfwInit()) {
		printf("Failed to initialize GLFW\n");
		return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
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
	cam = glm::lookAt(glm::vec3(25.0f, 25.0f, 75.0f), glm::vec3(25.0f, 25.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	proj = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 100.0f);
	return 0;
}

// TODO: scale with radius
static void draw_sphere_at(glm::vec3 pos, float radius) {
	radius = radius * 2.0f; // TODO: spheres are half their actual size - need to fix properly
	glm::mat4 scale = glm::scale(glm::vec3(radius, radius, radius));
	glm::mat4 translate = glm::translate(pos);
	glm::mat4 mvp = proj * cam * translate * scale;
	glUniformMatrix4fv(mvp_unif_id, 1, GL_FALSE, &mvp[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, verts.size() * sizeof(glm::vec3)); 
}

static const float tick_rate = 1.0f / 60.0f;

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
	float time_elapsed_iter = 0.0f;
	float cur_time = (float) glfwGetTime();
	bool last_iter = false;
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(sphere_shader_prog);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		for (int i = 0; i < num_spheres; i++) {
			draw_sphere_at(spheres[i].pos, spheres[i].radius);
			update_sphere_position(&spheres[i], tick_rate);
		}
		glDisableVertexAttribArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();
		// uncomment to run in real time
		// running at a faster rate is useful for debugging
		//while (((float) glfwGetTime() - cur_time) < tick_rate) {}
		cur_time = glfwGetTime();
		time_elapsed_iter += tick_rate;
		// TODO: time between iterations may be less than tick rate
		if (time_elapsed_iter >= simulation_time_of_next_event - simulation_time_of_last_event) {
			time_elapsed_iter = 0.0f;
			if (last_iter) {
				break;
			} else {
				int fin = get_next_iteration_sphere_state();
				if (fin == 1) {
					last_iter = true;
				} else if (simulation_time_of_next_event - simulation_time_of_last_event <= tick_rate) {
					printf("TODO: unhandled case\n");
					getchar();
					exit(1);
				}
			}
		}
	}
	glfwTerminate();
	return 0;
}
