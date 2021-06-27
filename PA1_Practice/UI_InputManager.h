#pragma once
#include <irrKlang/irrKlang.h>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>
#include "OpenGLWindow.h"
#include "HierarchicalModel.h"
#include "PhysicsUtilities.h"
#include "Utilities.h"
#include "Camera.h"
#include "Player.h"

using namespace glm;
using namespace Utilities;
using namespace PhysicsUtilities;
using namespace irrklang;

class UI_InputManager
{
public:
	ISoundEngine* SoundEngine;
	ISoundSource* jump1;

	// For switching the player model
	bool animatePlayerModel = true, switchedPlayerModel = true;

	// Keep track of mouse position for zoom/pan/tilt
	float mouseLastX = 0.0f, mouseLastY = 0.0f, rayMouseX = 0.0f, rayMouseY = 0.0f;
	bool leftClicked = false; bool rightClicked = false;
	
	// Mouse Button Variables
	bool leftMouseClicked = false, rightMouseClicked = false;
	bool fPressed = false, gPressed = false;
	bool uPressed = false, jPressed = false;
	bool pressed1 = false, pressed2 = false;
	bool pressed3 = false, pressed4 = false;
	bool pressed5 = false;
	bool bPressed = false, shiftBPressed = false, xPressed = false;
	bool aPressed = false, dPressed = false, sPressed = false, wPressed = false;
	bool spacePressed = false;

	// For keeping track of which primitive type is used to render
	GLint primitiveType = GL_TRIANGLES;

	// Player object we can control
	Player & player;

	// GUI Related Variables
	bool hoveringOnGUI = false;

	// SCREEN DIMENSIONS KEPT AS CONSTANT FOR NOW
	const float SCR_WIDTH = 1024.0, SCR_HEIGHT = 768.0;
	float prevWidth = SCR_WIDTH, prevHeight = SCR_HEIGHT;
	float normalizedSpeed = 1.0f;

	// Keep track of which model is currently selected by the F1...F5 keys

	bool resetLetterModelsToOriginalPosition = false;
	bool showSlide1 = true;
	bool showSlide2 = false;
	bool showSlide3 = false;

	// CAPS LOCK VARIABLE AND STATUS OF WORLD ORIENTATION CHANGE BY ARROW KEYS
	bool capsLock = false;

	// VARIABLES FOR CLICKING ON MODELS IN THE WORLD/SELECTING
	vec3 rayDirection;
	float xOffset = 0, yOffset = 0;
	float renderFrameWidth = SCR_WIDTH, renderFrameHeight = SCR_HEIGHT;

	UI_InputManager(GLFWwindow * mWind, Player & player, Camera * camera, ISoundEngine * soundEngine) : player(player) {
		this->projection = camera->GetProjectionMatrix();
		this->mWind = mWind;
		this->camera = camera;
		this->SoundEngine = soundEngine;
		jump1 = SoundEngine->addSoundSourceFromFile("../Audio/jump1.mp3");
		// Register GLFW callbacks
		registerCallbacks();
	};

