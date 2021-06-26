#version 330 core


///////////////////////////////////////// OUTPUTS
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
////////////////////////////////////////////////


in vec3 fColor;

uniform float time;
void main()
{
    FragColor = vec4(0.85,0.85,1.0,1.0);
	BrightColor = vec4(FragColor.rgb,1.0);
}
