#pragma once
#include <glm/glm.hpp>
#include "MarchingCubeChunk.h"
#include <vector>

// Functions we implemented to help with vectors
using namespace glm;

namespace Utilities {


	enum AXIS {X,Y,Z,NEGX,NEGY,NEGZ, NONE};
	struct SWAP_INFO {
		AXIS axis;

	};

	SWAP_INFO findChunksToSwap(vec3 oldChunkCenter, vec3 newChunkCenter, float chunkScale, std::vector<MarchingCubeChunk *> & chunksToSwap, MarchingCubeChunk chunks[3][3][3]) {
		// If no change, return
		if (oldChunkCenter == newChunkCenter) return SWAP_INFO{ AXIS{NONE} };
		chunksToSwap.clear();
		// Else find what axis we changed on
		AXIS axisChanged;
		float diameter = 6.0f ;

		if (newChunkCenter.x > oldChunkCenter.x) axisChanged = X;
		else if (newChunkCenter.y > oldChunkCenter.y) axisChanged = Y;
		else if (newChunkCenter.z > oldChunkCenter.z) axisChanged = Z;
		else if (newChunkCenter.x < oldChunkCenter.x) axisChanged = NEGX;
		else if (newChunkCenter.y < oldChunkCenter.y) axisChanged = NEGY;
		else if (newChunkCenter.z < oldChunkCenter.z) axisChanged = NEGZ;

		// Then find all chunks >=2 diameters away on that axis
		for (int x = 0; x < 3; ++x) for (int y = 0; y < 3; ++y) for (int z = 0; z < 3; ++z) {
			if (axisChanged == NEGX || axisChanged == X) {
				if (abs(chunks[x][y][z].center.x - newChunkCenter.x) >= 2*diameter) chunksToSwap.push_back(&chunks[x][y][z]);

			}
			else if (axisChanged == NEGY || axisChanged == Y) {
				if (abs(chunks[x][y][z].center.y - newChunkCenter.y) >= 2*diameter) chunksToSwap.push_back(&chunks[x][y][z]);

			}
			else if (axisChanged == NEGZ || axisChanged == Z) {
				if (abs(chunks[x][y][z].center.z - newChunkCenter.z) >= 2*diameter) chunksToSwap.push_back(&chunks[x][y][z]);

			}
		}

		std::cout << chunksToSwap.size() << std::endl;
		return SWAP_INFO{ axisChanged };
	}

	MarchingCubeChunk & currentChunk(vec3 chunkCenter, MarchingCubeChunk chunks[3][3][3]) {
		for (int x = 0; x < 3; ++x) for (int y = 0; y < 3; ++y) for (int z = 0; z < 3; ++z) {
			if (chunkCenter == chunks[x][y][z].center) return chunks[x][y][z];
		}
	}

	int RoundToMultiple(int toRound, int multiple)
	{
		const auto ratio = static_cast<double>(toRound) / multiple;
		const auto iratio = std::lround(ratio);
		return iratio * multiple;
	}

	vec3 getCollisionChunkIndices(vec3 currentPosition, vec3 nearestChunkCenter, float chunkScale) {
		int diameter = 60.0f;
		int radius = 30.0f;

		int roundedPosX = round(currentPosition.x);
		int centerX = RoundToMultiple(roundedPosX, radius);
		if (centerX % diameter == 0) {
			if (centerX > roundedPosX) centerX -= radius;
			else if (centerX < roundedPosX) centerX += radius;
		}

		int diffX = centerX - 60 * nearestChunkCenter.x;
		int collIndX = 0;
		switch (diffX) {
		case -150:
			collIndX = 0;
			break;
		case -90:
			collIndX = 1;
			break;

		case -30:
			collIndX = 2;
			break;

		case 30:
			collIndX = 3;
			break;

		case 90:
			collIndX = 4;
			break;

		case 150:
			collIndX = 5;
			break;

		}


		int roundedPosY = round(currentPosition.y);
		int centerY = RoundToMultiple(roundedPosY, radius);
		if (centerY % diameter == 0) {
			if (centerY > roundedPosY) centerY -= radius;
			else if (centerY < roundedPosY) centerY += radius;
		}

		int diffY = centerY - 60 * nearestChunkCenter.y;
		int collIndY = 0;
		switch (diffY) {
		case -150:
			collIndY = 0;
			break;
		case -90:
			collIndY = 1;
			break;

		case -30:
			collIndY = 2;
			break;

		case 30:
			collIndY = 3;
			break;

		case 90:
			collIndY = 4;
			break;

		case 150:
			collIndY = 5;
			break;
		}


		int roundedPosZ = round(currentPosition.z);
		int centerZ = RoundToMultiple(roundedPosZ, radius);
		if (centerZ % diameter == 0) {
			if (centerZ > roundedPosZ) centerZ -= radius;
			else if (centerZ < roundedPosZ) centerZ += radius;
		}

		int diffZ = centerZ - 60 * nearestChunkCenter.z;
		int collIndZ = 0;
		switch (diffZ) {
		case -150:
			collIndZ = 0;
			break;
		case -90:
			collIndZ = 1;
			break;

		case -30:
			collIndZ = 2;
			break;

		case 30:
			collIndZ = 3;
			break;

		case 90:
			collIndZ = 4;
			break;

		case 150:
			collIndZ = 5;
			break;

		}


		return vec3(collIndX, collIndY, collIndZ);
	}

	vec3 getNearestChunkCenter(vec3 currentPosition, float chunkScale) {

		int div = chunkScale;
		float diameter = chunkScale * 6.0f;
		float radius = diameter/2.0f;

		int centerX = RoundToMultiple((int)round(currentPosition.x), diameter);
		int centerY = RoundToMultiple((int)round(currentPosition.y), diameter);
		int centerZ = RoundToMultiple((int)round(currentPosition.z), diameter);

		return vec3(centerX/chunkScale, centerY/chunkScale, centerZ/chunkScale);

	}

	mat4 extractRotation(const mat4 & in) {
		float sX = length(vec3(in[0]));
		float sY = length(vec3(in[1]));
		float sZ = length(vec3(in[2]));
		mat4 rotMat{
			in[0][0] / sX, in[0][1] / sY, in[0][2] / sZ,0,
			in[1][0] / sX, in[1][1] / sY, in[1][2] / sZ,0,
			in[2][0] / sX, in[2][1] / sY, in[2][2] / sZ,0,
			0,0,0,1
		};

		return rotMat;
	}

	vec3 gammaCorrect(vec3 in, float gammaValue) {
		return glm::pow(in, vec3(1.0f/gammaValue));
	}

	template <typename T>
	bool contains(std::vector<T *> v, T * e) {
		for (T * t : v) if (t == e) return true;
		return false;

	};

	template <typename T>
	void remove(std::vector<T *> & v, T * e) {
		auto i = v.begin();
		unsigned int c = 0;
		for (T * t : v) {
			if (t == e) {
				v.erase(i + c);
				return;
			}
			c++;
		}
	};
}