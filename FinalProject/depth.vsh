#version 330

// Vertex position
layout(location = 0) in vec3 vertexPosition;


uniform mat4 orthoProjection, dirLightViewMatrix, model;
void main() {
	gl_Position = orthoProjection * dirLightViewMatrix * model * vec4(vertexPosition, 1.0);
}