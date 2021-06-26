#ifndef CAMERA_H
#define CAMERA_H

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
#include "Player.h"

// Default orientations
const float YAW = -90.0f;
const float PITCH = -glm::degrees(atanf(3.0f / 6.0f));
const float SPEED = 20.0f;
const float SENSITIVITY = 1.0;
const float ZOOM = 95.00f;

class Camera
{
public:
	Player & player;
	OpenGLWindow * mainWindow;

	// COLLISION TEMP VARIABLES
	bool collisionForward = false;
	quat rt;

	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	// Perspective matrix
	glm::mat4 projection;

	float pitch = 0, yaw = 0, roll = 0;
	// Camera Speeds and Sensitivities
	float MovementSpeed;
	float MouseSensitivity = 0.2f;
	float ZoomSensitivity = 0.5f;
	float Zoom;

	// Constructor with vectors
	Camera(OpenGLWindow * mainwindow, Player & player, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : player(player), Front(glm::vec3(0.0f, -.450f, -.9f)), MovementSpeed(SPEED), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		mainWindow = mainwindow;
		updateView();
		UpdateProjection(1.0f, 500.0f);
	}

	// Third person camera functions...
	void updateCameraViewAngleRelativeToPlayerPosition() {
	}

	// Returns the camera's view matrix using glm::lookAt
	glm::mat4 GetViewMatrix(){
		return lookAt(Position, player.getPosition() + Front, Up);
	}

	glm::mat4 * GetProjectionMatrix() { return &projection; }
	glm::vec3  GetPosition() { return Position; }

	void UpdateProjection(float near, float far) {
		projection = glm::perspective(glm::radians(Zoom), mainWindow->getAspectRatio(), near, far);
	}

	// Per-frame keyboard input process function
	void ProcessKeyboard(Movement direction, float normalizedSpeed)
	{
		float velocity = MovementSpeed * normalizedSpeed;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;

		// Limit y to ground level (~1.0 units up)
		float oldY = Position.y;

		if (direction == DOWN)
			Position -= Up * velocity;
		//if (Position.y <= 1.0f) Position.y = 1.0f;
		if (direction == UP)
			Position += Up * velocity;
	}













	void printMetrics() {
		std::cout << " --------------------- " << std::endl;
		std::cout << "CAMERA OBJECT METRICS" << std::endl;
		std::cout << "Position: (" << Position.x << ", " << Position.y << ", " << Position.z << ")" << std::endl;
		std::cout << "Front: (" << Front.x << ", " << Front.y << ", " << Front.z << ")" << std::endl;
	}










	void ProcessTilt(float tiltQty)
	{
		vec3 axis = cross(Up, Front);
		pitch += tiltQty;
		rt = angleAxis(0.01f*tiltQty, -axis);
		updateView();
	}

	void ProcessRoll(float tiltQty)
	{
		vec3 axis = cross(Right, Up);
		pitch += tiltQty;
		rt = angleAxis(0.01f*tiltQty, -axis);
		updateView();
	}

	mat4 test() {
		return toMat4(rt);
	}

	void ProcessPan(float panQty)
	{
		vec3 axis = cross(Right, Front);
		yaw += panQty;
		rt = angleAxis(0.01f*panQty, axis);
		updateView();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= yoffset;
		UpdateProjection(1.0f, 500.0f);
	}

	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateView()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front = vec3(toMat4(rt) * vec4(Front, 1.0f));
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		WorldUp = vec3(toMat4(rt) * vec4(WorldUp, 1.0f));
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
		//p->Up = Up;
	}

private:

};
#endif