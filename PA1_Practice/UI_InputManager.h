#pragma once
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

class UI_InputManager
{


public:

	// For switching the player model
	bool animatePlayerModel = true, switchedPlayerModel = true;

	// Variables for performance: calculating depth maps only when necessary, etc.
	bool modelsMoved = true;

	// Variables for enabling/disabling shadowing and texturing
	bool shadowingEnabled = true, sphereShadowingDisabled = false, texturingEnabled = true;

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
	bool rotateWorldOrientationAroundNegX = false;
	bool rotateWorldOrientationAroundPosX = false;
	bool rotateWorldOrientationAroundNegY = false;
	bool rotateWorldOrientationAroundPosY = false;
	bool resetWorldOrientation = false;
	bool revealHeightMap = false;


	// VARIABLES FOR CLICKING ON MODELS IN THE WORLD/SELECTING
	vec3 rayDirection;
	float xOffset = 0, yOffset = 0;
	float renderFrameWidth = SCR_WIDTH, renderFrameHeight = SCR_HEIGHT;

	std::vector<HierarchicalModel *> * SelectedModels;
	std::vector<HierarchicalModel *> * Models;


	UI_InputManager(GLFWwindow * mWind, Player & player, Camera * camera) : player(player) {
		this->projection = camera->GetProjectionMatrix();
		this->mWind = mWind;
		this->camera = camera;
		// Register GLFW callbacks
		registerCallbacks();
	};

