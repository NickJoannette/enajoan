#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include "shader.h"
#include <chrono>
#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>

class Cube
{

public:

#pragma region cube buffer objects
	unsigned int cubeVBO;
	unsigned int cubeVAO;
	unsigned int cubeEBO;


	glm::mat4 cubeTransform = glm::mat4(1.0f);
	glm::mat4 cubeModel = glm::mat4(1.0f);

	Cube() {
		cubeModel = glm::mat4(1);
		glGenBuffers(1, &cubeVBO);
		glGenVertexArrays(1, &cubeVAO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 288, cubeVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);
	}

	glm::mat4 * getTransformMatrix() { return &cubeTransform; }
	glm::mat4 * getModelMatrix() { return &cubeModel; }
	void setTransform(glm::mat4 newTransform) { cubeTransform = newTransform; }
	void resetTransform() { cubeTransform = glm::mat4(1.0f); }

	void Draw(GLint primitiveType, unsigned int diffuseMap) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		// to define the cube vertices this way.
		glDrawArrays(primitiveType, 0, 36);

		glBindTexture(GL_TEXTURE_2D, 0);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
	}


	void Draw(GLint primitiveType) {
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glDrawArrays(primitiveType, 0, 36);

		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);

	}

	void DrawIndexed(GLint primitiveType, std::vector<unsigned int> indices) {
		glGenBuffers(1, &cubeEBO);

		// Bind the vertex array to signal to OpenGL that we are referring to it.
		glBindVertexArray(cubeVAO);
		// Bind the EBO and pass in the indices.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * (sizeof(unsigned int)), indices.data(), GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
			glDrawElements(primitiveType, indices.size(), GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
		// Not sure if these two calls below are necessary but I hear they're good practice
		glBindVertexArray(0);


	}
#pragma endregion cube

	~Cube();

private:

	// set up the vertices for the cube
	// ------------------------------------------------------------------
	float cubeVertices[288] = {

		-0.5f,-0.5f,-0.5f, -1.0f,-1.0f,-1.0f, 0.0f, 0.0f, //left - red
		-0.5f,-0.5f, 0.5f, -1.0f,-1.0f,1.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, -1.0f,1.0f,1.0f, 1.0f, 1.0f,

		-0.5f,-0.5f,-0.5f,-1.0f,-1.0f,-1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f,-1.0f,1.0f,1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f,-0.5f,-1.0f,1.0f,-1.0f, 1.0f, 0.0f,




		 0.5f,0.5f,-0.5f, 1.0f,1.0f,-1.0f, 1.0f, 1.0f, // far - blue
		-0.5f,-0.5f,-0.5f, -1.0f,-1.0f,-1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f,-0.5f, -1.0f,1.0f,-1.0f, 0.0f, 1.0f,

		0.5f, 0.5f,-0.5f,  1.0f,1.0f,-1.0f,1.0f, 1.0f,
		0.5f,-0.5f,-0.5f,  1.0f,-1.0f,-1.0f,1.0f, 0.0f,
		-0.5f,-0.5f,-0.5f, -1.0f,-1.0f,-1.0f,0.0f, 0.0f,

		0.5f,-0.5f, 0.5f,  1.0f,-1.0f,1.0f,1.0f, 1.0f, // bottom - turquoise
		-0.5f,-0.5f,-0.5f, -1.0f,-1.0f,-1.0f,0.0f, 0.0f,
		0.5f,-0.5f,-0.5f,  1.0f,-1.0f,-1.0f,1.0f, 0.0f,

		0.5f,-0.5f, 0.5f,  1.0f,-1.0f,1.0f,1.0f, 1.0f,
		-0.5f,-0.5f, 0.5f, -1.0f,-1.0f,1.0f,0.0f, 1.0f,
		-0.5f,-0.5f,-0.5f, -1.0f,-1.0f,-1.0f,0.0f, 0.0f,

		-0.5f, 0.5f, 0.5f,  -1.0f,1.0f,1.0f,0.0f, 1.0f, // near - green
		-0.5f,-0.5f, 0.5f,  -1.0f,-1.0f,1.0f,0.0f, 0.0f,
		0.5f,-0.5f, 0.5f,   1.0f,-1.0f,1.0f,1.0f, 0.0f,

		0.5f, 0.5f, 0.5f,  1.0f,1.0f,1.0f,1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, -1.0f,1.0f,1.0f,0.0f, 1.0f,
		0.5f,-0.5f, 0.5f,  1.0f,-1.0f,1.0f,1.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f, // right - purple
		0.5f,-0.5f,-0.5f, 1.0f,-1.0f,-1.0f,0.0f, 0.0f,
		0.5f, 0.5f,-0.5f, 1.0f,1.0f,-1.0f,1.0f, 0.0f,

		0.5f,-0.5f,-0.5f, 1.0f,-1.0f,-1.0f,0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,
		0.5f,-0.5f, 0.5f, 1.0f,-1.0f,1.0f,0.0f, 1.0f,

		0.5f, 0.5f, 0.5f,  1.0f,1.0f,1.0f,1.0f, 1.0f, // top - yellow
		0.5f, 0.5f,-0.5f,  1.0f,1.0f,-1.0f,1.0f, 0.0f,
		-0.5f, 0.5f,-0.5f, -1.0f,1.0f,-1.0f,0.0f, 0.0f,

		0.5f, 0.5f, 0.5f,  1.0f,1.0f,1.0f,1.0f, 1.0f,
		-0.5f, 0.5f,-0.5f, -1.0f,1.0f,-1.0f,0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f,1.0f,1.0f,0.0f, 1.0f
	};

};
