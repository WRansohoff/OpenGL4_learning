#version 420

// Surface properties.
vec3 Ks = vec3(1.0, 1.0, 1.0);
vec3 Kd = vec3(1.0, 1.0, 1.0);
vec3 Ka = vec3(1.0, 1.0, 1.0);
// specular power.
float specular_exponent = 100.0;

//uniform mat4 view, proj;
layout (std140) uniform cam_ubo {
	mat4 V;
	mat4 P;
};
// World space coordinates; note that incoming values are in eye/camera space.
//uniform vec3 light_pos_W, light2_pos_W;
// Specula, diffuse, ambient light values.
//uniform vec3 Ls, Ld, La;
//uniform vec3 Ls2, Ld2, La2;
// std140 uses 4-float memory blocks, so use vec4s wherever possible.
layout (std140) uniform lights_ubo {
	vec4 light_pos_W;
	vec4 light2_pos_W;
	vec4 Ls;
	vec4 Ld;
	vec4 La;
	vec4 Ls2;
	vec4 Ld2;
	vec4 La2;
};

// Texture sampler. Use texture unit 0 - no need to glUniform this one.
layout(binding = 0) uniform sampler2D texture_sampler;

in vec3 pos_E, norm_E;
in vec2 tex_coords;
out vec4 frag_color;

void main() {
	// Lighting calculations.
	// Bring light position to eye space.
    vec3 light_pos_E = vec3(V * light_pos_W);
	vec3 dist_to_light_E = light_pos_E - pos_E;
	vec3 dir_to_light_E = normalize(dist_to_light_E);
    vec3 light2_pos_E = vec3(V * light2_pos_W);
	vec3 dist_to_light2_E = light2_pos_E - pos_E;
	vec3 dir_to_light2_E = normalize(dist_to_light2_E);

	// Ambient intensity.
	vec3 Ia = La.xyz * Ka;
	vec3 Ia2 = La2.xyz * Ka;

	// Diffuse.
	float diffuse_dot = dot(dir_to_light_E, norm_E);
	diffuse_dot = max(diffuse_dot, 0.0);
	vec3 Id = Ld.xyz * Kd * diffuse_dot;
	float diffuse_dot2 = dot(dir_to_light2_E, norm_E);
	diffuse_dot2 = max(diffuse_dot2, 0.0);
	vec3 Id2 = Ld2.xyz * Kd * diffuse_dot2;

	// Specular.
	vec3 to_surface = normalize(-pos_E);

	// Phong specular calculations.
	/*
	vec3 reflection = reflect(-dir_to_light_E, norm_E);
	float specular_dot = dot(reflection, to_surface);
	specular_dot = max(specular_dot, 0.0);
	*/

	// Blinn-Phong specular calculations.
	vec3 half_way = normalize(to_surface + dir_to_light_E);
	float specular_dot = dot(half_way, norm_E);
	specular_dot = max(specular_dot, 0.0);
	float specular_factor = pow(specular_dot, specular_exponent);
	vec3 Is = Ls.xyz * Ks * specular_factor;
	vec3 half_way2 = normalize(to_surface + dir_to_light2_E);
	float specular_dot2 = dot(half_way2, norm_E);
	specular_dot2 = max(specular_dot2, 0.0);
	float specular_factor2 = pow(specular_dot2, specular_exponent);
	vec3 Is2 = Ls2.xyz * Ks * specular_factor2;

	// Texture sampling.
	vec4 texel = texture(texture_sampler, tex_coords);

	frag_color = normalize(texel + vec4((Ia+Ia2) + (Id+Id2) + (Is+Is2), 1.0));
}
