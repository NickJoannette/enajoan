#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; 
layout (location = 2) in vec2 aTexCoord;

out VS_OUT {
    vec3 normal;
	vec3 color;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float time;

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = vec3(projection * vec4(normalMatrix * aNormal, 0.0));
    gl_Position = projection * view * model * vec4(aPos +vec3(50*sin(time*0.05),200*cos(0.075*time),0), 1.0); 
	vec4 PMPosition = projection * model * vec4(aPos,1.0);
	vs_out.color = vec3(sin(0.1*PMPosition.y),1,1-cos(0.4*PMPosition.x));
}