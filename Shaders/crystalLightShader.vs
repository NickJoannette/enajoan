#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;


out vec3 FragPos;
out vec3 normal;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform float time;



void main()
{


   
   FragPos = vec3(model * vec4(0.1*aPos, 1.0));
   normal = mat3(transpose(inverse(model))) * aNorm;  
   gl_Position =  projection * view * model * vec4(aPos, 1.0);
	
   texCoord = aTexCoord;

};