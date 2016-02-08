#include "math3d.h"

// 3D math struct definitions.
v2::v2() {
	v[0] = v[1] = 0;
}

v2::v2(float x, float y) {
	v[0] = x;
	v[1] = y;
}

v3::v3() {
	v[0] = v[1] = v[2] = 0;
}

v3::v3(float x, float y, float z) {
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

v3::v3(const v4& other) {
	// Just discard the v4's last element.
	v[0] = other.v[0];
	v[1] = other.v[1];
	v[2] = other.v[2];
}

v3 v3::operator+(const v3& other) {
	v3 vec;
	vec.v[0] = v[0] + other.v[0];
	vec.v[1] = v[1] + other.v[1];
	vec.v[2] = v[2] + other.v[2];
	return vec;
}

v3 v3::operator+(float scalar) {
	v3 vec;
	vec.v[0] = v[0] + scalar;
	vec.v[1] = v[1] + scalar;
	vec.v[2] = v[2] + scalar;
	return vec;
}

v3& v3::operator+=(const v3& other) {
	v[0] = v[0] + other.v[0];
	v[1] = v[1] + other.v[1];
	v[2] = v[2] + other.v[2];
	return *this;
}

v3& v3::operator+=(float scalar) {
	v[0] = v[0] + scalar;
	v[1] = v[1] + scalar;
	v[2] = v[2] + scalar;
	return *this;
}

v3 v3::operator-(const v3& other) {
	v3 vec;
	vec.v[0] = v[0] - other.v[0];
	vec.v[1] = v[1] - other.v[1];
	vec.v[2] = v[2] - other.v[2];
	return vec;
}

v3 v3::operator-(float scalar) {
	v3 vec;
	vec.v[0] = v[0] - scalar;
	vec.v[1] = v[1] - scalar;
	vec.v[2] = v[2] - scalar;
	return vec;
}

v3& v3::operator-=(const v3& other) {
	v[0] = v[0] - other.v[0];
	v[1] = v[1] - other.v[1];
	v[2] = v[2] - other.v[2];
	return *this;
}

v3& v3::operator-=(float scalar) {
	v[0] = v[0] - scalar;
	v[1] = v[1] - scalar;
	v[2] = v[2] - scalar;
	return *this;
}

v3 v3::operator*(float scalar) {
	v3 vec;
	vec.v[0] = v[0] * scalar;
	vec.v[1] = v[1] * scalar;
	vec.v[3] = v[2] * scalar;
	return vec;
}

v3& v3::operator*=(float scalar) {
	v[0] = v[0] * scalar;
	v[1] = v[1] * scalar;
	v[3] = v[2] * scalar;
	return *this;
}

v3 v3::operator/(float scalar) {
	v3 vec;
	vec.v[0] = v[0] / scalar;
	vec.v[1] = v[1] / scalar;
	vec.v[2] = v[2] / scalar;
	return vec;
}

v3& v3::operator/=(float scalar) {
	v[0] = v[0] / scalar;
	v[1] = v[1] / scalar;
	v[3] = v[2] / scalar;
	return *this;
}

v3& v3::operator=(const v3& other) {
	v[0] = other.v[0];
	v[1] = other.v[1];
	v[2] = other.v[2];
	return *this;
}

v4::v4() {
	v[0] = v[1] = v[2] = v[3] = 0;
}

v4::v4(float x, float y, float z, float w) {
	v[0] = x;
	v[1] = y;
	v[2] = z;
	v[3] = w;
}

v4::v4(const v3& other, float w) {
	v[0] = other.v[0];
	v[1] = other.v[1];
	v[2] = other.v[2];
	v[3] = w;
}

v4 v4::operator*(float scalar) {
	v4 vec;
	vec.v[0] = v[0] * scalar;
	vec.v[1] = v[1] * scalar;
	vec.v[2] = v[2] * scalar;
	vec.v[3] = v[3] * scalar;
	return vec;
}

v4& v4::operator*=(float scalar) {
	v[0] = v[0] * scalar;
	v[1] = v[1] * scalar;
	v[2] = v[2] * scalar;
	v[3] = v[3] * scalar;
	return *this;
}

v4 v4::operator/(float scalar) {
	v4 vec;
	vec.v[0] = v[0] / scalar;
	vec.v[1] = v[1] / scalar;
	vec.v[2] = v[2] / scalar;
	vec.v[3] = v[3] / scalar;
	return vec;
}

v4& v4::operator/=(float scalar) {
	v[0] = v[0] / scalar;
	v[1] = v[1] / scalar;
	v[2] = v[2] / scalar;
	v[3] = v[3] / scalar;
	return *this;
}

m3::m3() {
	m[0] = m[1] = m[2] = 0;
	m[3] = m[4] = m[5] = 0;
	m[6] = m[7] = m[8] = 0;
}

m3::m3(float a, float b, float c,
   float d, float e, float f,
   float g, float h, float i) {
	m[0] = a;
	m[1] = b;
	m[2] = c;
	m[3] = d;
	m[4] = e;
	m[5] = f;
	m[6] = g;
	m[7] = h;
	m[8] = i;
}

m4::m4() {
	m[0] = m[1] = m[2] = m[3] = 0;
	m[4] = m[5] = m[6] = m[7] = 0;
	m[8] = m[9] = m[10] = m[11] = 0;
	m[12] = m[13] = m[14] = m[15] = 0;
}

m4::m4(float a, float b, float c, float d,
   float e, float f, float g, float h,
   float i, float j, float k, float l,
   float n, float o, float p, float q) {
	m[0]  = a;
	m[1]  = b;
	m[2]  = c;
	m[3]  = d;
	m[4]  = e;
	m[5]  = f;
	m[6]  = g;
	m[7]  = h;
	m[8]  = i;
	m[9]  = j;
	m[10] = k;
	m[11] = l;
	m[12] = n;
	m[13] = o;
	m[14] = p;
	m[15] = q;
}

v4 m4::operator*(const v4& other) {
	v4 vec;
	vec.v[0] = m[0]*other.v[0] + m[1]*other.v[1] + m[2]*other.v[2] + m[3]*other.v[3];
	vec.v[1] = m[4]*other.v[0] + m[5]*other.v[1] + m[6]*other.v[2] + m[7]*other.v[3];
	vec.v[2] = m[8]*other.v[0] + m[9]*other.v[1] + m[10]*other.v[2] + m[11]*other.v[3];
	vec.v[3] = m[12]*other.v[0] + m[13]*other.v[1] + m[14]*other.v[2] + m[15]*other.v[3];
	return vec;
}

m4 m4::operator*(const m4& other) {
	m4 mat;
	// M1 row X * M2 column Y = M3[X, Y]
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			int row_base = row*4;
			int index = row_base + column;
			float result = 0;
			for (int subindex = 0; subindex < 4; subindex ++) {
				result += (m[row_base+subindex] * other.m[column+(subindex*4)]);
			}
			mat.m[index] = result;
		}
	}
	return mat;
}

