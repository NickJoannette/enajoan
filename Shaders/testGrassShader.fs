#version 330 core
out vec4 FragColor;

in vec3 fColor;

in GS_OUT {
    vec3 color;

} fs_in;

uniform float time;
void main()
{
	vec3 col = fs_in.color;
    FragColor = vec4(col,1.0);
}
