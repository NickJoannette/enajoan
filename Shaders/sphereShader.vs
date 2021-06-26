#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;


out vec3 FragPos;
out vec2 texCoord;
out vec3 vertPos;
out vec3 normal;

void main()
{
   vec3 testPosition = aPos;
   

   FragPos = vec3(model * vec4(aPos, 1.0));

  normal = normalize(mat3(transpose(inverse(model))) * aNorm);  
  
   texCoord = aTexCoord;

   gl_Position = projection * view  * model * vec4(aPos, 1.0);

	vec4 PMT = projection*model*vec4(aPos,1.0);
	vertPos = PMT.xyz;
};