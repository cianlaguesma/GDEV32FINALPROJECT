#version 330 
layout(location = 0) in vec3 aPos;
layout(location = 3) in vec3 aNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 transformationMatrix;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Normal = mat3(transpose(inverse(transformationMatrix))) * aNormal;
    Position = vec3(transformationMatrix * vec4(aPos, 1.0));
    gl_Position = projection * view * transformationMatrix * vec4(aPos, 1.0);
}