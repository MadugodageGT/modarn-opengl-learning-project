#version 330 core


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out	vec3 ourColor;	

uniform float time;

void main()
{

	float animatedX = aPos.x * sin(time) + aPos.x;
	float animatedY = aPos.y * sin(time) + aPos.y;


   gl_Position = vec4(animatedX, animatedY, aPos.z, 1.0);
   ourColor = aColor;
};
