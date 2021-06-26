#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

const int MAX_BONES = 5;

uniform int numBones;



out vec3 FragPos;
out vec3 normal;
out vec2 texCoord;
out vec3 PMT;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform float time;



uniform bool animatedModel;
uniform mat4 transform;


uniform mat4 gBones[MAX_BONES];






#define NUM_OCTAVES 5
float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }


float noise(vec2 x) {
	vec2 i = floor(x);
	vec2 f = fract(x);

	// Four corners in 2D of a tile
	float a = hash(i);
	float b = hash(i + vec2(1.0, 0.0));
	float c = hash(i + vec2(0.0, 1.0));
	float d = hash(i + vec2(1.0, 1.0));

	// Simple 2D lerp using smoothstep envelope between the values.
	// return vec3(mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
	//			mix(c, d, smoothstep(0.0, 1.0, f.x)),
	//			smoothstep(0.0, 1.0, f.y)));

	// Same code, with the clamps in smoothstep and common subexpressions
	// optimized away.
	vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float fbm(vec2 x) {
	float v = 0.0;
	float a = 0.5;
	vec2 shift = vec2(100);
	// Rotate to reduce axial bias
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));
	for (int i = 0; i < NUM_OCTAVES; ++i) {
		v += a * noise(x);
		x = rot * x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}






























void main()
{
int numBones =4;
mat4 BoneTransform;
if (animatedModel)  {
    BoneTransform = gBones[BoneIDs[0]] * Weights[0];
	for (int nb=1;nb<numBones;nb++) {
    BoneTransform += gBones[BoneIDs[nb]] * Weights[nb];
  // BoneTransform += gBones[BoneIDs[2]] * Weights[2];
  //  BoneTransform += gBones[BoneIDs[3]] * Weights[3];
  }
}

   vec3 editedPos = 
   (animatedModel ? vec3(BoneTransform * vec4(aPos,1.0)) : aPos);
   
   PMT = vec3(model*vec4(0.1*editedPos,1.0));
   
   FragPos = vec3(model * vec4(0.1*editedPos, 1.0));
   normal = mat3(transpose(inverse(model))) * aNorm;  
 if (animatedModel)
 gl_Position = projection * view * model * transform * vec4(editedPos,1.0);
 else gl_Position =  projection * view * model * vec4(editedPos, 1.0);
	
	// Pass out variables
	texCoord = aTexCoord;

};