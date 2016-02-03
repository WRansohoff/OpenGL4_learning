#include "util.h"

unsigned long getFileLength(std::ifstream& file) {
	if (!file.good()) return 0;

	unsigned long len = file.tellg();
	file.seekg(0, std::ios::end);
	len = file.tellg();
	file.seekg(std::ios::beg);

	return len;
}

int loadShader(const char* filename, GLuint shader) {
	std::ifstream file;
	file.open(filename, std::ios::in);
	if (!file) return 1;

	unsigned long len = getFileLength(file);
	char* contents = new char[len+1];
	const char* c_constants;
	file.read(contents, len);
	contents[len] = '\0';
	c_constants = contents;

	glShaderSource(shader, 1, &c_constants, NULL);
	glCompileShader(shader);

	// Log any shader compilation errors.
	int params = -1;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params) {
		gl_log_error("ERROR: GL shader '%s'(%i) did not compile\n", filename, shader);
		int actual_length = 0;
		char log[GL_SHADER_LOG_LEN];
		glGetShaderInfoLog(shader, GL_SHADER_LOG_LEN, &actual_length, log);
		gl_log("Shader info log for '%s'(%i):\n---\n%s\n---\n", filename, shader, log);
	}

	file.close();
	delete contents;

	return 0;
}

/*
 * OpenGL logging functions.
 */
int restart_gl_log() {
	FILE* file = fopen(GL_LOG_FILE, "a");
	if (!file) {
		fprintf(stderr, "Error: couldn't open log file for writing: %s\n", GL_LOG_FILE);
		return 1;
	}

	time_t now = time(NULL);
	char* datetime = ctime(&now);
	fprintf(file, "----------\nInit log. Local time %s\n", datetime);

	fclose(file);
	return 0;
}

int gl_log(const char* msg, ...) {
	va_list argptr;
	FILE* file = fopen(GL_LOG_FILE, "a");
	if (!file) {
		fprintf(stderr, "Error: couldn't open log file for appending: %s\n", GL_LOG_FILE);
		return 1;
	}

	va_start(argptr, msg);
	vfprintf(file, msg, argptr);
	va_end(argptr);
	
	fclose(file);
	return 0;
}

int gl_log_error(const char* msg, ...) {
	va_list argptr;
	FILE* file = fopen(GL_LOG_FILE, "a");
	if (!file) {
		fprintf(stderr, "Error: couldn't open log file for appending: %s\n", GL_LOG_FILE);
		return 1;
	}

	va_start(argptr, msg);
	vfprintf(file, msg, argptr);
	va_end(argptr);
	va_start(argptr, msg);
	vfprintf(stderr, msg, argptr);
	va_end(argptr);
	
	fclose(file);
	return 0;
}

void gl_info() {
	GLenum params[] = {
		GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
		GL_MAX_CUBE_MAP_TEXTURE_SIZE,
		GL_MAX_DRAW_BUFFERS,
		GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
		GL_MAX_TEXTURE_IMAGE_UNITS,
		GL_MAX_TEXTURE_SIZE,
		GL_MAX_VARYING_FLOATS,
		GL_MAX_VERTEX_ATTRIBS,
		GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
		GL_MAX_VERTEX_UNIFORM_COMPONENTS,
		GL_MAX_VIEWPORT_DIMS,
		GL_STEREO
	};
	const char* names[] = {
		"GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
		"GL_MAX_CUBE_MAP_TEXTURE_SIZE",
		"GL_MAX_DRAW_BUFFERS",
		"GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
		"GL_MAX_TEXTURE_IMAGE_UNITS",
		"GL_MAX_TEXTURE_SIZE",
		"GL_MAX_VARYING_FLOATS",
		"GL_MAX_VERTEX_ATTRIBS",
		"GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
		"GL_MAX_VERTEX_UNIFORM_COMPONENTS",
		"GL_MAX_VIEWPORT_DIMS",
		"GL_STEREO"
	};

	gl_log("GL Context Info:\n");
	for (int i=0; i<10; i++) {
		int v=0;
		glGetIntegerv(params[i], &v);
		gl_log("%s: %i\n", names[i], v);
	}
	int v[2];
	v[0] = v[1] = 0;
	glGetIntegerv(params[10], v);
	gl_log("%s %i %i\n", names[10], v[0], v[1]);
	unsigned char s = 0;
	glGetBooleanv(params[11], &s);
	gl_log("%s: %u\n", names[11], (unsigned int)s);
	gl_log("-----------------------------\n");
}
