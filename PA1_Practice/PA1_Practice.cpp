#include <iostream>
#include <chrono>
#include <vector>
#include <iterator>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <stack>
#include "OpenGLWindow.h"
#include "Shader.h"
#include "UI_InputManager.h"
#include "Camera.h"
#include "Cube.h"
#include "MarchingCubes.h"
#include "MarchingCubeChunk.h"
#include "Grid.h"
#include "Line.h"
#include "Sphere.h"
#include "HierarchicalModel.h"
#include "PhysicsUtilities.h"
#include "Material.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <irrKlang/irrKlang.h>
#include "Model.h"
#include "Mesh.h"
#include "Player.h"
#include "Boid.h"

/*
TESTING ANIMATION INCLUDES
*/

#include "ogldev_math_3d.h"
#include "Skinned_Mesh.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

using namespace glm;
using namespace PhysicsUtilities;


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

// CONSTANT USER SPECIFIED PARAMETER TO DETERMINE MARCHING CUBE VERTEX DENSITY

const float surfaceThreshold = 122;

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////









// Constants for defining window size
float SCR_WIDTH = 1024.0, SCR_HEIGHT = 768.0;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
float linear = 0.08f; float linearZero, linearOne;
float quadratic = 0.06f; float quadZero, quadOne;
// Create a new OpenGLWindow
OpenGLWindow * mainWindow = new OpenGLWindow(SCR_WIDTH, SCR_HEIGHT);

// Associate a GLFWindow
GLFWwindow * mWind = mainWindow->glWindow();

Player player;
Camera camera(mainWindow, vec3(0, 3, 6));

// Define the world matrix
mat4 worldMatrix(scale(vec3(1.0f)));

// Sound engine
using namespace irrklang;
ISoundEngine *SoundEngine = createIrrKlangDevice();
ISoundSource * musicArea1 = SoundEngine->addSoundSourceFromFile("../Audio/synth.mp3");
ISoundSource * grunt1 = SoundEngine->addSoundSourceFromFile("../Audio/grunt1.mp3");
ISoundSource * step1 = SoundEngine->addSoundSourceFromFile("../Audio/step1.mp3");

#pragma region Load shaders
// Instantiate the shaders. The shader class takes care of loading them in and checking for compilation errors.
Shader lineShader("../Shaders/lineShader.vs", "../Shaders/lineShader.fs");
Shader hdrShader("../Shaders/hdrShader.vs", "../Shaders/hdrShader.fs");
Shader blurShader("../Shaders/blur.vs", "../Shaders/blur.fs");
Shader bloomShader("../Shaders/bloom.vs", "../Shaders/bloom.fs");
Shader bloomFinalShader("../Shaders/bloom_final.vs", "../Shaders/bloom_final.fs");
Shader monoColorOutlineShader("../Shaders/monoColorOutlineShader.vs", "../Shaders/monoColorOutlineShader.fs");
Shader cubeDepthShader("../Shaders/cubeDepthShader.vs", "../Shaders/cubeDepthShader.fs", "../Shaders/cubeDepthShader.gs");
Shader falseFloatingParticlesShader("../Shaders/falseFloatingParticlesShader.vs", "../Shaders/falseFloatingParticlesShader.fs", "../Shaders/falseFloatingParticlesShader.gs");
Shader testGrassShader("../Shaders/testGrassShader.vs", "../Shaders/testGrassShader.fs", "../Shaders/testGrassShader.gs");
Shader sphereShader("../Shaders/sphereShader.vs", "../Shaders/sphereShader.fs");
Shader cubeShader("../Shaders/cubeShader.vs", "../Shaders/cubeShader.fs");
Shader masterWorldShader("../Shaders/masterWorldShader.vs", "../Shaders/masterWorldShader.fs");
Shader crystalLightShader("../Shaders/crystalLightShader.vs", "../Shaders/crystalLightShader.fs");
Shader importedModelShader("../Shaders/importedModelShader.vs", "../Shaders/importedModelShader.fs");
Shader marchingCubesShader("../Shaders/marchingCubesShader.vs", "../Shaders/marchingCubesShader.fs");
#pragma endregion

#pragma region Define materials
// Define different material structs to use
Material turquesa(0.1, 0.18725, 0.1745, 0.396, 0.74151, 0.69102, 0.297254, 0.30829, 0.306678, 0.1);
Material esmeralda(0.0215, 0.1745, 0.0215, 0.07568, 0.61424, 0.07568, 0.633, 0.727811, 0.633, 0.6);
Material jade(0.135, 0.2225, 0.1575, 0.54, 0.89, 0.63, 0.316228, 0.316228, 0.316228, 0.1);
Material obsidian(0.05375, 0.05, 0.06625, 0.18275, 0.17, 0.22525, 0.332741, 0.328634, 0.346435, 0.3);
Material gold(0.24725, 0.1995, 0.0745, 0.75164, 0.60648, 0.22648, 0.628281, 0.555802, 0.366065, 0.4);
Material silver(0.19225, 0.19225, 0.19225, 0.50754, 0.50754, 0.50754, 0.508273, 0.508273, 0.508273, 0.4);
Material Ruby(0.1745, 0.01175, 0.01175, 0.61424, 0.04136, 0.04136, 0.727811, 0.626959, 0.626959, 0.6);
Material chrome(0.25, 0.25, 0.25, 0.4, 0.4, 0.4, 0.774597, 0.774597, 0.774597, 0.6);
Material copper(0.19125, 0.0735, 0.0225, 0.7038, 0.27048, 0.0828, 0.256777, 0.137622, 0.086014, 0.1);
Material tile(0.32, 0.32, 0.32, 0.55, 0.55, 0.55, 0.65, 0.65, 0.65, 0.5);
Material blueRubber(0.0, 0.00, 0.05, 0.4, 0.4, 0.5, 0.04, 0.04, 0.7, .078125);

#pragma endregion

// OUR MODELS TRANSFORMS	
mat4 Nicks_Transform(translate(vec3(0))); vec3 NicksFN(0);
mat4 Mohamads_Transform(translate(vec3(0))); vec3 MohamadsFN(0);
mat4 Adriens_Transform(translate(vec3(0))); vec3 AdriensFN(0);
mat4 Samis_Transform(translate(vec3(0))); vec3 SamisFN(0);
mat4 Ales_Transform(translate(vec3(0))); vec3 AlesFN(0);



struct Vector8f {
	vec3 pos, norm; vec2 tex;
};




