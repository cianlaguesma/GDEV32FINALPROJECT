#version 330

out vec4 FragColor;

in vec3 outVertexPos;
in vec3 outVertexNormal;

uniform samplerCube skybox;
uniform sampler2D bump;

uniform vec3 cameraPos;

void main()
{
    vec3 viewDirVec = normalize(outVertexPos-cameraPos);
    vec3 refVec = reflect(viewDirVec, normalize(outVertexNormal));
    FragColor = vec4(texture(skybox,refVec).rgb,1.0) * texture(bump,refVec.zz);

}