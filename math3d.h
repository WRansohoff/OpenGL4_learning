#ifndef KESHI_MATH3D
#define KESHI_MATH3D

#include <math.h>
#include <string>

#include "util.h"

#define PI 3.14159

using std::string;
using std::to_string;

// 3D math structs.
struct v2;
struct v3;
struct v4;
struct m3;
struct m4;

struct v2 {
	float v[2];
	v2();
	v2(float x, float y);
};

struct v3 {
	float v[3];
	v3();
	v3(float x, float y, float z);
	v3(const v4& other);

	v3 operator+(const v3& other);
	v3 operator+(float scalar);
	v3& operator+=(const v3& other);
	v3& operator+=(float scalar);
	v3 operator-(const v3& other);
	v3 operator-(float scalar);
	v3& operator-=(const v3& other);
	v3& operator-=(float scalar);
	v3 operator*(float scalar);
	v3& operator*=(float scalar);
	v3 operator/(float scalar);
	v3& operator/=(float scalar);
	v3& operator=(const v3& other);
};

struct v4 {
	float v[4];
	v4();
	v4(float x, float y, float z, float w);
	v4(const v3& other, float w);

	v4 operator*(float scalar);
	v4& operator*=(float scalar);
	v4 operator/(float scalar);
	v4& operator/=(float scalar);
};

/*
 * 0 1 2
 * 3 4 5
 * 6 7 8
 */
struct m3 {
	float m[9];
	m3();
	m3(float a, float b, float c,
	   float d, float e, float f,
	   float g, float h, float i);
};

/*
 * 0  1  2  3
 * 4  5  6  7
 * 8  9  10 11
 * 12 13 14 15
 */
struct m4 {
	float m[16];
	m4();
	m4(float a, float b, float c, float d,
	   float e, float f, float g, float h,
	   float i, float j, float k, float l,
	   float n, float o, float p, float q);
	v4 operator*(const v4& other);
	m4 operator*(const m4& other);
	m4& operator=(const m4& other);

	v4 row(const int row);
	v4 col(const int row);
};

// Quaternion. (angle, x, y, z)
struct quat {
	float q[4];
	quat();
	quat(float a, float x, float y, float z);
	quat operator/(float scalar);
	quat& operator/=(float scalar);
	// q1 * q2 means, "rotate q2, then rotate q1".
	quat operator*(quat other);
	quat& operator*=(quat& other);
	quat& operator=(const quat& other);
};

// For logging etc.
string print(v2 vec);
string print(v3 vec);
string print(v4 vec);
string print(m3 mat);
string print(m4 mat);
string print(quat q);

// Generic math functions.
float ang_to_rad(float a);
float rad_to_ang(float r);

// 3D math functions.
// Vec3 functions.
float magnitude(v3& vec);
v3 normalize(v3& vec);
v3 cross(v3& vec1, v3& vec2);
v3 lerp(v3& vec1, v3& vec2, float prog);
// 4x4 Matrix functions.
m4 id4();
m4 rotate_x(float degrees);
m4 rotate_x_rads(float rads);
m4 rotate_y(float degrees);
m4 rotate_y_rads(float rads);
m4 rotate_z(float degrees);
m4 rotate_z_rads(float rads);
m4 translation_matrix(float x, float y, float z);
m4 quaternion_to_rotation(quat q);
m4 view_matrix(m4 translation, m4 rotation);
m4 look_at(v3 pos, v3 t_pos, v3 up);
m4 look_at(v3 pos, v3 t_angles);
m4 perspective(float near, float far, float fov, float aspect_ratio);
// Quaternion functions.
float dot(quat q1, quat q2);
float norm(quat q);
quat set(float a, v3 vec);
quat set(float a, float x, float y, float z);
quat normalize(quat q);
quat inverse(quat q);
quat conjugate(quat q);
quat slerp(quat q1, quat q2, float prog);

#endif