Vector8f corners2[8] = {
	Vector8f{ vec3(-0.5, -0.5, -0.5), vec3(0, -1.0f, 0), vec2(0.0f, 0.0f) }, // Lower back left -- 0Cm
	Vector8f{ vec3(0.5, -0.5, -0.5), vec3(0, -1.0f, 0), vec2(1.0f, 0.0f) }, // Lower back right -- 1
	Vector8f{ vec3(0.5, -0.5, 0.5),vec3(0, -1, 0), vec2(1.0f, 1.0f) }, // Lower front right -- 2
	Vector8f{ vec3(-0.5, -0.5, 0.5),vec3(0, -1, 0), vec2(0.0f, 1.0f) }, // Lower front left -- 3

																		// ----- Top
																		// Indices/IDs
																		Vector8f{ vec3(-0.5, 0.5, -0.5),vec3(0, 1, 0), vec2(0.0f, 1.0f) }, // Upper back left -- 4
																		Vector8f{ vec3(0.5, 0.5, -0.5),vec3(0, 1, 0), vec2(1.0f, 1.0f) }, // Upper back right -- 5
																		Vector8f{ vec3(0.5, 0.5, 0.5),vec3(0, 1, 0), vec2(1.0f, 1.0f) }, // Upper front right -- 6
																		Vector8f{ vec3(-0.5, 0.5, 0.5), vec3(0, 1, 0),vec2(0.0f, 1.0f) }, // Upper front left -- 7
};


// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{


	float widthRatio = SCR_WIDTH / 1024.0, heightRatio = SCR_HEIGHT / 768.0;
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void hdrBufferStuff(unsigned int hdrFBO, unsigned int colorBuffers[2], unsigned int rboDepth, unsigned int pingpongFBO[2], unsigned int pingpongColorbuffers[2]) {

	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	// create 2 floating point color buffers (1 for normal rendering, other for brightness treshold values)
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ping-pong-framebuffer for blurring
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

}


void handleWindowResize(UI_InputManager * UIM) {
	/*
	Handle logic for resizing
	*/
	int width, height;
	glfwGetWindowSize(mWind, &width, &height);
	float fWidth = (float)width, fHeight = (float)height;
	float aspectRatio = mainWindow->getAspectRatio();

	if (width >= height) {
		fWidth = (float)height * aspectRatio;
	}
	else {
		fHeight = (float)width / aspectRatio;

	}

	UIM->renderFrameHeight = height;
	UIM->renderFrameWidth = width;

	glViewport(0, 0, fWidth, fHeight);
	SCR_HEIGHT = fHeight; SCR_WIDTH = fWidth;

}
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

float mod289(float x) { return x - floor(x * (1.0f / 289.0f)) * 289.0f; }
vec4 mod289(vec4 x) { return x - floor(x * (1.0f / 289.0f)) * 289.0f; }
vec4 perm(vec4 x) { return mod289(((x * 34.0f) + 1.0f) * x); }


unsigned int * NVIDIA_sampleable_map_idea;
void createSampleableTexture(std::string path, bool alpha)
{
	// Load in the height map from the image
	int iWidth, iHeight, channels;
	std::string imagePath = "../HeightMaps/" + path + ".png";

	unsigned char *image = stbi_load(imagePath.c_str(), &iWidth, &iHeight, &channels,
		alpha ? STBI_rgb_alpha : STBI_rgb);
	NVIDIA_sampleable_map_idea = new unsigned int[iWidth * iHeight];

	size_t img_size = iWidth * iHeight * channels;
	unsigned char *new_image = new unsigned char[img_size];
	int i = 0;
	int k = 0;
	for (unsigned char *p = image; p != image + img_size; p += channels) {
		unsigned int height = static_cast<unsigned int> (*(p));
		*(NVIDIA_sampleable_map_idea + k++) = height;
	}
	stbi_image_free(image);
}

char densityFunction(vec3 wsc) { // wsc = World Space Coordinates

	char density;
	density = -wsc.y;

	return density;
}
float randomFloat(float LO, float HI) {
	return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
}
char noise(vec3 p) {

	srand(glfwGetTime());
	float r = randomFloat(1.0f, 30.0f);
	p *= r;
	vec3 a = floor(p);


	vec3 d = p - a;
	d = d * d * (3.0f - 2.0f * d);

	vec4 b = vec4(a.x, a.x, a.y, a.y) + vec4(0.0, 1.0, 0.0, 1.0);
	vec4 k1 = perm(vec4(b.x, b.y, b.x, b.y));
	vec4 k2 = perm(vec4(k1.x, k1.y, k1.x, k1.y) + vec4(b.z, b.z, b.w, b.w));

	vec4 c = k2 + vec4(a.z, a.z, a.z, a.z);
	vec4 k3 = perm(c);
	vec4 k4 = perm(c + 1.0f);

	vec4 o1 = fract(k3 * (1.0f / 41.0f));
	vec4 o2 = fract(k4 * (1.0f / 41.0f));

	vec4 o3 = o2 * d.z + o1 * (1.0f - d.z);
	vec2 o4 = vec2(o3.y, o3.w) * d.x + vec2(o3.x, o3.z) * (1.0f - d.x);

	return 255 * (o4.y * d.y + o4.x * (1.0 - d.y));
}

// Method that loads the textures for the models generated in the world.
bool load2DTexture(unsigned int * textureID, std::string fileName, bool verticallyFlipped) {

	glGenTextures(1, textureID);
	glBindTexture(GL_TEXTURE_2D, *textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	fileName.insert(0, "../Textures/");
	const char * filePath = fileName.c_str();
	int Texture_Width, Texture_Height, Texture_Nr_Chans;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *Tile_Texture_Image_Data_Loaded = stbi_load(filePath, &Texture_Width, &Texture_Height, &Texture_Nr_Chans, STBI_rgb);
	if (Tile_Texture_Image_Data_Loaded)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Texture_Width, Texture_Height, Texture_Width, GL_RGB, GL_UNSIGNED_BYTE, Tile_Texture_Image_Data_Loaded);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(Tile_Texture_Image_Data_Loaded);

		return true;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		stbi_image_free(Tile_Texture_Image_Data_Loaded);
		return false;
	}

}

// This is a function we created to sort the models based on their distance to the camera so that they are drawn in a way that their transparency and stencil outlines don't conflict
void sortModelsByDistanceToCamera(std::vector<HierarchicalModel * > & v) {

	int i, j;
	for (i = 0; i < v.size() - 1; i++)

		// Last i elements are already in place  
		for (j = 0; j < v.size() - i - 1; j++)
			if (length(v[j]->bubble.center - camera.Position) <= length(v[j + 1]->bubble.center - camera.Position)) {
				HierarchicalModel * t = v[j];
				v[j] = v[j + 1];
				v[j + 1] = t;
			}
}