m4& m4::operator=(const m4& other) {
	m[0] = other.m[0];
	m[1] = other.m[1];
	m[2] = other.m[2];
	m[3] = other.m[3];
	m[4] = other.m[4];
	m[5] = other.m[5];
	m[6] = other.m[6];
	m[7] = other.m[7];
	m[8] = other.m[8];
	m[9] = other.m[9];
	m[10] = other.m[10];
	m[11] = other.m[11];
	m[12] = other.m[12];
	m[13] = other.m[13];
	m[14] = other.m[14];
	m[15] = other.m[15];
	return *this;
}

v4 m4::row(const int row) {
	v4 vec;
	int rowInd = row*4;
	vec.v[0] = m[rowInd];
	vec.v[1] = m[rowInd + 1];
	vec.v[2] = m[rowInd + 2];
	vec.v[3] = m[rowInd + 3];
	return vec;
}

v4 m4::col(const int col) {
	v4 vec;
	vec.v[0] = m[col];
	vec.v[0] = m[col + 4];
	vec.v[1] = m[col + 8];
	vec.v[2] = m[col + 12];
	return vec;
}

// Quaternions.
quat::quat() {
	q[0] = q[1] = q[2] = q[3] = 0;
}

// I guess this is technically a versor, but I'm calling these quaternions.
quat::quat(float a, float x, float y, float z) {
	float rads = ang_to_rad(a);
	q[0] = cos(rads/2);
	q[1] = sin(rads/2) * x;
	q[2] = sin(rads/2) * y;
	q[3] = sin(rads/2) * z;
}

