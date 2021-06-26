#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#undef main


class OpenGLWindow
{
public:

	OpenGLWindow(float WIDTH, float HEIGHT);

	void clearColor(glm::vec4 clearColor);
	inline float getWidth() { return WIDTH; }
	inline float getHeight() { return HEIGHT; }
	inline float getAspectRatio() { return ASPECT_RATIO; }

	inline GLFWwindow * glWindow() { return window; }

private:

	GLuint gProgramID = 0;

	float WIDTH, HEIGHT, ASPECT_RATIO;
	GLFWwindow* window;
};

