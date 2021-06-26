#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 0) in vec3 aNorm;
layout (location = 0) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

uniform float time;
uniform bool isLightSource;

out vec2 texCoord;
out vec3 normal;
out vec3 FragPos;
out vec3 modelPos;

void main()
{
   vec3 vertPos = aPos;
   if (isLightSource) {
   vertPos.x += 0.17 * sin(time*vertPos.x / vertPos.y);
   vertPos.y += 0.22 * cos(time*vertPos.y /-vertPos.x);
   vertPos.z += 0.15 * cos(time*vertPos.y /vertPos.x);
   } 
   
   
   FragPos = vec3( model * vec4(vertPos, 1.0));
   texCoord = aTexCoord;
   normal = mat3(transpose(inverse(model))) * aNorm;   
   gl_Position = projection * view * model * vec4(vertPos + aNorm * (isLightSource ? .20 : .04), 1.0);
   
   	vec4 PMT = projection*model*vec4(aPos,1.0);
	modelPos = PMT.xyz;
};
