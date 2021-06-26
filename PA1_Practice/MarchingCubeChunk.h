#pragma once

#include <vector>
#include <glm/gtx/transform.hpp>
#include<glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <iostream>
#include "Material.h"
#include "stb_image.h"
#include "Shader.h"

using namespace glm;

class MarchingCubeChunk
{
public:

	struct face {
		vec3 v1, v2, v3, normal;
	};


	MarchingCubeChunk() {  };

	~MarchingCubeChunk() {};

	void Draw(GLint primitiveType, unsigned int diffuseMap, unsigned int depthCubeMap) {
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);

		glDrawArrays(primitiveType, 0, vertexCount);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);

	}

	// Variables for rendering
	unsigned int vbo;
	unsigned int vao;
	unsigned int vertexCount;
	const uint8_t xMag = 6;
	const uint8_t yMag = 6;
	const uint8_t zMag = 6;
	face * faces;
	vec3 center;
	unsigned char functionValues[6][6][6][8];
	unsigned char cubeIndexes[6][6][6][1];
	bool firstGeneration = true;

	// CRYSTALS
	std::vector<mat4> crystalTransforms;
	std::vector<vec3> crystalPositions; 
	std::vector<Shader::PointLight> crystalLights;
	vec3 crystalFaceNormal = vec3(0.0f);
	vec3 crystalPosition  = vec3(0.0f);


private:


};


