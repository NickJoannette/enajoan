#version 330 core
layout (triangles) in;
layout (points, max_vertices = 6) out;
uniform float time; 

in VS_OUT {
    vec3 normal;
	vec3 color;
} gs_in[];

const float MAGNITUDE = 1.0;
out vec3 fColor;
void GenerateLine(int index)
{
	fColor = vec3(1,0,0);
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
	
}