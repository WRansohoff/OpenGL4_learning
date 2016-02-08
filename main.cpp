#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "math2d.h"
#include "math3d.h"
#include "util.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define KSHI_AA_SAMPLES 16

// GLFW callback functions.
void glfw_log_err(int err, const char* desc);
void glfw_win_resize(GLFWwindow* window, int w, int h);
void glfw_mouse_pos(GLFWwindow* window, double m_x, double m_y);
void glfw_mouse_button(GLFWwindow* window, int button, int action, int mods);
// Bookkeeping.
void update_fps_counter(GLFWwindow* window);

// Basic UI values.
int g_win_w = 1280;
int g_win_h = 720;
// Bookkeeping.
double prev_seconds;
int frame_count;
int ogl_err = -1;
bool debug = false;
// Camera stuff.
int ubo_cam = 0;
float cam_speed = 2.0f;
float cam_yaw_speed = 100.0f;
float cam_pitch_speed = 100.0f;
float cam_roll_speed = 100.0f;
float cam_yaw = 180.0f; // degrees
float cam_pitch = 0.0f;
float cam_roll = 0.0f;
v3 cam_pos(0.0f, 0.0f, -2.0f);
v3 cam_rot(0.0f, 0.0f, 0.0f);
v4 cam_quat_v(0.0f, 0.0f, 1.0f, 0.0f);
v4 cam_quat_roll_v(0.0f, 0.0f, 0.0f, 1.0f);
v4 cam_quat_pitch_v(0.0f, 1.0f, 0.0f, 0.0f);
quat cam_quat(cam_quat_v.v[0], cam_quat_v.v[1], cam_quat_v.v[2], cam_quat_v.v[3]);
quat cam_quat_yaw(cam_quat_v.v[0], cam_quat_v.v[1], cam_quat_v.v[2], cam_quat_v.v[3]);
quat cam_quat_roll(cam_quat_roll_v.v[0], cam_quat_roll_v.v[1], cam_quat_roll_v.v[2], cam_quat_roll_v.v[3]);
quat cam_quat_pitch(cam_quat_pitch_v.v[0], cam_quat_pitch_v.v[1], cam_quat_pitch_v.v[2], cam_quat_pitch_v.v[3]);
v3 target_pos(0.0f, 0.0f, 0.0f);
v3 y_up(0.0f, 1.0f, 0.0f);
v3 c_move(0.0f, 0.0f, 0.0f);
// Lighting stuff.
int ubo_lights = 1;
float light_speed = 20.0f;
float light_z = 7.5f;
float light2_x = 4.5f;
int light_dir = -1;
int light2_dir = -1;
// Texture stuff.
int tex_x, tex_y, tex_n;
int tex_channels = 4;
unsigned char* tex_data;
const char* tex_fn = "textures/png/test_texture.png";
// Mouse stuff.
double mouse_x = 0.0f;
double mouse_y = 0.0f;
// Perspective matrix values.
float near = 0.1f;
float far = 99.9f;
// vertical fov = horizontal fov * aspect ratio.
// 67.0 ~= 90 @ 4:3. 50.625 = 90 @ 16:9.
float fov = 50.625f;
float a_ratio = (float)g_win_w / (float)g_win_h;
bool update_proj_matrix = false;

// Shaders. Better to load these from text files, but for now...
const char* vertex_shader_fn = "shaders/vert/test.vert";
const char* frag_shader_fn = "shaders/frag/test.frag";

