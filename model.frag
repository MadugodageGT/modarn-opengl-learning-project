#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D generated_texture;


void main()
{    
    //FragColor = texture(texture_diffuse1, TexCoords);
    vec4 texColor = texture(generated_texture, TexCoords);

    if(texColor.a < 0.1)
        discard;
    FragColor = texColor;
}