quat quat::operator/(float scalar) {
	quat result;
	result.q[0] = q[0] / scalar;
	result.q[1] = q[1] / scalar;
	result.q[2] = q[2] / scalar;
	result.q[3] = q[3] / scalar;
	return result;
}

quat& quat::operator/=(float scalar) {
	q[0] = q[0] / scalar;
	q[1] = q[1] / scalar;
	q[2] = q[2] / scalar;
	q[3] = q[3] / scalar;
	return *this;
}

// Quaternion multiplication. X * Y means to rotate by Y before X.
quat quat::operator*(quat o) {
	quat result;
	result.q[0] = q[0]*o.q[0] - q[1]*o.q[1] - q[2]*o.q[2] - q[3]*o.q[3];
	result.q[1] = q[1]*o.q[0] + q[0]*o.q[1] - q[3]*o.q[2] + q[2]*o.q[3];
	result.q[2] = q[2]*o.q[0] + q[3]*o.q[1] + q[0]*o.q[2] - q[1]*o.q[3];
	result.q[3] = q[3]*o.q[0] - q[2]*o.q[1] + q[1]*o.q[2] + q[0]*o.q[3];
	return result;
}

quat& quat::operator*=(quat& o) {
	q[0] = q[0]*o.q[0] - q[1]*o.q[1] - q[2]*o.q[2] - q[3]*o.q[3];
	q[1] = q[1]*o.q[0] + q[0]*o.q[1] - q[3]*o.q[2] + q[2]*o.q[3];
	q[2] = q[2]*o.q[0] + q[3]*o.q[1] + q[0]*o.q[2] - q[1]*o.q[3];
	q[3] = q[3]*o.q[0] - q[2]*o.q[1] + q[1]*o.q[2] + q[0]*o.q[3];
	return *this;
}

quat& quat::operator=(const quat& other) {
	q[0] = other.q[0];
	q[1] = other.q[1];
	q[2] = other.q[2];
	q[3] = other.q[3];
	return *this;
}

/*
 * Printing functions.
 */
string print(v2 vec) {
	return "[" + to_string(vec.v[0]) + ", " + to_string(vec.v[1]) + "]";
}

string print(v3 vec) {
	return "[" + to_string(vec.v[0]) + ", " + to_string(vec.v[1]) +
		   ", " + to_string(vec.v[2]) + "]";
}

string print(v4 vec) {
	return "[" + to_string(vec.v[0]) + ", " + to_string(vec.v[1]) +
		   ", " + to_string(vec.v[2]) + ", " + to_string(vec.v[3]) + "]";
}

string print(m3 mat) {
	return "[" + to_string(mat.m[0]) + ", " + to_string(mat.m[1]) + ", " + to_string(mat.m[2]) +
		   "\n " + to_string(mat.m[3]) + ", " + to_string(mat.m[4]) + ", " + to_string(mat.m[5]) +
		   "\n " + to_string(mat.m[6]) + ", " + to_string(mat.m[7]) + ", " + to_string(mat.m[8]) + "]";
}

