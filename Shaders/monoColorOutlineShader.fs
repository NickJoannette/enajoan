#version 330 core
out vec4 FragColor;
  
uniform float time;
uniform vec3 ourColor; // we set this variable in the OpenGL code.
uniform bool isLightSource;

in vec3 modelPos;

void main()
{
    FragColor = vec4(1.0);//vec4(vec3(0.1 + 0.5*abs(cos(time* 0.1 * modelPos.y)), 0, 0.25),1);
	if (isLightSource) FragColor = vec4(vec3(0.6*modelPos.y,0.5*abs(sin(modelPos.y)),0), 1.0);
}  
