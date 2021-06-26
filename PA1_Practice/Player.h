#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include<glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "OpenGLWindow.h"
#include <assimp/quaternion.h>

// So that the player model can be drawn from functions inside this class
#include "Skinned_Mesh.h"
#include "Shader.h"
/////////////////////////////////////////////////////////////////////////



using namespace glm;


enum Movement {

	FORWARD,
	BACKWARD,

	LEFT,
	RIGHT,

	UP,
	DOWN,

	ROTATE_RIGHT,
	ROTATE_LEFT
};
class Player
{


public :


	SkinnedMesh * Model;


	Player() { 
		initialize();
	}

	void initialize() {

		ModelMatrix = RotationMatrix = TranslationMatrix = mat4(1.0f);
		Position = vec3(0.0f);
		Velocity = vec3(0.0f);
		Acceleration = vec3(0.0f);
		Up = vec3(0, 1, 0);
		Right = vec3(1, 0, 0);
		Front = vec3(0, 0, -1);
		WorldUp = vec3(0, 1, 0);

		RotationQuaternion = angleAxis(0.0f, Front);
	}

	void processMovement(Movement direction, float normalizedSpeed);


	void linkModel(SkinnedMesh * SM) { Model = SM; }

	void Render(GLuint primitiveType, Shader & s, bool animated, bool overrideModelTexture) {
		if (animated) {
			s.setBool("animatedModel", true);
			std::vector<Matrix4f> transforms;
			Model->BoneTransform(glfwGetTime()*2.5f, transforms);
			for (unsigned int i = 0; i < transforms.size(); ++i) {
				std::string loc = "gBones[" + std::to_string(i) + "]";
				glUniformMatrix4fv(glGetUniformLocation(s.ID, loc.c_str()), 1, GL_TRUE, (const GLfloat*)transforms[i]);
			}
		}
		// Set shader bools for player 
		s.setBool("isFadeableObject", false);
		// Call the SkinnedModel.Render() function
		Model->Render(primitiveType, s, overrideModelTexture);
		// Set shader bools back
		s.setBool("isFadeableObject", true);
		s.setBool("animatedModel", false);
	}

	inline const mat4 & const getModelMatrix(float scaleFactor) { 
		Up = glm::normalize(Up);
		Front = glm::normalize(Front);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
		quat lookat = quatLookAt(Front, Up);
		return (scale(translate(mat4(1.0f),Position),vec3(scaleFactor))) * toMat4(lookat);
	}

	void updateView() {

	}

	inline const vec3 const getPosition() { return Position;  }
	inline const vec3 const getVelocity() { return Velocity;  }
	vec3 Position, Velocity, Acceleration;
	void update(float dt);

	void printMetrics();

	static inline glm::quat quat_cast(const aiQuaternion &q) { return glm::quat(q.w, q.x, q.y, q.z); }

	void rotateQuaternionTowards(quat target, vec3 camFront, vec3 camUp) {
	}


	// Public for debugging environment
	vec3 Front, Up, Right, WorldUp;
	mat4 ModelMatrix, RotationMatrix, TranslationMatrix;
	quat RotationQuaternion;
	bool unsolvedCollision = false;

private:

	// Physics properties
	const float MAX_ACCELERATION = 5.0f;
	const float MAX_VELOCITY = 10.0f;

	float accelerationMagnitude;

	// Orientation properties
	



};