string print(m4 mat) {
	return "[" + to_string(mat.m[0]) + ", " + to_string(mat.m[1]) + ", " + to_string(mat.m[2]) +
		   ", " + to_string(mat.m[3]) + "\n " + to_string(mat.m[4]) + ", " + to_string(mat.m[5]) +
		   ", " + to_string(mat.m[6]) + ", " + to_string(mat.m[7]) + "\n " + to_string(mat.m[8]) +
		   ", " + to_string(mat.m[9]) + ", " + to_string(mat.m[10]) + ", " + to_string(mat.m[11]) +
		   "\n " + to_string(mat.m[12]) + ", " + to_string(mat.m[13]) + ", " + to_string(mat.m[14]) +
		   ", " + to_string(mat.m[15]) + "]";
}

string print(quat q) {
	return "[" + to_string(q.q[0]) + ", " + to_string(q.q[1]) + ", " +
		   to_string(q.q[2]) + ", " + to_string(q.q[3]) + "]";
}

/*
 * Generic math functions.
 */
float ang_to_rad(float a) {
	return a * (PI / 180);
}

float rad_to_ang(float r) {
	return r * (180 / PI);
}

/*
 * 3D math functions.
 */
// Vec3 functions.
float magnitude(v3& vec) {
	return (sqrt(vec.v[0]*vec.v[0] + vec.v[1]*vec.v[1] + vec.v[2]*vec.v[2]));
}

v3 normalize(v3& vec) {
	v3 norm = vec / magnitude(vec);
	return norm;
}

v3 cross(v3& a, v3& b) {
	v3 vec;
	vec.v[0] = (a.v[1]*b.v[2]) - (a.v[2]*b.v[1]);
	vec.v[1] = (a.v[2]*b.v[0]) - (a.v[0]*b.v[2]);
	vec.v[2] = (a.v[0]*b.v[1]) - (a.v[1]*b.v[0]);
	return vec;
}

v3 lerp(v3& vec1, v3& vec2, float prog) {
	return vec1 + (vec2 - vec1) * prog;
}

// 4x4 Matrix functions.
// Identity matrix.
m4 id4() {
	return m4(1, 0, 0, 0,
			  0, 1, 0, 0,
			  0, 0, 1, 0,
			  0, 0, 0, 1);
}

m4 transpose(m4 orig) {
	return m4(orig.m[0], orig.m[4], orig.m[8],  orig.m[12],
			  orig.m[1], orig.m[5], orig.m[9],  orig.m[13],
			  orig.m[2], orig.m[6], orig.m[10], orig.m[14],
			  orig.m[3], orig.m[7], orig.m[11], orig.m[15]);
}

m4 rotate_x(float degrees) {
	float rads = ang_to_rad(degrees);
	return rotate_x_rads(rads);
}

m4 rotate_x_rads(float rads) {
	m4 mat(1, 0, 0, 0,
		   0, cos(rads), -sin(rads), 0,
		   0, sin(rads), cos(rads), 0,
		   0, 0, 0, 1);
	return mat;
}

m4 rotate_y(float degrees) {
	float rads = ang_to_rad(degrees);
	return rotate_y_rads(rads);
}

m4 rotate_y_rads(float rads) {
	m4 mat(cos(rads), 0, sin(rads), 0,
		   0, 1, 0, 0,
		   -sin(rads), 0, cos(rads), 0,
		   0, 0, 0, 1);
	return mat;
}

m4 rotate_z(float degrees) {
	float rads = ang_to_rad(degrees);
	return rotate_z_rads(rads);
}

m4 rotate_z_rads(float rads) {
	m4 mat(cos(rads), -sin(rads), 0, 0,
		   sin(rads), cos(rads), 0, 0,
		   0, 0, 1, 0,
		   0, 0, 0, 1);
   return mat;	
}

m4 translation_matrix(float x, float y, float z) {
	m4 translation(1, 0, 0, x,
				   0, 1, 0, y,
				   0, 0, 1, z,
				   0, 0, 0, 1);
	return translation;
}

