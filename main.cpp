#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "math3d.h"
#include "util.h"

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
// Camera stuff.
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
	// Create plane VBOs.
	GLuint plane_vbos[6] = {0, 0, 0, 0, 0, 0};
	glGenBuffers(6, plane_vbos);
	GLuint plane_normal_vbos[6] = {0, 0, 0, 0, 0, 0};
	glGenBuffers(6, plane_normal_vbos);
	for (int i=0; i<6; i++) {
		glBindBuffer(GL_ARRAY_BUFFER, plane_vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planes[i]), planes[i], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, plane_normal_vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(plane_normals[i]), plane_normals[i], GL_STATIC_DRAW);
	}

	// Create the VAOs.
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	GLuint vao2 = 0;
	glGenVertexArrays(1, &vao2);
	glBindVertexArray(vao2);
	glBindBuffer(GL_ARRAY_BUFFER, points2_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	// Create plane VAOs.
	GLuint plane_vaos[6] = {0, 0, 0, 0, 0, 0};
	glGenVertexArrays(6, plane_vaos);
	for (int i=0; i<6; i++) {
		glBindVertexArray(plane_vaos[i]);
		glBindBuffer(GL_ARRAY_BUFFER, plane_vbos[i]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, plane_normal_vbos[i]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	}

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
	int view_matrix_loc = glGetUniformLocation(shader_prog, "view");
	int proj_matrix_loc = glGetUniformLocation(shader_prog, "proj");
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
	glUniform3f(light_pos_loc, 7.5f, 7.5f, 7.5f);
	glUniform3f(light_specular_loc, 1.0f, 1.0f, 1.0f);
	glUniform3f(light_diffuse_loc, 0.0f, 0.2f, 0.0f);
	glUniform3f(light_ambient_loc, 0.2f, 0.2f, 0.2f);
	glUniform3f(light2_pos_loc, 4.5f, 7.5f, 7.5f);
	glUniform3f(light2_specular_loc, 1.0f, 1.0f, 1.0f);
	glUniform3f(light2_diffuse_loc, 0.3f, 0.0f, 0.0f);
	glUniform3f(light2_ambient_loc, 0.2f, 0.2f, 0.2f);

	bool cam_moved = true;
	while (!glfwWindowShouldClose(window)) {
		cam_yaw = cam_roll = cam_pitch = 0.0f;
		c_move.v[0] = c_move.v[1] = c_move.v[2] = 0.0f;
		static double prev_sec = glfwGetTime();
		double cur_sec = glfwGetTime();
		double elapsed_sec = cur_sec - prev_sec;
		prev_sec = cur_sec;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, g_win_w, g_win_h);
		update_fps_counter(window);

		// Only update the projection matrix if necessary.
		if (update_proj_matrix) {
			persp_matrix = perspective(near, far, fov, a_ratio);
			glUniformMatrix4fv(proj_matrix_loc, 1, GL_TRUE, persp_matrix.m);
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
			glUniformMatrix4fv(view_matrix_loc, 1, GL_TRUE, c_view_matrix.m);

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
