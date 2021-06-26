#version 330 core

///////////////////////////////////////// LAYOUT INPUTS
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

const int MAX_BONES = 16;
uniform mat4 gBones[MAX_BONES];


///////////////////////////////////////// OUTPUTS
out vec3 normal;
out vec2 texCoord;
out vec3 FragPos;

///////////////////////////////////////// UNIFORMS
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform bool animatedModel;


void main()
{


int numBones = 5;
mat4 BoneTransform;
    BoneTransform = gBones[BoneIDs[0]] * Weights[0];
	for (int nb=1;nb<numBones;nb++) {
    BoneTransform += gBones[BoneIDs[nb]] * Weights[nb];
  }

  ///////////////////////////////////////// SEND OUT THE NORMAL
  normal = normalize(mat3(transpose(inverse(model))) * aNorm);  
  
  ///////////////////////////////////////// SEND OUT THE GL_POSITION
  gl_Position =  projection * view * model * (animatedModel ? BoneTransform : mat4(1)) * vec4(aPos, 1.0);

  FragPos = vec3(model * vec4(aPos,1.0));
  ///////////////////////////////////////// SEND OUT THE TEXTURE COORDINATE
  texCoord = aTexCoord;

};