/*
 * Create a rotation matrix from a quaternion.
 */
m4 quaternion_to_rotation(quat q) {
	m4 mat = id4();
	// 1 - 2y^2 - 2z^2
	mat.m[0] = 1 - 2*q.q[2]*q.q[2] - 2*q.q[3]*q.q[3];
	// 2xy - 2az
	mat.m[1] = 2*q.q[1]*q.q[2] - 2*q.q[0]*q.q[3];
	// 2xz + 2ay
	mat.m[2] = 2*q.q[1]*q.q[3] + 2*q.q[0]*q.q[2];
	// 2xy + 2az
	mat.m[4] = 2*q.q[1]*q.q[2] + 2*q.q[0]*q.q[3];
	// 1 - 2x^2 - 2z^2
	mat.m[5] = 1 - 2*q.q[1]*q.q[1] - 2*q.q[3]*q.q[3];
	// 2yz - 2az
	mat.m[6] = 2*q.q[2]*q.q[3] - 2*q.q[0]*q.q[1];
	// 2xz - 2ay
	mat.m[8] = 2*q.q[1]*q.q[3] - 2*q.q[0]*q.q[2];
	// 2yz + 2ax
	mat.m[9] = 2*q.q[2]*q.q[3] + 2*q.q[0]*q.q[1];
	// 1 - 2x^2 - 2y^2
	mat.m[10] = 1 - 2*q.q[1]*q.q[1] - 2*q.q[2]*q.q[2];

	return mat;
}

// It's just multiplication, but I'd may as well make a helper for readability.
m4 view_matrix(m4 translation, m4 rotation) {
	return rotation * translation;
}

/*
 * View Matrix:
 *  RX  RY  RZ  -PX
 *  UX  UY  UZ  -PY
 *  -FX -FY -FZ -PZ
 *  0   0   0   1
 *
 *  First, create the translation matrix (only the position 'P' values).
 *  Then construct the rotation matrix (upper-left 3x3).
 *  The view matrix is V = R * T.
 *  Note that 'up' must be udpated as the camera rotates.
 */
m4 look_at(v3 pos, v3 t_pos, v3 up) {
	m4 t_mat = id4();

	// Apply translation.
	t_mat.m[3] = -pos.v[0];
	t_mat.m[7] = -pos.v[1];
	t_mat.m[11] = -pos.v[2];

	// Find right/forwards/up rotational vectors.
	// Distance from camera to target
	v3 dist = t_pos - pos;
	// Forward vector is pointing towards the target.
	v3 f = normalize(dist);
	// R is F x U.
	v3 r = cross(f, up);
	// Recalculate up as R x F.
	v3 u = cross(r, f);
	// Create rotation matrix.
	m4 r_mat(r.v[0], r.v[1], r.v[2], 0,
			 u.v[0], u.v[1], u.v[2], 0,
			 -f.v[0], -f.v[1], -f.v[2], 0,
			 0, 0, 0, 1);

	// R * T to get the view matrix.
	m4 v_mat = r_mat * t_mat;
	return v_mat;
}

/*
 * Alternate view matrix generator:
 * Takes (X, Y, Z) rotation instead of a point to look at.
 * Generate rotational matrix by rotating a +Z-axis unit vector
 * around Y, then X and generating a view matrix for that target.
 * For now, Z rotation will be ignored.
 * Also generate an 'up' vector from a +Y-axis unit fector rotated by Y, then X.
 */
m4 look_at(v3 pos, v3 t_angles) {
	v4 unit_direction(0.0f, 0.0f, 1.0f, 1.0f);
	v4 up_direction(0.0f, 1.0f, 0.0f, 1.0f);
	m4 ry = rotate_y(t_angles.v[1]);
	unit_direction = ry * unit_direction;
	up_direction = ry * up_direction;
	m4 rx = rotate_x(t_angles.v[0]);
	unit_direction = rx * unit_direction;
	up_direction = rx * up_direction;

	v3 t_vec = pos + unit_direction;
	return look_at(pos, t_vec, up_direction);
}

