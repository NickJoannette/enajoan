#include "Grid.h"
#include "stb_image.h"

Grid::Grid(unsigned int w, unsigned int l, bool mapYValuesToImage, std::string path, bool alpha)
{
	// Load in the height map from the image
	int iWidth, iHeight, channels;
	std::string imagePath = "../HeightMaps/" + path + ".png";

	unsigned char *image = stbi_load(imagePath.c_str(), &iWidth, &iHeight, &channels, 
		alpha ? STBI_rgb_alpha : STBI_rgb);
	unsigned int * heightMap = new unsigned int[iWidth * iHeight];
	if (mapYValuesToImage) {

		size_t img_size = iWidth * iHeight * channels;
		unsigned char *new_image = new unsigned char[img_size];
		int i = 0;
		int k = 0;
		for (unsigned char *p = image; p != image + img_size; p += channels) {
			unsigned int height = static_cast<unsigned int> (*(p));
			*(heightMap + k++) = height;
		}
		stbi_image_free(image);
	}
	// We need at least 2 vertices on either side for a square.
	if (w == 1 && l == 1) w = l = 2;
	width = w; length = l;

	// Create w*l*(3 + 2 + 3) floats. 3 floats for each vertex's 3D position vector, 3 for its normals, and 2 floats for its texture coordinate. 
	vertices = new float[w*l*8];

	// Algorithm for generating the plane's vertex data. The length of the grid is in the z axis, the width in the x axis.
	int c = 0, c2 = 0;
	for (int j = 0; j < l; j++) {
		float z = (float)j / (l - 1) - 0.5f;
		for (int i = 0; i < w; i++) {
			float x = (float)i / (w - 1) - 0.5f;
			float y = 0;
			//	Position coordinates
			*(vertices + c++) = x;
			*(vertices + c++) = mapYValuesToImage ? (float)heightMap[c2++]/1024.0f : 0;
			*(vertices + c++) = z;
			// Texture coordinates
			*(vertices + c++) = x + 0.5f;
			*(vertices + c++) = z + 0.5f;
			// Normal coordinates
			*(vertices + c++) = -5;
			*(vertices + c++) = -5;
			*(vertices + c++) = -5;
		}
	}

	// Algorithm for creating the indices to draw the grid in one pass.
	indices = new GLuint[6 * (w - 1)*(l - 1)];
	c = 0;
	int ct = 0;
	for (int i = 0; i < (w - 1)*(l - 1); i++, c++) {
		if ((c + 1) % w == 0) ++c;

		// Indices for one face
		*(indices + ct++) = c + l;
		*(indices + ct++) = c + 1;
		*(indices + ct++) = c;

		// Calculate its normal
		unsigned int v1Ind = 8 * (c + l);
		unsigned int v2Ind = 8 * (c + 1);
		unsigned int v3Ind = 8 * c;

		float  v1x, v1y, v1z, v2x, v2y, v2z, v3x, v3y, v3z;
		v1x = vertices[v1Ind], v1y = vertices[v1Ind + 1], v1z = vertices[v1Ind + 2];
		v2x = vertices[v2Ind], v2y = vertices[v2Ind + 1], v2z = vertices[v2Ind + 2];
		v3x = vertices[v3Ind], v3y = vertices[v3Ind + 1], v3z = vertices[v3Ind + 2];

		// Cross product
		float cx, cy, cz;

		cx = (v3y - v1y) * (v2z - v1z) - (v3z - v1z) * (v2y - v1y);
		cy = (v3x - v1x) * (v2z - v1z) - (v3z - v1z) * (v2x - v1x);
		cz = (v3x - v1x) * (v2y - v1y) - (v3y - v1y) * (v2x - v1x);

		float c2x, c2y, c2z;

		c2x = (v1y - v2y) * (v3z - v2z) - (v1z - v2z) * (v3y - v2y);
		c2y = (v1x - v2x) * (v3z - v2z) - (v1z - v2z) * (v3x - v2x);
		c2z = (v1x - v2x) * (v3y - v2y) - (v1y - v2y) * (v3x - v2x);

		glm::vec3 vec1(v3x - v2x, v3y - v2y, v3z - v2z);
		glm::vec3 vec2(v1x - v2x, v1y - v2y, v1z - v2z);

		glm::vec3 faceNormal = glm::cross(vec1, vec2);



		// Set the normalalized normal coordinates in the vertex array

		faceNormal = glm::normalize(faceNormal);

		if (vertices[v3Ind + 6] == -5) {
			vertices[v1Ind + 5] = faceNormal.x;
			vertices[v1Ind + 6] = faceNormal.y;
			vertices[v1Ind + 7] = faceNormal.z;
			vertices[v2Ind + 5] = faceNormal.x;
			vertices[v2Ind + 6] = faceNormal.y;
			vertices[v2Ind + 7] = faceNormal.z;
			vertices[v3Ind + 5] = faceNormal.x;
			vertices[v3Ind + 6] = faceNormal.y;
			vertices[v3Ind + 7] = faceNormal.z;
		}

		*(indices + ct++) = c + 1;
		*(indices + ct++) = c + w;
		*(indices + ct++) = c + w + 1;

		// Calculate its normal
		v1Ind = 8 * (c + 1);
		v2Ind = 8 * (c + w);
		v3Ind = 8 * (c + w + 1);
		v1x = vertices[v1Ind], v1y = vertices[v1Ind + 1], v1z = vertices[v1Ind + 2];
		v2x = vertices[v2Ind], v2y = vertices[v2Ind + 1], v2z = vertices[v2Ind + 2];
		v3x = vertices[v3Ind], v3y = vertices[v3Ind + 1], v3z = vertices[v3Ind + 2];

		// Cross product

		cx = (v3y - v1y) * (v2z - v1z) - (v3z - v1z) * (v2y - v1y);
		cy = (v3x - v1x) * (v2z - v1z) - (v3z - v1z) * (v2x - v1x);
		cz = (v3x - v1x) * (v2y - v1y) - (v3y - v1y) * (v2x - v1x);


		c2x = (v1y - v2y) * (v3z - v2z) - (v1z - v2z) * (v3y - v2y);
		c2y = (v1x - v2x) * (v3z - v2z) - (v1z - v2z) * (v3x - v2x);
		c2z = (v1x - v2x) * (v3y - v2y) - (v1y - v2y) * (v3x - v2x);

		vec1 = glm::vec3(v3x - v1x, v3y - v1y, v3z - v1z);
		vec2 = glm::vec3(v2x - v1x, v2y - v1y, v2z - v1z);

		faceNormal = glm::cross(vec2, vec1);


		// Set the normalalized normal coordinates in the vertex array
		if (vertices[v3Ind + 6] == -5) {
			faceNormal = glm::normalize(faceNormal);
			vertices[v1Ind + 5] = faceNormal.x;
			vertices[v1Ind + 6] = faceNormal.y;
			vertices[v1Ind + 7] = faceNormal.z;
			vertices[v2Ind + 5] = faceNormal.x;
			vertices[v2Ind + 6] = faceNormal.y;
			vertices[v2Ind + 7] = faceNormal.z;
			vertices[v3Ind + 5] = faceNormal.x;
			vertices[v3Ind + 6] = faceNormal.y;
			vertices[v3Ind + 7] = faceNormal.z;
		}
	}
	// Create a new vertex array object to store the grid's info.
	glGenVertexArrays(1, &vao);

	// Create a buffer for the vertex data and one for the indices.
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	// Bind the vertex array to signal to OpenGL that we are referring to it.
	glBindVertexArray(vao);

	// Bind the VBO and pass in the vertices.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, w*l * 8 * sizeof(float), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);

	// Let OpenGL know that the position vector will come in at layout = 0 in the shader code. We don't need the texture data yet so we won't bother to define its location.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5*sizeof(float)));

	// Bind the EBO and pass in the indices.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * (w - 1)*(l - 1) * sizeof(GLuint), indices, GL_STATIC_DRAW);

	// Not sure if these two calls below are necessary but I hear they're good practice
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	// Clear allocated memory now that it's been sent to the GPU. We don't need it on the CPU anymore.
	delete[8 * w*l] vertices;
	delete[6 * (w - 1)*(l - 1)] indices;

}


void Grid::Draw(GLint primitiveType, unsigned int diffuseTexture, unsigned int specularMap, unsigned int shadowMap)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, shadowMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);

	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glDrawElements(primitiveType, 6 * (width - 1)*(length - 1), GL_UNSIGNED_INT, 0);


	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

Grid::~Grid()
{
}
