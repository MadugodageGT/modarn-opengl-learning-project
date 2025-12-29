#version 330 core

out vec4 FragColor;



in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;


uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;


uniform bool useTexture;

void main()
{
	// ambient
  float ambientStrength = 0.3;
  vec3 ambient = ambientStrength * lightColor;

  // diffuse
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;
  
  if(useTexture == true){
	FragColor = vec4((ambient + diffuse),1.0f) * texture(texture1, TexCoord);
	return;
  }

  vec3 result = (ambient + diffuse) * objectColor;
  FragColor = vec4(result, 1.0f);

}