int main(int argc, char** args) {
	assert(restart_gl_log() == 0);

	// Initialize GLFW and GLEW.
	gl_log("Initialize GLFW\n%s\n", glfwGetVersionString());
	glfwSetErrorCallback(glfw_log_err);
	if (!glfwInit()) {
		fprintf(stderr, "Error: Could not start GLFW.\n");
		return 1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Anti-aliasing is nice.
	glfwWindowHint(GLFW_SAMPLES, KSHI_AA_SAMPLES);
	// Allow debugging stuff.
	if (debug) {
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	}

	// Until I make an options menu or something, put the window on
	// a monitor if available, to my laptop screen if not. (Uncomment to use fullscreen)
	/*
	int numMonitors;
	GLFWmonitor** monitors = glfwGetMonitors(&numMonitors);
	GLFWmonitor* monitor;
	if (numMonitors > 1) {
		monitor = monitors[1];
	}
	else {
		monitor = monitors[0];
	}
	const GLFWvidmode* vmode = glfwGetVideoMode(monitor);
	GLFWwindow* window = glfwCreateWindow(vmode->width, vmode->height, "Hello, OpenGL?", monitor, NULL);
	*/

	GLFWwindow* window = glfwCreateWindow(g_win_w, g_win_h, "Hello, OpenGL?", NULL, NULL);
	if (!window) {
		fprintf(stderr, "Error: Could not initialize window.\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, glfw_win_resize);
	glfwSetCursorPosCallback(window, glfw_mouse_pos);
	glfwSetMouseButtonCallback(window, glfw_mouse_button);

	glewExperimental = GL_TRUE;
	glewInit();

	// Setup extensions, if possible.
	if (debug) {
		gl_ext_check(&ogl_err);
	}

	// Get compatibility information.
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported: %s\n", version);

	// Depth testing; enable and define less depth as 'closer'.
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Create 3 points for a triangle.
	GLfloat points[] = {
		 0.0f,  0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f,
		-0.5f, -0.5f,  0.0f
	};
	GLfloat points2[] = {
		0.5f, 0.5f, 0.5f,
		0.0f, 0.0f, 0.5f,
		0.0f, 0.5f, 0.5f
	};
	GLfloat planes[6][18] = {
		{
		 10.0f,   7.0f, -10.0f,
		 10.0f,   8.0f,  10.0f,
		 10.0f, -10.0f,  10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f,   7.0f, -10.0f,
		 10.0f, -10.0f,  10.0f
		},
		{
		-10.0f,  10.0f, -10.0f,
		 10.0f,  10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f
		},
		{
		 10.0f,  10.0f, -10.0f,
		 10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f, -10.0f,
		 10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f,  10.0f
		},
		{
		 10.0f,  10.0f,  10.0f,
		 10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f
		},
		{
		-10.0f,  10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f, -10.0f, -10.0f,
		-10.0f,  10.0f,  10.0f
		},
		{
		 10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f, -10.0f
		}
	};
	// Not used with phong lighting.
	GLfloat colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
	// Normals.
	GLfloat normals[] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};
	GLfloat plane_normals[6][18] = {
		{
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f
		},
		{
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f
		},
		{
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f
		},
		{
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f
		},
		{
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f
		},
		{
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f
		}
	};
	GLfloat plane_colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.0f,
		0.5f, 0.0f, 0.5f
	};
	GLfloat tri_texcoords[] = {
		 0.0f,  0.5f,
		 0.5f, -0.5f,
		-0.5f, -0.5f
	};
	GLfloat plane_texcoords[6][12] = {
		{
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f
		},
		{
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f
		},
		{
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f
		},
		{
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 1.0f
		},
		{
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 1.0f
		},
		{
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f
		}
	};

	// Create the VBOs.
	GLuint points_vbo = 0;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	GLuint points2_vbo = 0;
	glGenBuffers(1, &points2_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points2_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points2), points2, GL_STATIC_DRAW);
	GLuint normals_vbo = 0;
	glGenBuffers(1, &normals_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	GLuint tri_tex_vbo = 0;
	glGenBuffers(1, &tri_tex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, tri_tex_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri_texcoords), tri_texcoords, GL_STATIC_DRAW);
	// Create plane VBOs.
	GLuint plane_vbos[6] = {0, 0, 0, 0, 0, 0};
	glGenBuffers(6, plane_vbos);
	GLuint plane_normal_vbos[6] = {0, 0, 0, 0, 0, 0};
	glGenBuffers(6, plane_normal_vbos);
	GLuint plane_tex_vbos[6] = {0, 0, 0, 0, 0, 0};
	glGenBuffers(6, plane_tex_vbos);
	for (int i=0; i<6; i++) {
		glBindBuffer(GL_ARRAY_BUFFER, plane_vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planes[i]), planes[i], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, plane_normal_vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(plane_normals[i]), plane_normals[i], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, plane_tex_vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(plane_texcoords[i]), plane_texcoords[i], GL_STATIC_DRAW);
	}

	// Create the VAOs.
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, tri_tex_vbo);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	GLuint vao2 = 0;
	glGenVertexArrays(1, &vao2);
	glBindVertexArray(vao2);
	glBindBuffer(GL_ARRAY_BUFFER, points2_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, tri_tex_vbo);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	// Create plane VAOs.
	GLuint plane_vaos[6] = {0, 0, 0, 0, 0, 0};
	glGenVertexArrays(6, plane_vaos);
	for (int i=0; i<6; i++) {
		glBindVertexArray(plane_vaos[i]);
		glBindBuffer(GL_ARRAY_BUFFER, plane_vbos[i]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, plane_normal_vbos[i]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, plane_tex_vbos[i]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
	}

	// Load and bind textures.
	// Load texture data.
	tex_data = stbi_load(tex_fn, &tex_x, &tex_y, &tex_n, tex_channels);
	if (!tex_data) {
		gl_log_error("ERROR: Could not load image: %s\n", tex_fn);
	}
	if ((tex_x & (tex_x-1)) != 0 || (tex_y & (tex_y-1) != 0)) {
		gl_log("WARN:  texture %s has x or y % 2 != 0", tex_fn);
	}
	// Flip the texture vertically.
	flip_tex_V(tex_data, tex_x, tex_y, tex_n);

	// Bind texture data.
	int mipmap_LOD = 0;
	GLuint tex = 0;
	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D,
				 mipmap_LOD, GL_SRGB_ALPHA, tex_x, tex_y,
				 0, GL_RGBA, GL_UNSIGNED_BYTE,
				 tex_data);
	// Some basic texture parameters.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// Generate mipmaps.
	glGenerateMipmap(GL_TEXTURE_2D);
	// Enable max supported level of anisotropic filtering.
	GLfloat max_anisotropic = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropic);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropic);
	//printf("Using %.2f anisotropic samples\n", max_anisotropic);

	// Compile the shaders.
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	loadShader(vertex_shader_fn, vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	loadShader(frag_shader_fn, fs);
	GLuint shader_prog = glCreateProgram();
	glAttachShader(shader_prog, fs);
	glAttachShader(shader_prog, vs);
	glLinkProgram(shader_prog);

	// Log shader linking errors.
	int params = -1;
	glGetProgramiv(shader_prog, GL_LINK_STATUS, &params);
	if (GL_TRUE != params) {
		gl_log_error("(%i) Error in shader linking!\n", shader_prog);
		int actual_length = 0;
		char log[GL_SHADER_LOG_LEN];
		glGetProgramInfoLog(shader_prog, GL_SHADER_LOG_LEN, &actual_length, log);
		gl_log("Shader log:\n---\n%s---\n", log);
	}

	// Setup initial camera values.
	m4 cam_trans = translation_matrix(cam_pos.v[0], cam_pos.v[1], cam_pos.v[2]);
	m4 cam_rot = quaternion_to_rotation(cam_quat);
	m4 c_view_matrix = look_at(cam_pos, target_pos, y_up);
	m4 persp_matrix = perspective(near, far, fov, a_ratio);
	v4 c_right(c_view_matrix.m[0], c_view_matrix.m[1], c_view_matrix.m[2], 0.0f);
	v4 c_up(c_view_matrix.m[4], c_view_matrix.m[5], c_view_matrix.m[6], 0.0f);
	v4 c_fwd(-c_view_matrix.m[8], -c_view_matrix.m[9], -c_view_matrix.m[10], 0.0f);

	float speed = 1.0f;
	float last_pos = 0.5f;
	// Draw loop.
	glClearColor(0.5f, 0.5f, 0.6f, 1.0f);

	// Non-UBO uniform setting.
	/*
	int view_matrix_loc = glGetUniformLocation(shader_prog, "view");
	int proj_matrix_loc = glGetUniformLocation(shader_prog, "proj");
	// Phong light values.
	int light_pos_loc = glGetUniformLocation(shader_prog, "light_pos_W");
	int light_specular_loc = glGetUniformLocation(shader_prog, "Ls");
	int light_diffuse_loc = glGetUniformLocation(shader_prog, "Ld");
	int light_ambient_loc = glGetUniformLocation(shader_prog, "La");
	int light2_pos_loc = glGetUniformLocation(shader_prog, "light2_pos_W");
	int light2_specular_loc = glGetUniformLocation(shader_prog, "Ls2");
	int light2_diffuse_loc = glGetUniformLocation(shader_prog, "Ld2");
	int light2_ambient_loc = glGetUniformLocation(shader_prog, "La2");
	glUseProgram(shader_prog);
	glUniformMatrix4fv(view_matrix_loc, 1, GL_TRUE, c_view_matrix.m);
	glUniformMatrix4fv(proj_matrix_loc, 1, GL_TRUE, persp_matrix.m);
	glUniform3f(light_pos_loc, 7.5f, 7.5f, light_z);
	glUniform3f(light_specular_loc, 1.0f, 1.0f, 1.0f);
	glUniform3f(light_diffuse_loc, 0.5f, 0.7f, 0.5f);
	glUniform3f(light_ambient_loc, 0.2f, 0.2f, 0.2f);
	glUniform3f(light2_pos_loc, light2_x, 7.5f, 6.5f);
	glUniform3f(light2_specular_loc, 1.0f, 1.0f, 1.0f);
	glUniform3f(light2_diffuse_loc, 0.5f, 0.5f, 0.7f);
	glUniform3f(light2_ambient_loc, 0.2f, 0.2f, 0.2f);
	*/

	glUseProgram(shader_prog);

	// Setup uniform buffer objects.
	// One for camera values, one for lighting values.
	GLuint cam_block_buffer;
	glGenBuffers(1, &cam_block_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, cam_block_buffer);
	// Store view and projection matrices; 2 * 16 floats.
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 32, NULL, GL_DYNAMIC_DRAW);
	GLuint cam_ubo_index = glGetUniformBlockIndex(shader_prog, "cam_ubo");
	glUniformBlockBinding(shader_prog, cam_ubo_index, ubo_cam);
	glBindBufferBase(GL_UNIFORM_BUFFER, ubo_cam, cam_block_buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 16, transpose(c_view_matrix).m);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 16, sizeof(float) * 16, transpose(persp_matrix).m);

	GLuint lights_block_buffer;
	glGenBuffers(1, &lights_block_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, lights_block_buffer);
	// Store position, specular, diffuse, and ambient values for 2 lights.
	// We'll use v4's, so that's 16 * 2 floats.
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 32, NULL, GL_DYNAMIC_DRAW);
	GLuint lights_ubo_index = glGetUniformBlockIndex(shader_prog, "lights_ubo");
	glUniformBlockBinding(shader_prog, lights_ubo_index, ubo_lights);
	glBindBufferBase(GL_UNIFORM_BUFFER, ubo_lights, lights_block_buffer);
	float light_pos[] = { 7.5f, 7.5f, light_z, 1.0f };
	float light2_pos[] = { light2_x, 7.5f, 6.5f, 1.0f };
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 4, light_pos);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 4, sizeof(float) * 4, light2_pos);
	float light_spec[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	float light_diffuse[] = { 0.5f, 0.7f, 0.5f, 0.0f };
	float light_ambient[] = { 0.2f, 0.2f, 0.2f, 0.0f };
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 8, sizeof(float) * 4, light_spec);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 12, sizeof(float) * 4, light_diffuse);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 16, sizeof(float) * 4, light_ambient);
	float light2_diffuse[] = { 0.5f, 0.5f, 0.7f, 0.0f };
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 20, sizeof(float) * 4, light_spec);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 24, sizeof(float) * 4, light2_diffuse);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 28, sizeof(float) * 4, light_ambient);

	bool cam_moved = true;
	while (!glfwWindowShouldClose(window)) {
		cam_yaw = cam_roll = cam_pitch = 0.0f;
		c_move.v[0] = c_move.v[1] = c_move.v[2] = 0.0f;
		static double prev_sec = glfwGetTime();
		double cur_sec = glfwGetTime();
		double elapsed_sec = cur_sec - prev_sec;
		prev_sec = cur_sec;

		// Update light positions.
		light_z += light_dir * light_speed * elapsed_sec;
		light2_x += light2_dir * light_speed * elapsed_sec;
		if (light_z >= 8.0f) { light_dir = -1; }
		else if (light_z <= -8.0f) { light_dir = 1; }
		if (light2_x >= 8.0f) { light2_dir = -1; }
		else if (light2_x <= -8.0f) { light2_dir = 1; }
		// Update shaders.
		glBindBuffer(GL_UNIFORM_BUFFER, lights_block_buffer);
		glBindBufferBase(GL_UNIFORM_BUFFER, ubo_lights, lights_block_buffer);
		light_pos[2] = light_z;
		light2_pos[0] = light2_x;
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 4, light_pos);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 4, sizeof(float) * 4, light2_pos);
		//glUniform3f(light_pos_loc, 7.5f, 7.5f, light_z);
		//glUniform3f(light2_pos_loc, light2_x, 7.5f, 6.5f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, g_win_w, g_win_h);
		update_fps_counter(window);

		// Only update the projection matrix if necessary.
		if (update_proj_matrix) {
			persp_matrix = perspective(near, far, fov, a_ratio);
			glBindBuffer(GL_UNIFORM_BUFFER, cam_block_buffer);
			glBindBufferBase(GL_UNIFORM_BUFFER, ubo_cam, cam_block_buffer);
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 16, sizeof(float) * 16, transpose(persp_matrix).m);
			//glUniformMatrix4fv(proj_matrix_loc, 1, GL_TRUE, persp_matrix.m);
			update_proj_matrix = false;
		}

		// Check input.
		glfwPollEvents();
		// Escape = quit.
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, 1);
		}
		// Camera movement. First update vectors, then apply transformations.
		// khjluo for rotation
		if (glfwGetKey(window, GLFW_KEY_H)) {
			cam_yaw -= cam_yaw_speed * elapsed_sec;
			cam_quat_yaw = set(cam_yaw, c_up);
			cam_quat = cam_quat * cam_quat_yaw;
			cam_rot = quaternion_to_rotation(cam_quat);
			c_right = cam_rot.row(0);
			c_up = cam_rot.row(1);
			c_fwd = cam_rot.row(2) * -1.0f;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_L)) {
			cam_yaw += cam_yaw_speed * elapsed_sec;
			cam_quat_yaw = normalize(set(cam_yaw, c_up));
			cam_quat = cam_quat * cam_quat_yaw;
			cam_rot = quaternion_to_rotation(cam_quat);
			c_right = cam_rot.row(0);
			c_up = cam_rot.row(1);
			c_fwd = cam_rot.row(2) * -1.0f;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_K)) {
			cam_pitch -= cam_pitch_speed * elapsed_sec;
			cam_quat_pitch = normalize(set(cam_pitch, c_right));
			cam_quat = cam_quat * cam_quat_pitch;
			cam_rot = quaternion_to_rotation(cam_quat);
			c_right = cam_rot.row(0);
			c_up = cam_rot.row(1);
			c_fwd = cam_rot.row(2) * -1.0f;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_J)) {
			cam_pitch += cam_pitch_speed * elapsed_sec;
			cam_quat_pitch = normalize(set(cam_pitch, c_right));
			cam_quat = cam_quat * cam_quat_pitch;
			cam_rot = quaternion_to_rotation(cam_quat);
			c_right = cam_rot.row(0);
			c_up = cam_rot.row(1);
			c_fwd = cam_rot.row(2) * -1.0f;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_U)) {
			cam_roll += cam_roll_speed * elapsed_sec;
			cam_quat_roll = normalize(set(cam_roll, c_fwd));
			cam_quat = cam_quat * cam_quat_roll;
			cam_rot = quaternion_to_rotation(cam_quat);
			c_right = cam_rot.row(0);
			c_up = cam_rot.row(1);
			c_fwd = cam_rot.row(2) * -1.0f;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_O)) {
			cam_roll -= cam_roll_speed * elapsed_sec;
			cam_quat_roll = normalize(set(cam_roll, c_fwd));
			cam_quat = cam_quat * cam_quat_roll;
			cam_rot = quaternion_to_rotation(cam_quat);
			c_right = cam_rot.row(0);
			c_up = cam_rot.row(1);
			c_fwd = cam_rot.row(2) * -1.0f;
			cam_moved = true;
		}
		// wasd for movement.
		if (glfwGetKey(window, GLFW_KEY_A)) {
			c_move.v[0] += c_right.v[0] * cam_speed * elapsed_sec;
			c_move.v[1] += c_right.v[1] * cam_speed * elapsed_sec;
			c_move.v[2] += c_right.v[2] * cam_speed * elapsed_sec;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_D)) {
			c_move.v[0] -= c_right.v[0] * cam_speed * elapsed_sec;
			c_move.v[1] -= c_right.v[1] * cam_speed * elapsed_sec;
			c_move.v[2] -= c_right.v[2] * cam_speed * elapsed_sec;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_W)) {
			c_move.v[0] -= c_fwd.v[0] * cam_speed * elapsed_sec;
			c_move.v[1] -= c_fwd.v[1] * cam_speed * elapsed_sec;
			c_move.v[2] -= c_fwd.v[2] * cam_speed * elapsed_sec;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			c_move.v[0] += c_fwd.v[0] * cam_speed * elapsed_sec;
			c_move.v[1] += c_fwd.v[1] * cam_speed * elapsed_sec;
			c_move.v[2] += c_fwd.v[2] * cam_speed * elapsed_sec;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_Q)) {
			c_move.v[0] += c_up.v[0] * cam_speed * elapsed_sec;
			c_move.v[1] += c_up.v[1] * cam_speed * elapsed_sec;
			c_move.v[2] += c_up.v[2] * cam_speed * elapsed_sec;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_E)) {
			c_move.v[0] -= c_up.v[0] * cam_speed * elapsed_sec;
			c_move.v[1] -= c_up.v[1] * cam_speed * elapsed_sec;
			c_move.v[2] -= c_up.v[2] * cam_speed * elapsed_sec;
			cam_moved = true;
		}

		// Updates based on camera movement.
		if (cam_moved) {
			printf("CQ: %s\n", print(cam_quat).c_str());
			cam_pos.v[0] += c_move.v[0];
			cam_pos.v[1] += c_move.v[1];
			cam_pos.v[2] += c_move.v[2];
			m4 cam_trans = translation_matrix(cam_pos.v[0], cam_pos.v[1], cam_pos.v[2]);
			c_view_matrix = view_matrix(cam_trans, cam_rot);

			printf("Yaw:   %.2f\nRoll:  %.2f\nPitch: %.2f\n", cam_yaw, cam_roll, cam_pitch);
			printf("Up:    %s\nRight: %s\nFwd:   %s\n", print(c_up).c_str(), print(c_right).c_str(), print(c_fwd).c_str());
			printf("Rotation matrix:\n%s\n", print(quaternion_to_rotation(cam_quat)).c_str());

			// Don't forget to tell the shaders.
			glBindBuffer(GL_UNIFORM_BUFFER, cam_block_buffer);
			glBindBufferBase(GL_UNIFORM_BUFFER, ubo_cam, cam_block_buffer);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 16, transpose(c_view_matrix).m);
			//glUniformMatrix4fv(view_matrix_loc, 1, GL_TRUE, c_view_matrix.m);

			cam_moved = false;
		}

		// Draw stuff, flip buffers.
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(vao2);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		for (int i=0; i<6; i++) {
			glBindVertexArray(plane_vaos[i]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glfwSwapBuffers(window);
	}

	// Exit.
	//gl_info();
	glfwTerminate();
	return 0;
}

