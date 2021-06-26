#pragma once
#include <glm/glm.hpp>

// Simple material class for the shaders
class Material {

private:

	float ambient_r;
	float ambient_g;
	float ambient_b;

	float diffuse_r;
	float diffuse_g;
	float diffuse_b;

	float specular_r;
	float specular_g;
	float specular_b;

	float shininess;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;


public:

	Material(float ar, float ag, float ab, float dr, float dg, float db, float sr, float sg, float sb, float shness);
	Material() {};
	float getShininess();
	glm::vec3 getAmbient();
	glm::vec3 getDiffuse();
	glm::vec3 getSpecular();
	
};


