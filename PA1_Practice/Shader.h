#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include "Material.h"
#include <glm/gtx/transform.hpp>
#include<glm/gtx/string_cast.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

class Shader
{

public:
	/////////////////////////////////////// STRUCTS TO SIMPLIFY SENDING INFO TO THE SHADERS ////////////////////////////////////////////////
	struct ViewClipPlane { float far, near; };

	struct CameraViewInfo { 
		vec3 viewPosition;
		mat4 projection, view;
		vec3 fogColor;
	};

	struct UserInputDefinedVariables {
		bool texturingEnabled, shadowingEnabled;
	};

	struct PointLight {
		vec3 position, ambient, diffuse, specular;
		vec2 attenuation;
		float strength;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	unsigned int ID;

	Shader() {};

	Shader(const char* vertexPath, const char* fragmentPath)
	{
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			vShaderFile.close();
			fShaderFile.close();
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure& e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char * fShaderCode = fragmentCode.c_str();
		unsigned int vertex, fragment;

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	// Constructor for vertex + geometry + fragment shader combo
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
	{
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string geometryCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream gShaderFile;
		std::ifstream fShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			// open files
			vShaderFile.open(vertexPath);
			gShaderFile.open(geometryPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, gShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			gShaderStream << gShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			gShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			geometryCode = gShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure& e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* gShaderCode = geometryCode.c_str();
		const char * fShaderCode = fragmentCode.c_str();
		// 2. compile shaders
		unsigned int vertex, geometry, fragment;

		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");

		// geometry shader
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");

		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");

		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, geometry);
		glAttachShader(ID, fragment);

		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(geometry);
		glDeleteShader(fragment);
	}

	void use() // Utility function to use the shader program
	{
		glUseProgram(ID);
	}

	// Generic uniform setting functions for the shader.
	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setMat4(const std::string &name, glm::mat4 value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
	void setVec3(const std::string &name, glm::vec3 value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
	}
	void setVec2(const std::string &name, glm::vec2 value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
	}

	void setMaterial( Material m) {
		setVec3("material.ambient", m.getAmbient());
		setVec3("material.diffuse", m.getDiffuse());
		setVec3("material.specular", m.getSpecular());
		setFloat(".shininess", m.getShininess());

	}

	void setMaterial(std::string MaterialName, Material m) {
		setVec3(MaterialName + ".ambient", m.getAmbient());
		setVec3(MaterialName + ".diffuse", m.getDiffuse());
		setVec3(MaterialName + ".specular", m.getSpecular());
		setFloat(MaterialName + ".shininess", m.getShininess());
	}

	void setViewClipPlane(ViewClipPlane VCP) {
		setFloat("far", VCP.far);
		setFloat("near", VCP.near);
	}

	void setViewInfo(CameraViewInfo CVI) {
		setVec3("viewPos", CVI.viewPosition);
		setMat4("projection", CVI.projection);
		setMat4("view", CVI.view);
		setVec3("distantBackgroundFogColor", CVI.fogColor);

	}

	void setUserInputDefinedVariables(UserInputDefinedVariables UIDV) {
		setBool("texturingEnabled", UIDV.texturingEnabled);
		setBool("shadowingEnabled", UIDV.shadowingEnabled);
	}
	
	void setPointLights(PointLight & PL) {
		setVec3("pointLight.position", PL.position);
		setVec3("pointLight.ambient", PL.ambient);
		setVec3("pointLight.diffuse", PL.diffuse);
		setVec3("pointLight.specular", PL.specular);
		setVec2("pointLight.attenuation", PL.attenuation);
		setFloat("pointLight.strength", PL.strength);
	}

	void setModelMatrix(mat4 MM) {
		setMat4("model", MM);
	}

	void setCrystalLights(std::vector<PointLight> & crystalLights) {

		for (unsigned int i = 0; i < crystalLights.size(); ++i) {
			std::string loc = "crystalLights[" + std::to_string(i) + "]";
			setVec3(loc + ".position", crystalLights[i].position);
			setVec3(loc + ".ambient", crystalLights[i].ambient);
			setVec3(loc + ".diffuse", crystalLights[i].diffuse);
			setVec3(loc + ".specular", crystalLights[i].specular);
			setVec2(loc + ".attenuation", crystalLights[i].attenuation);
			setFloat(loc + ".strength", crystalLights[i].strength);
		}
	}

	void prepare(CameraViewInfo & CVI, ViewClipPlane & VCP, UserInputDefinedVariables &UIDV, PointLight & PL) {
		use();
		setInt("texture_diffuse", 0);
		setInt("texture_specular", 1);
		setInt("texture_normal", 2);
		setViewInfo(CVI);
		setViewClipPlane(VCP);
		setUserInputDefinedVariables(UIDV);
		setPointLights(PL);
	}
	void prepare(CameraViewInfo & CVI, ViewClipPlane & VCP, UserInputDefinedVariables &UIDV) {
		use();
		setInt("diffuseMap", 0);
		setInt("texture_specular", 1);
		setInt("texture_normal", 2);
		setViewInfo(CVI);
		setViewClipPlane(VCP);
		setUserInputDefinedVariables(UIDV);
	}
private:

	// Error checking to print to console to debug shaders
	void checkCompileErrors(unsigned int shader, std::string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- ---------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};
