#include <string>
#include <fstream>

#include "GL/glew.h"

bool load_shader_file(std::string& loaded_string, std::string& filepath) {
	std::ifstream input(filepath, std::ios::in);
	if (input.is_open() && input.good() && !input.bad()) {
		loaded_string.assign((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
		input.close();
		input.clear();
		return true;
	} else {
		printf("Error opening the file\n");
		return false;
	}
}

GLint create_vertex_shader(std::string filepath) {
	std::string loaded_string;
	if (!load_shader_file(loaded_string, filepath)) {
		return -1;
	}
	const char *shader_string = loaded_string.c_str();
	GLint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &shader_string, NULL);
	glCompileShader(vs);
	return vs;

}

GLint create_fragment_shader(std::string filepath) {
	std::string loaded_string;
	if (!load_shader_file(loaded_string, filepath)) {
		return -1;
	}
	const char *shader_string = loaded_string.c_str();
	GLint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &shader_string, NULL);
	glCompileShader(fs);
	return fs;
}

bool check_shader_status(GLint vs, GLint fs) {
	GLint status;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		char buffer[512];
		glGetShaderInfoLog(vs, 512, NULL, buffer);
		printf("%s:\n %s\n", "Vertex shader:", buffer);
		return false;
	}
	glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		char buffer[512];
		glGetShaderInfoLog(fs, 512, NULL, buffer);
		printf("%s:\n %s\n", "Fragment shader:", buffer);
		return false;
	}
	return true;
}

GLint init_shaders() {
	GLint vs = create_vertex_shader("src//vert.glsl");
	if (vs == -1) {
		return -1;
	}
	GLint fs = create_fragment_shader("src//frag.glsl");
	if (fs == -1) {
		return -1;
	}
	if (check_shader_status(vs, fs) == false) {
		return -1;
	}
	GLint prog = glCreateProgram();
	glAttachShader(prog, fs);
	glAttachShader(prog, vs);
	glLinkProgram(prog);
	return prog;
}