	void processInput(float normalizedSpeed)
	{
		// Camera/main character movement
		if (glfwGetKey(mWind, GLFW_KEY_W) == GLFW_PRESS)
			camera->Position += .005f * vec3(camera->Front);//.x,0.0f,camera->Front.z);
		if (glfwGetKey(mWind, GLFW_KEY_S) == GLFW_PRESS)
			camera->Position -= .005f * vec3(camera->Front);//.x, 0.0f, camera->Front.z);
		if (glfwGetKey(mWind, GLFW_KEY_D) == GLFW_PRESS)
			camera->Position += .005f * vec3(camera->Right);// .x, 0.0f, camera->Right.z);

		if (glfwGetKey(mWind, GLFW_KEY_A) == GLFW_PRESS)
			camera->Position -= .005f * vec3(camera->Right);//.x, 0.0f, camera->Right.z);



		if (glfwGetKey(mWind, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(mWind, true);
		if (glfwGetKey(mWind, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
			camera->MovementSpeed -= 0.50f;
		if (glfwGetKey(mWind, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS)
			camera->MovementSpeed += 0.5f;
		if (glfwGetKey(mWind, GLFW_KEY_R) == GLFW_PRESS)
			camera->ProcessRoll(normalizedSpeed*1750.0f);
		if (glfwGetKey(mWind, GLFW_KEY_Q) == GLFW_PRESS)
			camera->ProcessRoll(-normalizedSpeed*1750.0f);

	};


private:
	Camera * camera;
	GLFWwindow * mWind;
	glm::mat4 * projection;
	bool firstMouse = true;

#pragma region // Callback functions

	void registerCallbacks() {
		glfwSetWindowUserPointer(mWind, reinterpret_cast<void*>(this));
		glfwSetKeyCallback(mWind, keyCallBackPort);
		glfwSetFramebufferSizeCallback(mWind, framebufferSizeCallBackPort);
		glfwSetMouseButtonCallback(mWind, mouseButtonCallBackPort);
		glfwSetCursorPosCallback(mWind, mouseCallBackPort);
		glfwSetWindowSizeCallback(mWind, windowSizeCallBackPort);
	};
	static void windowSizeCallBackPort(GLFWwindow* window, int width, int height) {
		static_cast<UI_InputManager*>(glfwGetWindowUserPointer(window))->
			window_size_callback(window, width, height);
	};

	static void keyCallBackPort(GLFWwindow* window, int key, int scancode, int action, int mods) {
		static_cast<UI_InputManager*>(glfwGetWindowUserPointer(window))->
			key_callback(window, key, scancode, action, mods);
	};

	static void framebufferSizeCallBackPort(GLFWwindow* window, int width, int height) {
		static_cast<UI_InputManager*>(glfwGetWindowUserPointer(window))->
			framebuffer_size_callback(window, width, height);
	};

	static void mouseCallBackPort(GLFWwindow* window, double xpos, double ypos) {
		static_cast<UI_InputManager*>(glfwGetWindowUserPointer(window))->
			mouse_callback(window, xpos, ypos);
	};
	
	static void mouseButtonCallBackPort(GLFWwindow* window, int button, int action, int mods) {
		static_cast<UI_InputManager*>(glfwGetWindowUserPointer(window))->
			mouse_button_callback(window, button, action, mods);
	};
	void window_size_callback(GLFWwindow * window, int width, int height) {

		if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED)) {
			
		}
	}
	void framebuffer_size_callback(GLFWwindow* window, int width, int height){
		prevWidth = width;
		prevHeight = height;
	}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (glfwGetKey(mWind, GLFW_KEY_1) == GLFW_PRESS && !pressed1) {
			animatePlayerModel = !animatePlayerModel;
			switchedPlayerModel = true;
			pressed1 = true;
		}
		if (glfwGetKey(mWind, GLFW_KEY_1) == GLFW_RELEASE) pressed1 = false;
		if (glfwGetKey(mWind, GLFW_KEY_X) == GLFW_RELEASE) xPressed = false;
		if (glfwGetKey(mWind, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed) {
			spacePressed = true;
			camera->Velocity += vec3(0,1,0) * .015f;
			SoundEngine->play2D(jump1, false);
		}
		if (glfwGetKey(mWind, GLFW_KEY_SPACE) == GLFW_RELEASE) spacePressed = false;
		if (glfwGetKey(mWind, GLFW_KEY_B) == GLFW_PRESS && !bPressed) bPressed = true;
		if (glfwGetKey(mWind, GLFW_KEY_B) == GLFW_RELEASE) bPressed = false;
		if (glfwGetKey(mWind, GLFW_KEY_U) == GLFW_RELEASE) uPressed = false;
		if (glfwGetKey(mWind, GLFW_KEY_J) == GLFW_RELEASE) jPressed = false;
		if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS)
			capsLock = !capsLock;
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		{
			glm::vec3 f = camera->Front;
			glm::vec3 p = camera->Position;
			std::cout << "Camera is Facing: " << f.x << "," << f.y << "," << f.z << std::endl;
			std::cout << "Camera's Position is: " << p.x << "," << p.y << "," << p.z << std::endl;
			std::cout << "Camera's FOV angle is: " << camera->Zoom << std::endl;
		}
		if (glfwGetKey(mWind, GLFW_KEY_T) == GLFW_PRESS)primitiveType = GL_TRIANGLES;
		if (glfwGetKey(mWind, GLFW_KEY_P) == GLFW_PRESS)primitiveType = GL_POINTS;
		if (glfwGetKey(mWind, GLFW_KEY_L) == GLFW_PRESS)primitiveType = GL_LINES;

	};

	inline static bool IntersectRaySphere(float &t0, float &t1, const vec3 &rO, const vec3 &rD, const vec3 &sO, const float sqRad)
	{
		vec3 L = sO - rO;
		float tca = dot(L,rD);
		float d2 = dot(L,L) - tca * tca;

		if (d2 <= sqRad) // hits sphere
		{
			float thc = sqrt(sqRad - d2);
			t0 = tca - thc;
			t1 = tca + thc;
			return true;
		}
		else // misses sphere
		{
			t0 = tca;
			t1 = tca;
			return false;
		}
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)leftClicked = true;
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) leftClicked = false;	
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) rightClicked = true;
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) rightClicked = false;
	}

	void mouse_callback(GLFWwindow* window, float xpos, float ypos)
	{
		if (!hoveringOnGUI && !(glfwGetKey(mWind, GLFW_KEY_TAB) == GLFW_PRESS)) {
				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
					float movementInY = mouseLastY - ypos; // how much the mouse was just dragged
					camera->ProcessTilt(movementInY * camera->MouseSensitivity);

				}
				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
					float movementInX = mouseLastX - xpos; // how much the mouse was just dragged
					camera->ProcessPan(movementInX * camera->MouseSensitivity);

				}
				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
					float movementInY = mouseLastY - ypos; // how much the mouse was just dragged
					camera->ProcessMouseScroll(movementInY * camera->ZoomSensitivity);
				}
			}

		vec3 ray_wor;

		// Logic for getting mouse click ray in world coordinates
	
		glm::vec3 camPos = camera->Position;
		glm::mat4 transf = glm::inverse(*camera->GetProjectionMatrix());
		float x = (2.0f * (float)rayMouseX) / renderFrameWidth - 1.0f;
		float y = 1.0f - (2.0f * (float)rayMouseY) / renderFrameHeight;
		float z = 1.0f;
		glm::vec3 ray_nds = glm::vec3(x, y, z);
		glm::vec4 ray_clip = glm::vec4(ray_nds, 1.0);
		glm::vec4 ray_eye = glm::inverse(*camera->GetProjectionMatrix()) * ray_clip;
		ray_eye = glm::vec4(ray_eye.x, -ray_eye.y, -1.0, 0.0);
		ray_wor = (glm::inverse(camera->GetViewMatrix()) * ray_eye);
		rayDirection = glm::normalize(ray_wor);
		
		rayMouseX = xpos - xOffset;
		rayMouseY = renderFrameHeight - (-yOffset + abs(ypos));
		
		mouseLastX = xpos;
		mouseLastY = ypos;

	};

#pragma endregion
};

