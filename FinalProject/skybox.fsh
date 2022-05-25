#version 330

out vec4 FragColor;

in vec3 texCoords;
in vec3 outVertexPos;
in vec3 outVertexNormal;

uniform samplerCube skybox;
uniform vec3 cameraPos;

void main()
{
    vec3 I = normalize(outVertexPos-cameraPos);
    vec3 R = reflect(I, normalize(outVertexNormal));
    FragColor = texture(skybox, texCoords);

}