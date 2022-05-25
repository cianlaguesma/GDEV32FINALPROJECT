#version 330

out vec4 FragColor;

in vec3 texCoords;
in vec3 outVertexPos;
in vec3 outVertexNormal;

uniform samplerCube skybox;
uniform vec3 cameraPos;

void main()
{
    vec3 viewDirVec = normalize(outVertexPos-cameraPos);
    vec3 refVec = reflect(viewDirVec, normalize(outVertexNormal));
    FragColor = texture(skybox, texCoords);

}