#ifndef KESHI_UTIL
#define KESHI_UTIL

#include <GL/glew.h>

#include <fstream>
#include <stdarg.h>
#include <time.h>

#define GL_LOG_FILE "log/gl.log"
#define GL_SHADER_LOG_LEN 2048

unsigned long getFileLength(std::ifstream& file);
int loadShader(const char* filename, GLuint shader);

// OpenGL logging stuff.
int restart_gl_log();
int gl_log(const char* msg, ...);
int gl_log_error(const char* msg, ...);
void gl_info();
void gl_ext_check(int *err_param);
void gl_error_debug(unsigned int source, unsigned int type,
					unsigned int id, unsigned int severity,
					int length, const char* message,
					void* user_param);

#endif
