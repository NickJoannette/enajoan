#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <stack>
#include <list>
#include "shader.h"
#include <chrono>
#include <GL/glew.h>
#include "Cube.h"
#include "Sphere.h"
#include "Material.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>


using namespace glm;

class Child {
public:

	Child(Child * parent, bool isLetter, Cube * c, Sphere * uS, GLint * primitiveType, unsigned int textureID1, mat4 matrixOperation, mat4 * transformMatrix = new mat4(1),  unsigned int shadowMapID1 = NULL, unsigned int bubbleTextureID1 = NULL) {
		this->parent = parent;
		this->isLetter = isLetter;
		this->matrixOperation = matrixOperation;
		this->transformMatrix = transformMatrix;
		this->c = c; this->primitiveType = primitiveType;
		this->textureID1 = textureID1;
		this->shadowMapID1 = shadowMapID1;
		this->bubbleTextureID1 = bubbleTextureID1;
		this->uS = uS;
	}

	Child(Child * parent, Sphere * s, GLint * primitiveType, unsigned int textureID1, mat4 matrixOperation, mat4 * transformMatrix = new mat4(1), unsigned int shadowMapID1 = NULL, unsigned int bubbleTextureID1 = NULL) {
		this->parent = parent;
		this->matrixOperation = matrixOperation;
		this->transformMatrix = transformMatrix;
		this->primitiveType = primitiveType;
		this->textureID1 = textureID1;
		this->shadowMapID1 = shadowMapID1;
		this->bubbleTextureID1 = bubbleTextureID1;
		this->s = s;
	}	
	
	Child(Child * parent, Cube * c,  GLint * primitiveType, unsigned int textureID1, mat4 matrixOperation, mat4 * transformMatrix = new mat4(1), unsigned int shadowMapID1 = NULL, unsigned int bubbleTextureID1 = NULL) {
		this->parent = parent;
		this->matrixOperation = matrixOperation;
		this->transformMatrix = transformMatrix;
		this->primitiveType = primitiveType;
		this->textureID1 = textureID1;
		this->shadowMapID1 = shadowMapID1;
		this->c = c; 
		this->bubbleTextureID1 = bubbleTextureID1;
	}

	Child() {

	}

	mat4 matrixOperation = mat4(1.0f);
	mat4 resultMatrix = mat4(1.0f);
	mat4 * transformMatrix = new mat4(1.0f);
	mat4 * worldMatrix = new mat4(1.0f);

	// Bounds for this child
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::lowest();
	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();

	bool isLetter;

	void(*drawFunction)(GLint primitiveType, unsigned int textureID1);
	GLint * primitiveType;
	Cube * c = NULL;
	Sphere * s = NULL, *uS = NULL;
	Child * parent;

	unsigned int textureID1, bubbleTextureID1, shadowMapID1;

	mat4 processMatrix(Shader * S) {
		if (parent == NULL) {
			resultMatrix = *transformMatrix * matrixOperation;
		}
		else {
			resultMatrix = parent->resultMatrix * matrixOperation;
		}

		if (uS != NULL) {
			mat4 originMatrix = inverse(*transformMatrix) * resultMatrix;
			float minX, minY, maxX, maxY;
			float sX = length(originMatrix[0]), sY = length(originMatrix[1]);
			float tX = originMatrix[3].x, tY = originMatrix[3].y;
			minX = tX - sX * 0.5f;
			minY = tY - sY * 0.5f;
			maxX = tX + sX * 0.5f;
			maxY = tY + sY * 0.5f;

			if (minX < this->minX) this->minX = minX;
			if (maxX > this->maxX) this->maxX = maxX;

			if (minY < this->minY) this->minY = minY;
			if (maxY > this->maxY) this->maxY = maxY;
		}
		return resultMatrix;
	}

	mat4 processOutlineMatrix(Shader * S) {
		mat4 scaledTransform = *transformMatrix;

		if (parent == NULL) {
			resultMatrix = scaledTransform * matrixOperation;
		}
		else {
			resultMatrix = parent->resultMatrix * matrixOperation;
		}
		return resultMatrix;

	}

	void printBounds() {

		std::cout << " -------------------- CHILD ----------------- " << std::endl;
		std::cout << "X Bounds: [" << minX << " , " << maxX << "]" << std::endl;
		std::cout << "Y Bounds: [" << minY << " , " << maxY << "]" << std::endl;
	}

	void draw() {
		if (c == NULL) s->Draw(*primitiveType, textureID1);
		else c->Draw(*primitiveType, textureID1);
	}
};











class HierarchicalModel
{ 
public:
	
	// Creating this struct so we can do quick ray-sphere intersection checks and select the models
	struct bubbleSphere {
		float rad;
		vec3 center;
	};

	HierarchicalModel(Child * parent, mat4 * worldMatrix, bool useDiffuse, Material M) {
		this->M = M;
		this->useDiffuse = useDiffuse;
		this->worldMatrix = worldMatrix;
		
		parent->worldMatrix = worldMatrix;
		children.push_back(parent);
	};