/*
 * Perspective matrix:
 * X 0  0 0
 * 0 Y  0 0
 * 0 0  Z P
 * 0 0 -1 0
 *
 * X = 2 * (near / (range * aspect))
 * Y = near / range
 * Z = -(far + near) / (far - near)
 * P = -(2 * far * near) / (far - near)
 * range = tan(fov * 0.5) * near
 * (aspect_ratio = viewport_width / viewport_height)
 */
m4 perspective(float near, float far, float fov, float aspect_ratio) {
	float fov_rads = ang_to_rad(fov);
	float range = near * tan(fov_rads * 0.5);
	float PX = 2 * (near / (range * aspect_ratio));
	float PY = near / range;
	float PZ = -(far + near) / (far - near);
	float P = -(2 * far * near) / (far - near);

	m4 persp_mat(PX, 0, 0, 0,
				 0, PY, 0, 0,
				 0, 0, PZ, P,
				 0, 0, -1, 0);
	return persp_mat;
}

/*
 * Quaternion functions.
 */
float dot(quat q1, quat q2) {
	return q1.q[0]*q2.q[0] + q1.q[1]*q2.q[1] + q1.q[2]*q2.q[2] + q1.q[3]*q2.q[3];
}

float norm(quat q) {
	return q.q[0]*q.q[0] + q.q[1]*q.q[1] + q.q[2]*q.q[2] + q.q[3]*q.q[3];
}

quat set(float a, v3 vec) {
	quat q;
	float rads = ang_to_rad(a);
	q.q[0] = cos(rads/2);
	q.q[1] = sin(rads/2) * vec.v[0];
	q.q[2] = sin(rads/2) * vec.v[1];
	q.q[3] = sin(rads/2) * vec.v[2];
	return q;
}

quat set(float a, float x, float y, float z) {
	quat q;
	float rads = ang_to_rad(a);
	q.q[0] = cos(rads/2);
	q.q[1] = sin(rads/2) * x;
	q.q[2] = sin(rads/2) * y;
	q.q[3] = sin(rads/2) * z;
	return q;
}

quat normalize(quat q) {
	float len = q.q[0]*q.q[0] + q.q[1]*q.q[1] + q.q[2]*q.q[2] + q.q[3]*q.q[3];
	if (fabs(len) >= 1.0f && fabs(len) <= 1.001f) {
		return q;
	}

	return q / sqrt(len);
}

quat inverse(quat q) {
	return conjugate(q) / norm(q);
}

quat conjugate(quat q) {
	quat result(q.q[0], -q.q[1], -q.q[2], -q.q[3]);
	return result;
}

quat slerp(quat q1, quat q2, float prog) {
	quat result;
	float d = dot(q1, q2);

	// Negate one of the quats if we want to take the longer
	// rounded path.
	if (d < 0.0f) {
		for (int i=0; i<4; i++) {
			q1.q[i] *= -1.0f;
		}
	}

	if (fabs(d) >= 1.0f && fabs(d) <= 1.001f) {
		// The two quaternions are roughly equal.
		return q1;
	}

	// Find the sin(arccos) without using expensive trig functions.
	float sin_o = sqrt(1.0f - d*d);
	if (fabs(sin_o) < 0.001f) {
		// Can't /0, so do linear interpolation.
		for (int i=0; i<4; i++) {
			result.q[i] = (1.0f - prog) * q1.q[i] + prog*q2.q[i];
		}
		return result;
	}

	float o = acos(d);
	float a = sin((1.0f - prog) * o) / sin_o;
	float b = sin(prog * o) / sin_o;
	for (int i=0; i<4; i++) {
		result.q[i] = q1.q[i] * a + q2.q[i] * b;
	}
	return result;
}
