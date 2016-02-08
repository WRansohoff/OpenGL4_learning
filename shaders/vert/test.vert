#version 420

layout(location = 0) in vec3 vp;
layout(location = 1) in vec3 vn;
layout(location = 2) in vec2 vt;

//uniform mat4 view, proj;
layout (std140) uniform cam_ubo {
	mat4 V;
	mat4 P;
};

out vec3 pos_E, norm_E;
out vec2 tex_coords;

void main() {
	pos_E = vec3(V * vec4(vp, 1.0));
	norm_E = vec3(V * vec4(vn, 0.0));
	tex_coords = vt;
	gl_Position = P * vec4(pos_E, 1.0);
}
