#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;

};

struct Light{

	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

};

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;



uniform bool useTexture;

void main()
{
	// ambient
  vec3 ambient = light.ambient * material.ambient;


  // diffuse
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse *( material.diffuse * light.diffuse);

  //specular
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 36);
  vec3 specular = material.specular * spec *light.specular;
  
  if(useTexture == true){
	FragColor = vec4((ambient + diffuse+ specular),1.0f) * texture(texture1, TexCoord);
	return;
  }

  vec3 result = (ambient + diffuse + specular) * objectColor;
  FragColor = vec4(result, 1.0f);

}