#version 410
layout(location = 0) in vec3 vp;
layout(location = 1) in vec3 vn;

uniform mat4 view, proj;

out vec3 pos_E, norm_E;

void main() {
	pos_E = vec3(view * vec4(vp, 1.0));
	norm_E = vec3(view * vec4(vn, 0.0));
	gl_Position = proj * vec4(pos_E, 1.0);
}