/*
 * GLFW callback functions.
 */
void glfw_log_err(int err, const char* description) {
	gl_log_error("GLFW Error (%i): %s\n", err, description);
}

void glfw_win_resize(GLFWwindow* window, int w, int h) {
	g_win_w = w;
	g_win_h = h;
	a_ratio = (float)w / (float)h;
	
	// Update the viewport.
	glViewport(0, 0, g_win_w, g_win_h);

	// We should update the projection matrix next game loop step.
	update_proj_matrix = true;
}

void glfw_mouse_pos(GLFWwindow* window, double m_x, double m_y) {
	mouse_x = m_x;
	mouse_y = m_y;
}

void glfw_mouse_button(GLFWwindow* window, int button, int action, int mods) {
	printf("MX: %.2f\nMY: %.2f\n", mouse_x, mouse_y);
}

/*
 * Bookkeeping.
 */
void update_fps_counter(GLFWwindow* window) {
	double cur_seconds, elapsed_seconds;
	cur_seconds = glfwGetTime();
	elapsed_seconds = cur_seconds - prev_seconds;
	// Limit to ~4 updates per second.
	if (elapsed_seconds > 0.25) {
		prev_seconds = cur_seconds;
		char tmp[128];
		double fps = (double)frame_count / elapsed_seconds;
		sprintf(tmp, "OpenGL - FPS: %.2f", fps);
		glfwSetWindowTitle(window, tmp);
		frame_count = 0;
	}
	frame_count ++;
}
