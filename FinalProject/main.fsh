#version 330

// UV-coordinate of the fragment (interpolated by the rasterization stage)
in vec2 outUV;

// Color of the fragment received from the vertex shader (interpolated by the rasterization stage)
in vec3 outColor;

//Normal
in vec3 outNormal;

//pos

in vec3 outVertexPos;

// Final color of the fragment that will be rendered on the screen
out vec4 fragColor;

// Texture unit of the texture
uniform sampler2D tex;

uniform vec3 lightPos;
uniform vec3 cameraPos;

struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct DirectionalLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

};

struct PointLight {
	vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectionalLight light;
uniform PointLight plight;

uniform Material material;

vec3 CalcDirLight(DirectionalLight light)
{
	vec3 lightDir = normalize(-light.direction);
	vec3 viewDir = normalize(cameraPos - outVertexPos);
	vec3 normal = normalize(outNormal);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	vec3 ambient = light.ambient * material.ambient;
	vec3 diffuse = light.diffuse  * (diff * material.diffuse);
	vec3 specular = light.specular  * (spec * material.specular);

	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light) {
	vec3 lightDir = normalize(light.position - outVertexPos);
	vec3 viewDir = normalize(cameraPos - outVertexPos);
	vec3 normal = normalize(outNormal);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	float distance    = length(light.position - outVertexPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance)); 

	
	vec3 ambient = light.ambient * material.ambient;
	vec3 diffuse = light.diffuse  * (diff * material.diffuse);
	vec3 specular = light.specular  * (spec * material.specular);

	ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

	return (ambient + diffuse + specular);
}


void main()
{
	vec3 dresult = CalcDirLight(light);
	vec3 presult = CalcPointLight(plight);
	vec3 result = dresult+presult;
	vec3 newColor = outColor;
	fragColor = texture(tex, outUV) *(vec4(result,1.f));

}
