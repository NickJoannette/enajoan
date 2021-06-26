#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>
#include<glm/gtx/string_cast.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Material.h"


Material::Material(float ar, float ag, float ab, float dr, float dg, float db, float sr, float sg, float sb, float shness) {
	ambient_r = ar;   ambient_g = ag;   ambient_b = ab;
	diffuse_r = dr;   diffuse_g = dg;   diffuse_b = db;
	specular_r = sr;  specular_g = sg;  specular_b = sb;


	ambient = {ambient_r, ambient_g, ambient_b};
	diffuse = { diffuse_r, diffuse_g, diffuse_b};
	specular = {specular_r, specular_g, specular_b};

	shininess = shness;
}

float Material::getShininess(){
	return shininess;
}

glm::vec3 Material::getAmbient() {
	return ambient;
};
glm::vec3 Material::getDiffuse() {
	return diffuse;
};
glm::vec3 Material::getSpecular() {
	return specular;
};