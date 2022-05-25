#version 330

layout(location = 0) in vec3 vertexPos;
layout(location = 3) in vec3 vertexNormal;
out vec3 texCoords;
out vec3 outVertexPos;
out vec3 outVertexNormal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() {
	texCoords = vec3(vertexPos) * vec3(1, 1, 1);

	outVertexNormal = mat3(transpose(inverse(model))) * vertexNormal;
	outVertexPos = vec3(model * vec4(vertexPos, 1.0));

	gl_Position = (projection * view * vec4(vertexPos, 1.0)).xyww;
}