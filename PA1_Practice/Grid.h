#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include "shader.h"
#include <chrono>
#include <GL/glew.h>
#include <string>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>

class Grid
{
public:

	Grid() {  };
	Grid(unsigned int w, unsigned int l, bool mapYValuesToImage, std::string path, bool alpha);
	void Draw(GLint primitiveType, unsigned int diffuseTexture, unsigned int specularMap, unsigned int shadowMap);

	glm::mat4 * getTransformMatrix() { return &gridTransform; }
	glm::mat4 * getModelMatrix() { return &gridModel; }
	void setTransform(glm::mat4 newTransform) { gridTransform = newTransform; }
	void resetTransform() { gridTransform = glm::mat4(1.0f); }

	~Grid();

private:

	// Variables for rendering
	unsigned int ebo;
	unsigned int vbo;
	unsigned int vao;

	GLuint * indices;
	float * vertices;

	// Geometric variables
	unsigned int width, length;
	glm::mat4 gridModel = glm::mat4(1.0f);
	glm::mat4 gridTransform = glm::mat4(1.0f);
	bool flat;
};

