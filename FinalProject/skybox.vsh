#version 330

layout(location = 0) in vec3 aPos;

out vec3 tex_coords;

uniform mat4 view;
uniform mat4 projection;

void main() {
	tex_coords = vec3(aPos) * vec3(1, 1, 1);
	gl_Position = (projection * view * vec4(aPos, 1.0)).xyww;
}