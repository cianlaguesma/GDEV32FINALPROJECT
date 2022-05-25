#version 330

layout(location = 0) in vec3 aPos;

uniform mat4 transformationMatrix;
uniform mat4 view;
uniform mat4 projection;

void main() {

	gl_Position = projection * view * transformationMatrix * vec4(aPos, 1.0);
}