// Function to render coordinate axes
void drawCoordinateAxes(Line L, Shader S, GLint primitiveType, mat4 viewMatrix, mat4 projectionMatrix) {

	// DRAW THE COORDINATE AXES
	S.use();
	S.setMat4("projection", projectionMatrix);
	S.setMat4("world", worldMatrix);
	S.setMat4("view", viewMatrix);
	S.setMat4("transform", mat4(1.0f));

	// Draw the Y axis vector line
	S.setVec3("ourColor", vec3(0.0f, 1.0f, 0.0f));
	L.Draw();

	// Draw the X axis vector line
	L.setTransform(rotate(*L.getTransformMatrix(), -radians(90.0f), vec3(0, 0, 1)));
	S.setMat4("transform", *L.getTransformMatrix());
	S.setVec3("ourColor", vec3(1.0f, 0.0f, 0.0f));
	L.Draw();
	L.resetTransform();

	// Draw the Z axis vector line
	L.setTransform(rotate(*L.getTransformMatrix(), radians(90.0f), vec3(1, 0, 0)));
	S.setMat4("transform", *L.getTransformMatrix());
	S.setVec3("ourColor", vec3(1.0f, 1.0f, 0.0f));
	L.Draw();
	L.resetTransform();
	L.setTransform(mat4(1.0f));
}

// Function to normalize the speed relative to framerate
float normalizeSpeed(float FPS, float referenceFPS, float movementIncrement) {
	float FPS_ratio = referenceFPS / FPS;
	return FPS_ratio * movementIncrement;
}
float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }
float smoothstep(float min, float max, float value) {
	float v1 = 0.0f;
	float v2 = (value - min) / (max - min) < 1.0f ? (value - min) / (max - min) : 1.0f;
	float x = v2 < 0.0f ? v2 : 0.0f;
	return x * x*(3 - 2 * x);
};


vec3 noise2(vec2 x) {
	vec2 i = floor(x);
	vec2 f = fract(x);

	// Four corners in 2D of a tile
	float a = hash(i);
	float b = hash(i + vec2(1.0, 0.0));
	float c = hash(i + vec2(0.0, 1.0));
	float d = hash(i + vec2(1.0, 1.0));


	// Simple 2D lerp using smoothstep envelope between the values.
	return vec3(mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
		mix(c, d, smoothstep(0.0, 1.0, f.x)),
		smoothstep(0.0, 1.0, f.y)));

}
float dddnoise(vec2 x) {
	vec2 i = floor(x);
	vec2 f = fract(x);

	// Four corners in 2D of a tile
	float a = hash(i);
	float b = hash(i + vec2(1.0, 0.0));
	float c = hash(i + vec2(0.0, 1.0));
	float d = hash(i + vec2(1.0, 1.0));



	// Same code, with the clamps in smoothstep and common subexpressions
	// optimized away.
	vec2 u = f * f * (3.0f - 2.0f * f);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}