	void processModelControl(float normalizedSpeed) {
		for (HierarchicalModel * HM : *SelectedModels) {

			this->normalizedSpeed = normalizedSpeed;
			mat4 & modelTransform = *HM->base()->transformMatrix;

			/*
			, A → move left, D → move right, W → move up, S → move down, a → rotate
			left 5 degrees about Y axis, d → rotate right 5 degrees about Y axis. */

			// The instructions say that capital A, D, W, and S are for movement. So, if either left or right shift are
			// pressed, then the a,d,w,s keys will be used for movement.

			bool shiftPressed = glfwGetKey(mWind, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(mWind, GLFW_KEY_RIGHT_SHIFT);
			bool leftCtrlPressed = glfwGetKey(mWind, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
			if (!leftCtrlPressed && ((!shiftPressed && capsLock) || (shiftPressed && !capsLock))) {
				// If pressing capital A, translate left (negative X axis)
				if (glfwGetKey(mWind, GLFW_KEY_A) == GLFW_PRESS) {
					modelTransform = glm::translate(modelTransform, glm::vec3(-camera->MovementSpeed * normalizedSpeed, 0.0, 0.0));
					modelsMoved = true;
				}
				// If pressing capital A, translate right (positive X axis)
				if (glfwGetKey(mWind, GLFW_KEY_D) == GLFW_PRESS) {
					modelTransform = glm::translate(modelTransform, glm::vec3(camera->MovementSpeed * normalizedSpeed, 0.0, 0.0));
					modelsMoved = true;
				}
				// If pressing capital W, translate up (positive Y axis)
				if (glfwGetKey(mWind, GLFW_KEY_W) == GLFW_PRESS) {
				//	modelTransform = glm::translate(modelTransform, glm::vec3(0.0, camera->MovementSpeed * normalizedSpeed, 0.0));
				//	modelsMoved = true;
				}
				// If pressing capital S, translate down (positive Y axis)
				if (glfwGetKey(mWind, GLFW_KEY_S) == GLFW_PRESS) {
					modelTransform = glm::translate(modelTransform, glm::vec3(0.0, -camera->MovementSpeed * normalizedSpeed, 0.0));
					modelsMoved = true;
				}

				if (glfwGetKey(mWind, GLFW_KEY_F) == GLFW_PRESS) {
					modelTransform = glm::translate(modelTransform, glm::vec3(0.0, 0.0, -camera->MovementSpeed * normalizedSpeed));
					modelsMoved = true;
				}	
				if (glfwGetKey(mWind, GLFW_KEY_G) == GLFW_PRESS) {
					modelTransform = glm::translate(modelTransform, glm::vec3(0.0, 0.0, camera->MovementSpeed * normalizedSpeed));
					modelsMoved = true;
				}
			
			}
			else if (!leftCtrlPressed) {

				// If pressing lower case 'd', rotate left 5 degrees about the y axis
				if (glfwGetKey(mWind, GLFW_KEY_D) == GLFW_PRESS && !dPressed) {
					modelTransform = glm::rotate(modelTransform, glm::radians(5.0f), glm::vec3(0.0, -1.0f, 0.0));
					modelsMoved = true;
					if (SelectedModels->at(SelectedModels->size() - 1) == HM) dPressed = true;

				}

				// If pressing lower case 'a', rotate right 5 degrees about the y axis
				if (glfwGetKey(mWind, GLFW_KEY_A) == GLFW_PRESS && !aPressed) {
					modelTransform = glm::rotate(modelTransform, glm::radians(5.0f), glm::vec3(0.0, 1.0f, 0.0));
					modelsMoved = true;
					if (SelectedModels->at(SelectedModels->size() - 1) == HM)aPressed = true;
				}

				// If pressing lower case 's', rotate 5 degrees about the 'positive' x axis
				if (glfwGetKey(mWind, GLFW_KEY_S) == GLFW_PRESS && !sPressed) {
					modelTransform = glm::rotate(modelTransform, glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
					modelsMoved = true;
					if (SelectedModels->at(SelectedModels->size() - 1) == HM)sPressed = true;

				}

				// If pressing lower case 'w', rotate right 5 degrees about the 'negative' x axis
				if (glfwGetKey(mWind, GLFW_KEY_W) == GLFW_PRESS && !wPressed) {
				//	modelTransform = glm::rotate(modelTransform, glm::radians(5.0f), glm::vec3(-1.0, 0.0, 0.0));
				//	modelsMoved = true;
				//	if (SelectedModels->at(SelectedModels->size() - 1) == HM)wPressed = true;
				}
				if (glfwGetKey(mWind, GLFW_KEY_W) == GLFW_RELEASE) wPressed = false;

				// If pressing capital F, translate forward (negative Z axis)
				if (glfwGetKey(mWind, GLFW_KEY_F) == GLFW_PRESS && !fPressed) {
					for (HierarchicalModel * HM : *SelectedModels)
						*HM->base()->transformMatrix = glm::translate(*(HM->base()->transformMatrix), glm::vec3(0.0, 0.0, -camera->MovementSpeed * normalizedSpeed));
					modelsMoved = true;
					fPressed = true;
				}

				// If pressing capital G, translate backward (positive Z axis)
				if (glfwGetKey(mWind, GLFW_KEY_G) == GLFW_PRESS && !gPressed) {
					for (HierarchicalModel * HM : *SelectedModels)
						*HM->base()->transformMatrix = glm::translate(*(HM->base()->transformMatrix), glm::vec3(0.0, 0.0, camera->MovementSpeed * normalizedSpeed));
					modelsMoved = true;
					gPressed = true;
				}

				if (glfwGetKey(mWind, GLFW_KEY_F) == GLFW_RELEASE) fPressed = false;
				if (glfwGetKey(mWind, GLFW_KEY_G) == GLFW_RELEASE) gPressed = false;


			}
		}
		if (glfwGetKey(mWind, GLFW_KEY_D) == GLFW_RELEASE) dPressed = false;
		if (glfwGetKey(mWind, GLFW_KEY_A) == GLFW_RELEASE) aPressed = false;
		if (glfwGetKey(mWind, GLFW_KEY_S) == GLFW_RELEASE) sPressed = false;
		if (glfwGetKey(mWind, GLFW_KEY_W) == GLFW_RELEASE) wPressed = false;


	}

	void processWorldOrientation(float normalizedSpeed, glm::mat4 & worldMatrix, mat4 & PointLight_Transform) {

		// If the user is clicking the arrow keys, rotate the world matrix to change the orientation appropriately.
		if (rotateWorldOrientationAroundNegY) {
			worldMatrix = glm::rotate(worldMatrix, -20*normalizedSpeed, glm::vec3(0, 1, 0));
			modelsMoved = true;
		}
		if (rotateWorldOrientationAroundPosY) {
			worldMatrix = glm::rotate(worldMatrix, 20*normalizedSpeed, glm::vec3(0, 1, 0));
			modelsMoved = true;
		}
		if (rotateWorldOrientationAroundNegX) { 
			worldMatrix = glm::rotate(worldMatrix, 20*normalizedSpeed, glm::vec3(1, 0, 0));
			modelsMoved = true;
		}
		if (rotateWorldOrientationAroundPosX) { 
			worldMatrix = glm::rotate(worldMatrix, -20*normalizedSpeed, glm::vec3(1, 0, 0));
			modelsMoved = true;
		}
		if (resetWorldOrientation) {
			worldMatrix = glm::mat4(1.0f);
			resetWorldOrientation = false;
			modelsMoved = true;
		}
	}

	void processInput(float normalizedSpeed)
	{

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

		// WHILE HOLDING LEFT CONTROL, USE THE CONTROLS TO MOVE THE CAMERA:
			// CAMERA MOVEMENT CONTROLS, FORWARD AND BACKWARD
			if (glfwGetKey(mWind, GLFW_KEY_W) == GLFW_PRESS)
				// Move the player model towards its front
				player.processMovement(FORWARD, normalizedSpeed);
				//camera->ProcessKeyboard(FORWARD, normalizedSpeed);
			if (glfwGetKey(mWind, GLFW_KEY_S) == GLFW_PRESS)
				player.processMovement(BACKWARD, normalizedSpeed);


		if (glfwGetKey(mWind, GLFW_KEY_RIGHT) == GLFW_PRESS)
			rotateWorldOrientationAroundNegX = true;
		else rotateWorldOrientationAroundNegX = false;

		if (glfwGetKey(mWind, GLFW_KEY_LEFT) == GLFW_PRESS)
			rotateWorldOrientationAroundPosX = true;
		else rotateWorldOrientationAroundPosX = false;

		if (glfwGetKey(mWind, GLFW_KEY_UP) == GLFW_PRESS)
			rotateWorldOrientationAroundPosY = true;
		else rotateWorldOrientationAroundPosY = false;

		if (glfwGetKey(mWind, GLFW_KEY_DOWN) == GLFW_PRESS)
			rotateWorldOrientationAroundNegY = true;
		else rotateWorldOrientationAroundNegY = false;

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

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		// So we can resize the window dynamically
		prevWidth = width;
		prevHeight = height;
	}


	//	Classic Perlin 2D Noise 
	//	by Stefan Gustavson
	//
	vec2 fade(vec2 t) { return t * t*t*(t*(t*6.0f - 15.0f) + 10.0f); }

	vec4 permute(vec4 x) { return mod((((x*34.0f) + 1.0f))*x, 289.0f); }
	vec4 taylorInvSqrt(vec4 r) { return 1.79284291400159f - 0.85373472095314f * r; }
	vec3 fade(vec3 t) { return t * t*t*(t*(t*6.0f - 15.0f) + 10.0f); }


	float noise(vec2 P) {
		vec4 Pi = floor(vec4(P.x, P.y, P.x, P.y)) + vec4(0.0, 0.0, 1.0, 1.0);
		vec4 Pf = fract(vec4(P.x, P.y, P.x, P.y)) - vec4(0.0, 0.0, 1.0, 1.0);
		Pi = mod(Pi, 289.0f); // To avoid truncation effects in permutation
		vec4 ix(Pi.x, Pi.z, Pi.x, Pi.z);
		vec4 iy(Pi.y, Pi.y, Pi.w, Pi.w);
		vec4 fx(Pf.x, Pf.z, Pf.x, Pf.z);
		vec4 fy(Pf.y, Pf.y, Pf.w, Pf.w);
		vec4 i = permute(permute(ix) + iy);
		vec4 gx = 2.0f * fract(i * 0.01232462f) - 1.0f; // 1/41 = 0.024...
		vec4 gy = abs(gx) - 0.5f;
		vec4 tx = floor(gx + 0.5f);
		gx = gx - tx;
		vec2 g00 = vec2(gx.x, gy.x);
		vec2 g10 = vec2(gx.y, gy.y);
		vec2 g01 = vec2(gx.z, gy.z);
		vec2 g11 = vec2(gx.w, gy.w);
		vec4 norm = 1.79284291400159f - 0.85373472095314f *
			vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11));
		g00 *= norm.x;
		g01 *= norm.y;
		g10 *= norm.z;
		g11 *= norm.w;
		float n00 = dot(g00, vec2(fx.x, fy.x));
		float n10 = dot(g10, vec2(fx.y, fy.y));
		float n01 = dot(g01, vec2(fx.z, fy.z));
		float n11 = dot(g11, vec2(fx.w, fy.w));
		vec2 fade_xy(fade(vec2(Pf.x, Pf.y)));
		vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
		float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
		return 2.3 * n_xy;
	}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (glfwGetKey(mWind, GLFW_KEY_1) == GLFW_PRESS && !pressed1) {
			animatePlayerModel = !animatePlayerModel;
			switchedPlayerModel = true;
			pressed1 = true;
		}
		if (glfwGetKey(mWind, GLFW_KEY_1) == GLFW_RELEASE) pressed1 = false;

		if (glfwGetKey(mWind, GLFW_KEY_X) == GLFW_PRESS && !xPressed) {
			texturingEnabled = !texturingEnabled;
			xPressed = true;
		}
		if (glfwGetKey(mWind, GLFW_KEY_X) == GLFW_RELEASE) xPressed = false;


		if (glfwGetKey(mWind, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed) {
			modelsMoved = true;
			spacePressed = true;
		}

		if (glfwGetKey(mWind, GLFW_KEY_SPACE) == GLFW_RELEASE) spacePressed = false;

		// MOVING FORWARD AND BACKWARDS BY A SMALL DELTA WITH CAPITAL F/G
		bool shiftPressed = glfwGetKey(mWind, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(mWind, GLFW_KEY_RIGHT_SHIFT);
		bool leftCtrlPressed = glfwGetKey(mWind, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
		if (!leftCtrlPressed && ((!shiftPressed && capsLock) || (shiftPressed && !capsLock))) {

			// Press capitalized 'B' to disable/enable shadows for the transparent spheres (we were not sure if they should have had shadows)
			if (glfwGetKey(mWind, GLFW_KEY_B) == GLFW_PRESS && !shiftBPressed) {
				sphereShadowingDisabled = !sphereShadowingDisabled;
				modelsMoved = true;
				shiftBPressed = true;
			}
			if (glfwGetKey(mWind, GLFW_KEY_B) == GLFW_RELEASE) shiftBPressed = false;

		
		}
		else {
			// Press 'B' to enable/disable shadows
			if (glfwGetKey(mWind, GLFW_KEY_B) == GLFW_PRESS && !bPressed) {
				shadowingEnabled = !shadowingEnabled;
				bPressed = true;
			}
			if (glfwGetKey(mWind, GLFW_KEY_B) == GLFW_RELEASE) bPressed = false;

		}

		

		if (glfwGetKey(mWind, GLFW_KEY_U) == GLFW_RELEASE) uPressed = false;
		if (glfwGetKey(mWind, GLFW_KEY_J) == GLFW_RELEASE) jPressed = false;

		// ENTER TO GO TO THE NEXT SLIDE
		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
			if (showSlide1) {
				showSlide1 = false;
				showSlide2 = true;
			}
			else if (showSlide2) {
				showSlide2 = false;
				showSlide3 = true;

			}
			else if (showSlide3) {
				showSlide3 = false;
				showSlide1 = true;
			}
		}
		// BACKSPACE TO GO BACK
		if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
			if (showSlide1) {
				showSlide1 = false;
				showSlide3 = true;
			}
			else if (showSlide2) {
				showSlide2 = false;
				showSlide1 = true;

			}
			else if (showSlide3) {
				showSlide3 = false;
				showSlide2 = true;
			}
		}
		// END TO HIDE
		if (glfwGetKey(window, GLFW_KEY_END) == GLFW_PRESS) {
			showSlide1 = false;
			showSlide2 = false;
			showSlide3 = false;
		}
	
		// IF CAPS LOCK IS PRESSED, SET capsLock BOOL TO TRUE
		if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS)
			capsLock = !capsLock;

		// PRINT THE CAMERA POSITION AND DIRECTION IT IS FACING TO THE CONSOLE WINDOW
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		{
			glm::vec3 f = camera->Front;
			glm::vec3 p = camera->Position;
			std::cout << "Camera is Facing: " << f.x << "," << f.y << "," << f.z << std::endl;
			std::cout << "Camera's Position is: " << p.x << "," << p.y << "," << p.z << std::endl;
			std::cout << "Camera's FOV angle is: " << camera->Zoom << std::endl;
		}

		// ASSIGNMENT 1 - CONTROLS
		if (glfwGetKey(mWind, GLFW_KEY_T) == GLFW_PRESS)
			primitiveType = GL_TRIANGLES;

		if (glfwGetKey(mWind, GLFW_KEY_P) == GLFW_PRESS)
			primitiveType = GL_POINTS;

		if (glfwGetKey(mWind, GLFW_KEY_L) == GLFW_PRESS)
			primitiveType = GL_LINES;

		

		// SECRET CONTROLS TO MOVE THE CAMERA AROUND THE WORLD
		if (glfwGetKey(mWind, GLFW_KEY_MINUS) == GLFW_PRESS) {
			if (glfwGetKey(mWind, GLFW_KEY_A) == GLFW_PRESS)
				camera->ProcessKeyboard(ROTATE_LEFT, 0.01f);
			if (glfwGetKey(mWind, GLFW_KEY_D) == GLFW_PRESS)
				camera->ProcessKeyboard(ROTATE_RIGHT, 0.01f);
		}



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

		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			leftClicked = true;

		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
			leftClicked = false;	
		
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
			rightClicked = true;

		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
			rightClicked = false;

		bool hoveringOnModel = false;
		float nearestSelect = std::numeric_limits<float>::max(), d = std::numeric_limits<float>::lowest();

	
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

