#version 330

// UV-coordinate of the fragment (interpolated by the rasterization stage)
in vec2 outUV;

// Color of the fragment received from the vertex shader (interpolated by the rasterization stage)
in vec3 outColor;

// Final color of the fragment that will be rendered on the screen
out vec4 fragColor;

// Texture unit of the texture
uniform sampler2D tex;

uniform vec3 objectColor;

void main()
{
    vec3 newColor = outColor;
    vec3 color = glm::vec3(1.f,1.f,1.f);
    //fragColor = vec4(newColor, 1.0);
    fragColor = texture(tex, outUV) * vec4(color,1.0);
}
