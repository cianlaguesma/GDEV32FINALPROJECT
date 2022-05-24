
#version 330

// Vertex position
layout(location = 0) in vec3 vertexPosition;

// Vertex color
layout(location = 1) in vec3 vertexColor;

// Vertex UV coordinate
layout(location = 2) in vec2 vertexUV;

// UV coordinate (will be passed to the fragment shader)
out vec2 outUV;

// Color (will be passed to the fragment shader)
out vec3 outColor;

uniform mat4 view, projection, transformationMatrix;

void main()
{
    //x = originX + cos(angle)*radius;
    // y = originY + sin(angle) * radius;

    vec4 newPosition = vec4(vertexPosition,1.0);
    newPosition = projection * view * transformationMatrix * newPosition;
    gl_Position = newPosition;
    outUV = vertexUV;
    outColor = vertexColor;
}