void loadCubeDepthMap(unsigned int & depthMapFBO, unsigned int & depthCubeMap) {
	glGenFramebuffers(1, &depthMapFBO);


	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
	for (unsigned int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


float * vertices;

const uint8_t xMag = 6;
const uint8_t yMag = 6;
const uint8_t zMag = 6;
const float chunkScale = 1;

vec3 MCResolution(xMag, yMag, zMag);


// We are assuming there are a maximum of 4 triangular faces in an XYZ cube coordinate


void setDensities(MarchingCubeChunk & chunk, vec3 center) {
	chunk.center = center; 
	int numDensities = 0;
	for (int x = (int)center.x - xMag / 2; x < (int)center.x + xMag / 2; ++x) for (int y = (int)center.y - yMag / 2; y < (int)center.y + yMag / 2; ++y) for (int z = (int)center.z - zMag / 2; z < (int)center.z + zMag / 2; ++z) {
		vec3 startPos(x, y, z);
		for (int i = 0; i < 8; ++i) {
			vec3 v = startPos + corners2[i].pos;
			char density = noise(v);
			chunk.functionValues[x - (int)center.x + xMag / 2][y - (int)center.y + yMag / 2][z - (int)center.z + zMag / 2][i] = density;
		}

	}
}

MarchingCubeChunk chunks[3][3][3];
std::vector<MarchingCubeChunk *> chunksToSwap;
std::vector<MarchingCubeChunk *> chunksToCheckForCollision;

bool firstone = true;
void marchCubes(MarchingCubeChunk & chunk, vec3 center, float surfaceThreshold, bool centerChunk = false, bool firstIteration = false) {

	int totalFaceCount = 0;
	int realTotalFaceCount = 0;
	if (!firstone) delete[xMag*yMag*zMag * 128] vertices;
	if (!chunk.firstGeneration) {
		delete[xMag*yMag*zMag * 7] chunk.faces;
		if (!chunk.crystalTransforms.empty()) chunk.crystalTransforms.clear();
		if (!chunk.crystalLights.empty()) chunk.crystalLights.clear();
		if (!chunk.crystalPositions.empty()) chunk.crystalPositions.clear();
	}
	else chunk.firstGeneration = false;

	firstone = false;

	chunk.vertexCount = 0;
	vertices = new float[xMag*yMag*zMag * 128];

	chunk.faces = new MarchingCubeChunk::face[xMag*yMag*zMag * 7];
	uint8_t xMag = MCResolution.x;
	uint8_t yMag = MCResolution.y;
	uint8_t zMag = MCResolution.z;
	int crystCount = 0;
	for (int x = (int)center.x - xMag / 2; x < (int)center.x + xMag / 2; ++x) for (int y = (int)center.y - yMag / 2; y < (int)center.y + yMag / 2; ++y) for (int z = (int)center.z - zMag / 2; z < (int)center.z + zMag / 2; ++z) {

		vec3 startPos(x, y, z);
		unsigned char cubeIndex = 0;
		for (unsigned int i = 0; i < 8; ++i)
			if (chunk.functionValues[x - (int)center.x + xMag / 2][y - (int)center.y + yMag / 2][z - (int)center.z + zMag / 2][i] < surfaceThreshold) cubeIndex |= 1 << i;

		chunk.cubeIndexes[x - (int)center.x + xMag / 2][y - (int)center.y + yMag / 2][z - (int)center.z + zMag / 2][0] = cubeIndex;

		int * triangulation = MarchingCubes::triTable[chunk.cubeIndexes[x - (int)center.x + xMag / 2][y - (int)center.y + yMag / 2][z - (int)center.z + zMag / 2][0]];
		int faceCount = 0;
		int realFaceCount = 0;
		bool placedCryst = false;

		for (unsigned int i = 0; i < 16; ++i) {

			int8_t edgeIndex = triangulation[i];
			if (edgeIndex == -1) continue;
			char indexA = MarchingCubes::cornerIndexFromEdge[edgeIndex][0];
			char indexB = MarchingCubes::cornerIndexFromEdge[edgeIndex][1];

			vec3 newPos = startPos + corners2[indexA].pos +
				//	(corners2[indexA].pos + corners2[indexB].pos) / 2.0f;
				(surfaceThreshold - chunk.functionValues[x - (int)center.x + xMag / 2][y - (int)center.y + yMag / 2][z - (int)center.z + zMag / 2][indexA]) * (corners2[indexB].pos - corners2[indexA].pos) /
				(float)(chunk.functionValues[x - (int)center.x + xMag / 2][y - (int)center.y + yMag / 2][z - (int)center.z + zMag / 2][indexB] - chunk.functionValues[x - (int)center.x + xMag / 2][y - (int)center.y + yMag / 2][z - (int)center.z + zMag / 2][indexA]);
			//vec2 newTex = corners2[indexA].tex + (surfaceThreshold - functionValues[x + xMag / 2][y + yMag / 2][z + zMag / 2][indexA]) * (corners2[indexB].tex - corners2[indexA].tex) / (float)(functionValues[x + xMag / 2][y + yMag / 2][z + zMag / 2][indexB] - functionValues[x + xMag / 2][y + yMag / 2][z + zMag / 2][indexA]);
			//vec3 newNorm(0);
			newPos *= chunkScale;
			vertices[chunk.vertexCount * 8] = newPos.x; vertices[chunk.vertexCount * 8 + 1] = newPos.y; vertices[chunk.vertexCount * 8 + 2] = newPos.z;
			vertices[chunk.vertexCount * 8 + 3] = 0; vertices[chunk.vertexCount * 8 + 4] = 0; vertices[chunk.vertexCount * 8 + 5] = 0;
			vertices[chunk.vertexCount * 8 + 6] = 0; vertices[chunk.vertexCount * 8 + 7] = 0; // for now, useless tex coords.
			++chunk.vertexCount;
			if (chunk.vertexCount % 3 == 0 && chunk.vertexCount > 0) {
				// Calculate face normal
				// These are the vertices of one of the faces in this X,Y,Z cube. Add them to the face index
				float v1x = vertices[chunk.vertexCount * 8 - 8], v1y = vertices[chunk.vertexCount * 8 - 7], v1z = vertices[chunk.vertexCount * 8 - 6];
				float v2x = vertices[(chunk.vertexCount - 1) * 8 - 8], v2y = vertices[(chunk.vertexCount - 1) * 8 - 7], v2z = vertices[(chunk.vertexCount - 1) * 8 - 6];
				float v3x = vertices[(chunk.vertexCount - 2) * 8 - 8], v3y = vertices[(chunk.vertexCount - 2) * 8 - 7], v3z = vertices[(chunk.vertexCount - 2) * 8 - 6];
				vec3 vec1(v3x - v2x, v3y - v2y, v3z - v2z);
				vec3 vec2(v1x - v2x, v1y - v2y, v1z - v2z);

				vec3 faceNormal = glm::cross(vec1, vec2);
				
				
				vec3 fv1(v1x, v1y, v1z);
				vec3 fv2(v2x, v2y, v2z);
				vec3 fv3(v3x, v3y, v3z);
				// Tell the chunk about this face
				chunk.faces[(((x - (int)center.x + xMag / 2)*xMag + (y - (int)center.y + yMag / 2))*yMag + (z - (int)center.z + zMag / 2)) * 5 + faceCount++] = MarchingCubeChunk::face{fv1,fv2, fv3, faceNormal };

				// Determine if a crystal is on it, and if so define it
				if (x - (int)center.x == 2 && y - (int)center.y == -1 && z  - (int)center.z== -1  &&!placedCryst && crystCount < 4) {
					vec3 b3d = (1.0f / 3.0f)*(fv1 + fv2 + fv3);
					placedCryst = true;
					crystCount++;
					quat rotatesModelToSurfaceNormal;
					vec3 a = vec3(0, 0, 1);
					float flipDot = dot(camera.Front, faceNormal);

					vec3 b = normalize(flipDot >= 0 ? -faceNormal : faceNormal);
					vec3 ax = cross(a, b);

					quat identity(vec3(0.0f));
					rotatesModelToSurfaceNormal.x = ax.x;
					rotatesModelToSurfaceNormal.y = ax.y;
					rotatesModelToSurfaceNormal.z = ax.z;
					mat4 rotMat = rotate(mat4(1.0f), orientedAngle(a, b, ax), ax);
					rotatesModelToSurfaceNormal.w = sqrt((length(a) * length(a)) * (length(b) * length(b))) + dot(a, b);
					quat movesModelToSurface2 = slerp(identity, rotatesModelToSurfaceNormal, 1.0f);

					chunk.crystalTransforms.push_back(translate(b3d) * rotMat);
					//vec3 lightCol(noise(vec3(b3d.x,b3d.z,b3d.y)),noise(vec3(b3d.z,b3d.y,b3d.z)), noise(vec3(b3d.y,b3d.z,b3d.y)));
					vec3 lightCol(0.64, 0.34, 0.72);
					glm::vec2 atten(linear, quadratic);
					chunk.crystalLights.push_back(Shader::PointLight{ b3d, lightCol, lightCol, lightCol, atten, 100.0f });

					chunk.crystalFaceNormal = b;
					chunk.crystalPosition = b3d;
					
				}

				if (centerChunk && x - (int)center.x == 1 && y - (int)center.y == 2 && z - (int)center.z == 0) {

					// Set nicks model position
					vec3 b3d = (1.0f / 3.0f)*(fv1 + fv2 + fv3);
					quat rotatesModelToSurfaceNormal;
					vec3 a = vec3(0, 1, 0);
					vec3 b = normalize(faceNormal);
					vec3 ax = cross(a, b);
					quat identity(vec3(0.0f));
					rotatesModelToSurfaceNormal.x = ax.x;
					rotatesModelToSurfaceNormal.y = ax.y;
					rotatesModelToSurfaceNormal.z = ax.z;
					mat4 rotMat = rotate(mat4(1.0f), orientedAngle(a, b, ax), ax);
					rotatesModelToSurfaceNormal.w = sqrt((length(a) * length(a)) * (length(b) * length(b))) + dot(a, b);
					quat movesModelToSurface2 = slerp(identity, rotatesModelToSurfaceNormal, 1.0f);

					Nicks_Transform = mat4(translate(b3d) * rotMat);
					NicksFN = b;
				}
				
				if (centerChunk && x - (int)center.x == 1 && y - (int)center.y == 2 && z - (int)center.z == 1) {

					// Set nicks model position
					vec3 b3d = (1.0f / 3.0f)*(fv1 + fv2 + fv3);
					quat rotatesModelToSurfaceNormal;
					vec3 a = vec3(0, 1, 0);
					vec3 b = normalize(faceNormal);
					vec3 ax = cross(a, b);
					quat identity(vec3(0.0f));
					rotatesModelToSurfaceNormal.x = ax.x;
					rotatesModelToSurfaceNormal.y = ax.y;
					rotatesModelToSurfaceNormal.z = ax.z;
					mat4 rotMat = rotate(mat4(1.0f), orientedAngle(a, b, ax), ax);
					rotatesModelToSurfaceNormal.w = sqrt((length(a) * length(a)) * (length(b) * length(b))) + dot(a, b);
					quat movesModelToSurface2 = slerp(identity, rotatesModelToSurfaceNormal, 1.0f);

					Ales_Transform = mat4(translate(b3d) * rotMat);
					AlesFN = b;
				}
				
				if (centerChunk && x - (int)center.x == 2 && y - (int)center.y == 2 && z - (int)center.z == 2) {

					// Set nicks model position
					vec3 b3d = (1.0f / 3.0f)*(fv1 + fv2 + fv3);
					quat rotatesModelToSurfaceNormal;
					vec3 a = vec3(0, 1, 0);
					vec3 b = normalize(faceNormal);
					vec3 ax = cross(a, b);
					quat identity(vec3(0.0f));
					rotatesModelToSurfaceNormal.x = ax.x;
					rotatesModelToSurfaceNormal.y = ax.y;
					rotatesModelToSurfaceNormal.z = ax.z;
					mat4 rotMat = rotate(mat4(1.0f), orientedAngle(a, b, ax), ax);
					rotatesModelToSurfaceNormal.w = sqrt((length(a) * length(a)) * (length(b) * length(b))) + dot(a, b);
					quat movesModelToSurface2 = slerp(identity, rotatesModelToSurfaceNormal, 1.0f);

					Mohamads_Transform = mat4(translate(b3d) * rotMat);
					MohamadsFN = b;
				}
				
				if (centerChunk && x - (int)center.x == 2 && y - (int)center.y == 1 && z - (int)center.z == 0) {

					// Set nicks model position
					vec3 b3d = (1.0f / 3.0f)*(fv1 + fv2 + fv3);
					quat rotatesModelToSurfaceNormal;
					vec3 a = vec3(0, 1, 0);
					vec3 b = normalize(faceNormal);
					vec3 ax = cross(a, b);
					quat identity(vec3(0.0f));
					rotatesModelToSurfaceNormal.x = ax.x;
					rotatesModelToSurfaceNormal.y = ax.y;
					rotatesModelToSurfaceNormal.z = ax.z;
					mat4 rotMat = rotate(mat4(1.0f), orientedAngle(a, b, ax), ax);
					rotatesModelToSurfaceNormal.w = sqrt((length(a) * length(a)) * (length(b) * length(b))) + dot(a, b);
					quat movesModelToSurface2 = slerp(identity, rotatesModelToSurfaceNormal, 1.0f);

					Adriens_Transform = mat4(translate(b3d) * rotMat);
					AdriensFN = b;
				}
				
				if (centerChunk && x - (int)center.x == 2 && y - (int)center.y == -3 && z - (int)center.z == -3) {

					// Set nicks model position
					vec3 b3d = (1.0f / 3.0f)*(fv1 + fv2 + fv3);
					quat rotatesModelToSurfaceNormal;
					vec3 a = vec3(0, 1, 0);
					vec3 b = normalize(faceNormal);
					vec3 ax = cross(a, b);
					quat identity(vec3(0.0f));
					rotatesModelToSurfaceNormal.x = ax.x;
					rotatesModelToSurfaceNormal.y = ax.y;
					rotatesModelToSurfaceNormal.z = ax.z;
					mat4 rotMat = rotate(mat4(1.0f), orientedAngle(a, b, ax), ax);
					rotatesModelToSurfaceNormal.w = sqrt((length(a) * length(a)) * (length(b) * length(b))) + dot(a, b);
					quat movesModelToSurface2 = slerp(identity, rotatesModelToSurfaceNormal, 1.0f);

					Samis_Transform = mat4(translate(b3d) * rotMat);
					SamisFN = b;
				}








				vec3 t((x - (int)center.x + xMag / 2), (y - (int)center.y + yMag / 2), (z - (int)center.z + zMag / 2));
				
				//std::cout << "Positions created for collision: " << to_string(t) << std::endl;
				int ind = chunk.vertexCount * 8;
				faceNormal = normalize(faceNormal);

				// Set the normals retroactively
				vertices[ind - 5] = faceNormal.x; vertices[ind - 4] = faceNormal.y; vertices[ind - 3] = faceNormal.z;
				vertices[ind - 13] = faceNormal.x; vertices[ind - 12] = faceNormal.y; vertices[ind - 11] = faceNormal.z;
				vertices[ind - 21] = faceNormal.x; vertices[ind - 20] = faceNormal.y; vertices[ind - 19] = faceNormal.z;
			}

		}
		// Now we have determined all the vertices for this x,y,z coordinate.
		totalFaceCount += faceCount;
		realTotalFaceCount += realFaceCount;
	}
}

void marchCubesToGPU(MarchingCubeChunk & chunk) {
	glBindVertexArray(chunk.vao);
	glBindBuffer(GL_ARRAY_BUFFER, chunk.vbo);
	glBufferData(GL_ARRAY_BUFFER, chunk.vertexCount * 8 * sizeof(float), vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	//delete[xMag*yMag*zMag*128] vertices;
}

void renewBlurBuffers(unsigned int hdrFBO, unsigned int colorBuffers[2], unsigned int rboDepth, unsigned int pingpongFBO[2], unsigned int pingpongColorbuffers[2]) {
	// configure (floating point) framebuffers
	// ---------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	// create 2 floating point color buffers (1 for normal rendering, other for brightness treshold values)
	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}
	// create and attach depth buffer (renderbuffer)
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ping-pong-framebuffer for blurring
	//glGenFramebuffers(2, pingpongFBO);
	//glGenTextures(2, pingpongColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}

}


int main() {


	SkinnedMesh sharkModel;
	SkinnedMesh submarineModel;
	submarineModel.LoadMesh("../Models/testSubmarine.fbx");
	sharkModel.LoadMesh("../Models/aShark2.fbx");
	sharkModel.setMaterial(blueRubber);
	player.linkModel(&sharkModel);

	SkinnedMesh crystalModel;
	crystalModel.LoadMesh("../Models/testCrystal.fbx");

#pragma region FBO, texture for shadowing


	// LOAD IN TEXTURES
	unsigned int tileTextureID1;
	load2DTexture(&tileTextureID1, "TileTexture_3.jpg", true);

	unsigned int woodBoxTextureID1;
	load2DTexture(&woodBoxTextureID1, "NickBoxTexture_1.jpg", true);

	unsigned int testTexture_2;
	load2DTexture(&testTexture_2, "testImage_1.jpg", true);

	unsigned int tileTextureID2;
	load2DTexture(&tileTextureID2, "TileTexture_1.jpg", true);

	unsigned int tileSpecMapID1;
	load2DTexture(&tileSpecMapID1, "TileSpecMap_3.jpg", true);

	unsigned int lavaTextureID1;
	load2DTexture(&lavaTextureID1, "LavaTexture_1.jpg", true);

	unsigned int iceTextureID1;
	load2DTexture(&iceTextureID1, "IceTexture_1.jpg", true);

	unsigned int blueGlassTextureID1;
	load2DTexture(&blueGlassTextureID1, "BlueStainedTexture_1.jpg", true);

	unsigned int blackTileTextureID1;
	load2DTexture(&blackTileTextureID1, "BlackTileTexture_1.jpg", true);

	unsigned int depthCubeMap;
	glGenTextures(1, &depthCubeMap);
	// CUBIC DEPTH MAP INSTANTIATION FOR THE POINT LIGHT SHADOWING
	unsigned int depthMapFBO;
	loadCubeDepthMap(depthMapFBO, depthCubeMap);
#pragma endregion

#pragma region Instantiate our resource and input managers
	UI_InputManager UIM(mWind, player, &camera, SoundEngine);

#pragma endregion

#pragma region Set up ImGui and any variables it will be referencing.
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	const char* glsl_version = "#version 330";

	ImGui::CreateContext();
	ImGuiIO& mainMenu = ImGui::GetIO(); (void)mainMenu;
	mainMenu.IniFilename = NULL; // Get rid of the save file

								 // Setup Dear ImGui theme color
	ImGui::StyleColorsDark();
	// ------------- VALUES CONTROLLED BY THE GUI -------------------
	ImGui_ImplGlfw_InitForOpenGL(mWind, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 lightAmbient = ImVec4(0.044f, .078f, .171f, 1.0f);
	ImVec4 lightDiffuse = ImVec4(0.209f, 0.209f, 0.629f, 1.0f);
	ImVec4 lightSpecular = ImVec4(0.0f, 0.0f, 0.1f, 1.0f);
	float far = 5.0f, near = 0.001f;
	float lightIntensity = 300.0f;
	
	float waveSpread = 10, waveMagnitude = 3;
	float textureMovementX = 0, textureMovementZ = 0;
	float heightScalingFactor = 1.0f;
	int waveType = 0;
	int heightMapResolution = 0;

	srand(glfwGetTime() * rand() * 21384239.12384f);


	// Variables for debugging environment
	bool applyTest1 = false, applyTest2 = false;

	//////////////////////////////////////////////////////////////////
#pragma endregion

#pragma region Create the unit "building block" meshes that we use for other things (cubes,spheres,etc.)
	mat4 Sun_Object_Transform(translate(vec3(15, 30, 30)));

#pragma endregion


	// Keep track of the 
	float prevHeight = SCR_HEIGHT, prevWidth = SCR_WIDTH;



	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	// Set the noise values for the vertives
	// Generate the surfaces

	
	for (int x = -1; x <= 1; ++x) {
	for (int y = -1; y <= 1; ++y) {
	for (int z = -1; z <= 1; ++z) {

		glGenBuffers(1, &chunks[x+1][y+1][z+1].vbo);
		glGenVertexArrays(1, &chunks[x + 1][y + 1][z + 1].vao);

		vec3 center = vec3(x * 6, y * 6, z * 6);



		setDensities(chunks[x + 1][y + 1][z + 1], center);
		marchCubes(chunks[x + 1][y + 1][z + 1], center, surfaceThreshold, x == 0 && y == 0 && z == 0);
		marchCubesToGPU(chunks[x + 1][y + 1][z + 1]);
		}
	}
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	// Now, for every face of the terrain, we generate a mesh of points

	int numFaces = xMag * yMag*zMag * 7;


	int mcrX = MCResolution.x;
	int mcrY = MCResolution.y;
	int mcrZ = MCResolution.z;
	bool faceNormals = true;

	vec3 positionVector(0, 0, 0);
	vec3 velocityVector(1, 0, 0);
	vec3 accelerationVector(0, 0, 0);
	float vx = 0.0f, vy = 0.0f, vz = 0.0f;
	float cFrame = glfwGetTime();


	float testTime1, testTime2;
	float timeFactor = 30.0f;
	ImVec4 testAxis = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

//	SoundEngine->play2D(musicArea1, true);




	// AI DEBUGGING
	float cohesionStrength = 0.0f;
	float alignmentStrength = 0.0f;
	float separationStrength = 0.0f;
	float tenacityFactor = 0.0f;

	float QSLAF = 0.0000006f;
	float LSLAF = 0.0000175f;

	////////////////////////////////////////////////////////////// RENDER LOOP STARTS HERE //////////////////////////////////////////////////////////////////////
	std::vector<vec3> collisionCasts;

	float PI = 3.14159;

	float x, y, z, xy;                              // vertex position
	float sectorStep = 2 * PI / 2;
	float stackStep = PI / 2;
	float sectorAngle, stackAngle;

	unsigned int v = 0;
	for (int i = 0; i <= 2; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = 1 * cosf(stackAngle);             // r * cos(u)
		z = 1 * sinf(stackAngle);              // r * sin(u)
		for (int j = 0; j <= 2; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			collisionCasts.push_back(vec3(x, y, z));
		}
	}

	bool testValue1 = true, testValue2 = true;
	float testFloat1 = 1.0f, testFloat2 = 2.75f, testFloat3 = 0.0f;

	// configure (floating point) framebuffers
	// ---------------------------------------
	unsigned int hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	// create 2 floating point color buffers (1 for normal rendering, other for brightness treshold values)
	unsigned int colorBuffers[2];
	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}
	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ping-pong-framebuffer for blurring
	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}

	// shader configuration
	// --------------------
	bloomFinalShader.use();
	bloomFinalShader.setInt("scene", 0);
	bloomFinalShader.setInt("bloomBlur", 1);

		vec3 nearestChunkCenter = getNearestChunkCenter(camera.Position, chunkScale);


	
	vec3 playerModelPositions[5] = { Nicks_Transform[3], Mohamads_Transform[3], Ales_Transform[3], Adriens_Transform[3], Samis_Transform[3] };
	vec3 playerModelFaceNormals[5] = { NicksFN, MohamadsFN, AlesFN, AdriensFN, SamisFN };


	
	while (!glfwWindowShouldClose(mWind))
	{
#pragma region Define frame-specific constants




		// UIM dependent vars
		const GLint & primitiveType = UIM.primitiveType;



		// Camera dependent vars
		const mat4 & cameraProjection = perspective(radians(camera.Zoom), mainWindow->getAspectRatio(), near, far);
		const mat4 & cameraView = camera.GetViewMatrix();

		// FPS & Time dependent vars
		float FPS = ImGui::GetIO().Framerate;
		float normalizedSpeed = normalizeSpeed(FPS, 60.0f, 0.001f);
		float time = glfwGetTime();
		float prevFrame = cFrame;
		cFrame = time;
#pragma endregion

#pragma region Start and prep Dear ImGui
		// Start Dear ImGui 
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		bool regen = false;
		// Handle input from GUI to assign to relevant variables
		static float lightX = 0, lightY = 10, lightZ = -10;
		{

			ImGui::Begin("Control GUI");



			ImGui::SetWindowPos(ImVec2(0, 0), 2);
			ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x / 4.0, ImGui::GetIO().DisplaySize.y / 2.0), 2);
			ImGui::SetWindowCollapsed(true, 2);

			if (ImGui::CollapsingHeader("HDR & Blur")) {

				ImGui::SliderFloat("HDR Exposure", &testFloat1, 0.0f, 10.0f);
				ImGui::SliderFloat("Blur Spread", &testFloat2, -10.0f, 20.0f);

				if (ImGui::Button(!applyTest1 ? "Enable Blur" : "Disable Blur")) {
					testTime1 = glfwGetTime();
					applyTest1 = !applyTest1;
				}
			}

			if (ImGui::Button(!applyTest2 ? "Enable Particles" : "Disable Particles")) {
				testTime1 = glfwGetTime();
				applyTest2 = !applyTest2;
			}
			if (ImGui::CollapsingHeader("Metrics")) {
				ImGui::Text("Metrics");
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / FPS, FPS);
			}
			ImGui::End();
		}

		// Make sure we arent zooming, panning, tilting when the user is clicking in the GUI window.
		if (ImGui::GetIO().WantCaptureMouse && ImGui::GetIO().MouseClicked) {
			UIM.hoveringOnGUI = true;
		}
		else UIM.hoveringOnGUI = false;
		camera.UpdateProjection(near, far);
#pragma endregion

		vec3 shadowResult = extractRotation(worldMatrix) * Sun_Object_Transform[3];

		// Set the light source position to the chosen value in the GUI window
		Sun_Object_Transform[3].x = lightX;
		Sun_Object_Transform[3].y = lightY;
		Sun_Object_Transform[3].z = lightZ;

		handleWindowResize(&UIM);


		float waterDensity = camera.Position.y >= 0 ? camera.Position.y : 0;
		float ncwf = 0.1f*waterDensity / 10000.0f;// Natural Water Color Factor (dependent on player position in Y)
		float distanceFromSurface = 10000.0f - camera.Position.y;

		float quadraticSunlightAttenuation = 1.0f / (1.0f + LSLAF * distanceFromSurface + QSLAF * (distanceFromSurface * distanceFromSurface));
		vec3 currentDistantWaterColor = vec3(0.21, 0.733, 0.94) + vec3(0, ncwf / 3.0, ncwf); currentDistantWaterColor *= quadraticSunlightAttenuation;

		Shader::ViewClipPlane VCP{ far, near };
		Shader::CameraViewInfo CVI{ camera.GetPosition(), cameraProjection, cameraView, currentDistantWaterColor };
		Shader::UserInputDefinedVariables UIDV{ true, true };
		Shader::PointLight SunLight{ Sun_Object_Transform[3],vec3(0.05),vec3(0.45),vec3(0.175), vec2(linear, quadratic), 100.0f };


		float deltaT = cFrame - prevFrame;
		mainWindow->clearColor(vec4(gammaCorrect(currentDistantWaterColor, 2.2), 1.0));

		hdrBufferStuff(hdrFBO,colorBuffers,rboDepth,pingpongFBO,pingpongColorbuffers);


		masterWorldShader.prepare(CVI, VCP, UIDV, SunLight);
		masterWorldShader.setModelMatrix(worldMatrix * scale(Sun_Object_Transform, vec3(1)));


		masterWorldShader.setBool("overrideColor", false);
		masterWorldShader.setMaterial(obsidian);

	
		vec3 oldChunkCenter = nearestChunkCenter;
		nearestChunkCenter = getNearestChunkCenter(camera.Position, chunkScale);
		SWAP_INFO chunkSwapInfo = findChunksToSwap(oldChunkCenter, nearestChunkCenter, chunkScale, chunksToSwap, chunks);
		if (!chunksToSwap.empty()) {
			// 1. Calculate densities for the new positions in chunks to swap

			for (int i = 0; i < chunksToSwap.size(); ++i) {
				vec3 offset(0);
				switch (chunkSwapInfo.axis) {
				case X:
					offset.x = 18;
					break;
				case Y:
					offset.y = 18;
					break;
				case Z:
					offset.z = 18;
					break;

				case NEGX:
					offset.x = -18;
					break;
				case NEGY:
					offset.y = -18;
					break;

				case NEGZ:
					offset.z = -18;
					break;

				}
				chunksToSwap[i]->center += offset;
				setDensities(*chunksToSwap[i], chunksToSwap[i]->center);
				// 2. Set the vertices
				marchCubes(*chunksToSwap[i], chunksToSwap[i]->center, surfaceThreshold);
				// 3. push to the gpu
				marchCubesToGPU(*chunksToSwap[i]);
			}
			chunksToSwap.clear();
		}


		MarchingCubeChunk & curChunk = currentChunk(nearestChunkCenter, chunks);
		// Draw the mesh
		marchingCubesShader.prepare(CVI, VCP, UIDV, SunLight);
		marchingCubesShader.setBool("isFadeableObject", true);
		marchingCubesShader.setInt("texture_diffuse", 0);
		marchingCubesShader.setInt("shadowMap", 1);
		marchingCubesShader.setMaterial("baseMaterial", blueRubber);
		marchingCubesShader.setModelMatrix(worldMatrix);

		
		for (int x = -1; x <= 1; ++x) {
			for (int y = -1; y <= 1; ++y) {
				for (int z = -1; z <= 1; ++z) {
					marchingCubesShader.use();
					marchingCubesShader.setCrystalLights(chunks[x + 1][y + 1][z + 1].crystalLights);
					marchingCubesShader.setFloat("NUM_CRYSTALS_IN_CHUNK", (float) chunks[x + 1][y + 1][z + 1].crystalLights.size());
					chunks[x + 1][y + 1][z + 1].Draw(&chunks[x+1][y+1][z+1] == &curChunk ? primitiveType : primitiveType, tileTextureID1, depthCubeMap);
					
					/*
					crystalLightShader.prepare(CVI, VCP, UIDV);
					crystalLightShader.setMaterial(esmeralda);

					for (int i = 0; i < chunks[x + 1][y + 1][z + 1].crystalTransforms.size(); ++i) {
						chunks[x + 1][y + 1][z + 1].crystalLights[i].attenuation  = vec2(linear, quadratic);

						crystalLightShader.setMat4("model", glm::scale(chunks[x+1][y+1][z+1].crystalTransforms[i],glm::vec3(0.1f)));
						crystalLightShader.setPointLights(chunks[x + 1][y + 1][z + 1].crystalLights[i]);
						crystalModel.Render(primitiveType, crystalLightShader);
					}
					*/
				}
			}
		}




		
		if (applyTest2) {
			falseFloatingParticlesShader.prepare(CVI, VCP, UIDV, SunLight);
			falseFloatingParticlesShader.setModelMatrix(worldMatrix);
			falseFloatingParticlesShader.setFloat("time", cFrame);
			for (int x = -1; x <= 1; ++x) {
				for (int y = -1; y <= 1; ++y) {
					for (int z = -1; z <= 1; ++z) {
						//	if (&chunks[x + 1][y + 1][z + 1] == &curChunk) std::cout << to_string(chunks[x + 1][y + 1][z + 1].center) << std::endl;
						chunks[x + 1][y + 1][z + 1].Draw(&chunks[x + 1][y + 1][z + 1] == &curChunk ? primitiveType : primitiveType, tileTextureID1, depthCubeMap);
					}
				}
			}
		}
		

		marchingCubesShader.setBool("isFadeableObject", false);
		//camera.Position = player.getPosition();// -5.0f * camera.Front; // vec3(0, 0, 5);
		

		

		vec3 whereWeAre = camera.Position;

		vec3 playerCollisionPosition = vec3((int)round(whereWeAre.x/ chunkScale) - nearestChunkCenter.x + xMag / 2,
			(int)round(whereWeAre.y / chunkScale) - nearestChunkCenter.y + yMag/2, (int)round(whereWeAre.z / chunkScale) - nearestChunkCenter.z + zMag/2);

		bool xNext = false, yNext = false, zNext = false;

		if (playerCollisionPosition.x == 6) {
			playerCollisionPosition.x = 0;
			xNext = true;
		}

		if (playerCollisionPosition.y == 6) {
			playerCollisionPosition.y = 0;
			yNext = true;
		}

		if (playerCollisionPosition.z == 6) {
			playerCollisionPosition.z = 0;
			zNext = true;
		}


		chunksToCheckForCollision.push_back(&curChunk);
		for (int x = 0; x < 3; x++) for (int y = 0; y < 3; y++) for (int z = 0; z < 3; z++) {
			if (chunks[x][y][z].center.x - nearestChunkCenter.x == 6) chunksToCheckForCollision.push_back(&chunks[x][y][z]);
			if (chunks[x][y][z].center.y - nearestChunkCenter.y == 6) chunksToCheckForCollision.push_back(&chunks[x][y][z]);
			if (chunks[x][y][z].center.z - nearestChunkCenter.z == 6) chunksToCheckForCollision.push_back(&chunks[x][y][z]);
			if (xNext && yNext) {
				if (chunks[x][y][z].center - nearestChunkCenter == vec3(6,6,0)) chunksToCheckForCollision.push_back(&chunks[x][y][z]);
			}		
			if (xNext && zNext) {
				if (chunks[x][y][z].center - nearestChunkCenter == vec3(6,0,6)) chunksToCheckForCollision.push_back(&chunks[x][y][z]);
			}
			if (yNext && zNext) {
				if (chunks[x][y][z].center - nearestChunkCenter == vec3(0, 6, 6)) chunksToCheckForCollision.push_back(&chunks[x][y][z]);
			}
		}

		int collisions = 0;

		vec3 collisionChunkIndices = getCollisionChunkIndices(camera.Position, nearestChunkCenter, chunkScale);
		for (int i = 0; i < 5; i++) {
			std::vector<MarchingCubeChunk::face> facesToCheck;
			for (MarchingCubeChunk * MCC : chunksToCheckForCollision) 
				facesToCheck.push_back(MCC->faces[((((int)playerCollisionPosition.x)*xMag + ((int)playerCollisionPosition.y))*yMag +
				((int)playerCollisionPosition.z)) * 5 + i]);

			for (MarchingCubeChunk::face f : facesToCheck)
				if (distance(f.v3, f.v2) > 0) {
					vec3 p1 = whereWeAre;
					vec2 baryCenter;
					float distance = 9999999.0f;
					for (int i = 0; i < collisionCasts.size(); ++i) {
						if (intersectRayTriangle(p1, normalize(collisionCasts[i]), f.v1, f.v2, f.v3, baryCenter, distance) && abs(distance) < 0.1f) {
							collisions++;
							if (!camera.collisionForward)
							{
								if (length(camera.Velocity) > 1.0f) {
								//	SoundEngine->play2D(grunt1, false);
								}
								SoundEngine->play2D(step1, false);
								camera.collisionForward = true;

								camera.Velocity = vec3(0.0f);
								break;
								/*
								vec3 reflectionDirection;
								reflectionDirection = (camera.Front) - 1.0f*(dot(camera.Front, f.normal)) * f.normal;
								camera.Front = normalize(reflectionDirection);
								vec3 halfAngle = cross(reflectionDirection, f.normal);

								if (halfAngle == vec3(0)) {
									camera.Front = normalize(f.normal);
									player.unsolvedCollision = true;
								}
								else {
									camera.Front = normalize(reflectionDirection);
									player.unsolvedCollision = true;
								}
								*/
							}
							break;
						}

						
					}

			}




		}
		if (camera.collisionForward && collisions == 0) camera.collisionForward = false;

		chunksToCheckForCollision.clear();
		camera.update(deltaT);




		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		bool horizontal = true, first_iteration = true;

		if (applyTest1) {
			// 2. blur bright fragments with two-pass Gaussian Blur 
			// --------------------------------------------------
			unsigned int amount = 6;
			blurShader.use();
			blurShader.setFloat("gaussSpread", testFloat2);
			glActiveTexture(GL_TEXTURE0);

			for (unsigned int i = 0; i < amount; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
				blurShader.setBool("horizontal", horizontal);
				glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
				renderQuad();
				horizontal = !horizontal;
				if (first_iteration)
					first_iteration = false;
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		}

		// 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
		// --------------------------------------------------------------------------------------------------------------------------
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		bloomFinalShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		if (applyTest1) glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
		bloomFinalShader.setInt("bloom", applyTest1);
		bloomFinalShader.setFloat("exposure", testFloat1);
		renderQuad();


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		// Process input and its effects
		glfwPollEvents();
		UIM.processInput(normalizedSpeed);

		// Swap the buffers
		glfwSwapBuffers(mWind);

	}

	// Properly clean/Delete all of GLFW's resources that were allocated
	glfwTerminate();

	return -1;

}
