#version 330

// Vertex position
layout(location = 0) in vec3 vertexPosition;

// Vertex color
layout(location = 1) in vec3 vertexColor;

// Vertex UV coordinate
layout(location = 2) in vec2 vertexUV;

//NORMAL
layout(location = 3) in vec3 vertexNormal;

// UV coordinate (will be passed to the fragment shader)
out vec2 outUV;

// Color (will be passed to the fragment shader)
out vec3 outColor;

//Normal
out vec3 outNormal;

//shadowmap
out vec4 fragPosLCSpace;
//vertexpos

out vec3 outVertexPos;
uniform mat4 lightProjection;
uniform mat4 view, projection, transformationMatrix;



void main()
{


	vec4 newPosition = vec4(vertexPosition, 1.0);
	newPosition = projection * view * transformationMatrix * newPosition;
	gl_Position = newPosition;

	outVertexPos = vec3(transformationMatrix * vec4(vertexPosition, 1.0));
	outUV = vertexUV;
	outColor = vertexColor;
	outNormal = mat3(transpose(inverse(transformationMatrix))) * vertexNormal;


	fragPosLCSpace = lightProjection * vec4(outVertexPos, 1.0);
}