	void setChild(Child * C){
		C->worldMatrix = worldMatrix;
		children.push_back(C);
	}

	void clear() {
		children.clear();
	}

	void Draw(Shader * cS, Shader * sS, mat4 worldMatrix, bool spheresDisabled = false) {

		cS->use();
		for (Child * C : children) {

			cS->setMat4("model", worldMatrix * C->processMatrix(cS));
			cS->setMaterial(M);
			C->draw();
			updateBounds(C);

			if (C == children.back() && !spheresDisabled) {
				if (C->uS != NULL) {
					// Enable alpha transparency
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

					// Now that the children have been drawn, draw an appropriately sized/placed sphere
					radius = abs(maxX - minX) / 2.0f + 1.0;

					mat4 sphereTransform = worldMatrix *( *C->transformMatrix * *C->uS->getTransformMatrix() *
						translate(vec3((maxX + minX) / 2.0f, (minY + maxY) / 2.0f + radius, 0)) * scale(vec3(radius)));
				
					sS->use();
					sS->setMat4("model",  sphereTransform);
					sS->setBool("useDiffuse", true);
					sS->setMaterial("baseMaterial",M);
					bubble.center = vec3(sphereTransform[3]);
					bubble.rad = radius * length(vec3((*C->transformMatrix)[0]));
					C->uS->Draw(*C->primitiveType, C->bubbleTextureID1 == NULL ? C->textureID1 : C->bubbleTextureID1);

					// Disable alpha transparency
					glDisable(GL_BLEND);
				}
				else if (C->s != NULL) {

					sS->use();
					sS->setMat4("model",worldMatrix * *C->transformMatrix);
					sS->setBool("isLightSource", true);
					bubble.center = vec3((*C->transformMatrix)[3]);
					bubble.rad = 1.0f;
					C->s->Draw(*C->primitiveType, C->bubbleTextureID1 == NULL ? C->textureID1 : C->bubbleTextureID1);
					sS->setBool("isLightSource", false);

				}
			}
	
		}
	}

	void DrawOutline(Shader * cS, Shader * sS, mat4 worldMatrix) {
		cS->use();
		for (Child * C : children) {
			cS->setMat4("model", worldMatrix *C->processOutlineMatrix(cS));

			if (C->c != NULL) C->draw();

			if (C == children.back()) {
				if (C->uS != NULL) {

					// Now that the children have been drawn, draw an appropriately sized/placed sphere
					if (radius == NULL) radius = abs(maxX - minX) / 2.0f + 1.0f;

					mat4 scaledTransform = worldMatrix * *C->transformMatrix;
					sS->use();
					mat4 sphereTransform = scaledTransform * *C->uS->getTransformMatrix() * translate(vec3((maxX + minX) / 2.0f, (minY + maxY) / 2.0f + radius, 0)) * scale(vec3(radius));
					sS->setMat4("model", sphereTransform);
					C->uS->Draw(*C->primitiveType, C->bubbleTextureID1 == NULL ? C->textureID1 : C->bubbleTextureID1);
				}
				else {
					sS->use();
					sS->setMat4("model",  *C->transformMatrix);
					sS->setBool("isLightSource", true);
					bubble.center = vec3((*C->transformMatrix)[3]);
					bubble.rad = 1.0f;
					C->s->Draw(*C->primitiveType, C->bubbleTextureID1 == NULL ? C->textureID1 : C->bubbleTextureID1);
					sS->setBool("isLightSource", false);
				}
			}
		}
	}

	float radius = NULL;

	void updateBounds(Child * C) {
		if (C->minX < this->minX) this->minX = C->minX;
		if (C->maxX > this->maxX) this->maxX = C->maxX;
		if (C->minY < this->minY) this->minY = C->minY;
		if (C->maxY > this->maxY) this->maxY = C->maxY;
	}

	void printBounds() {

		std::cout << " -------------------- HIERARCHICAL MODEL --------------------- " << std::endl;
		std::cout << "X Bounds: [" << minX << " , " << maxX << "]" << std::endl;
		std::cout << "Y Bounds: [" << minY << " , " << maxY << "]" << std::endl;
	}

	unsigned int numNodes() { return children.size(); }

	Child * last() {
		auto ri = children.rbegin();
		return *ri;
	}


	Child * skip_back(unsigned int back) {
		auto ri = children.rbegin();
		for (int i = 0; i < back; ++i) ri ++;
		return *ri;
	}

	Child * base() {
		auto i = children.begin();
		return (*i);
	}

	Child temp1, temp2;

	// Bounds for the entire hierarchical model
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::lowest();
	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	bubbleSphere bubble;
	Material M = Material(19125, 0.0735, 0.0225, 0.7038, 0.27048, 0.0828, 0.256777, 0.137622, 0.086014, 0.1);
	bool useDiffuse;
	mat4 * worldMatrix;
	HierarchicalModel();

private:

	std::list<Child *> children;


};

