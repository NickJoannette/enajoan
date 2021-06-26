 #pragma once
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>

using namespace glm;

// Simple class to hold our ray casting funcs

namespace PhysicsUtilities {

	inline static bool withinCube(vec3 position, vec3 cubeCenter, float sideLength) {
		// Check if in x bound

		float minX = cubeCenter.x - sideLength;
		float maxX = cubeCenter.x + sideLength;
		float minY = cubeCenter.y - sideLength;
		float maxY = cubeCenter.y + sideLength;
		float minZ = cubeCenter.z - sideLength;
		float maxZ = cubeCenter.z + sideLength;

		return (position.x >= minX && position.x <= maxX && position.y >= minY && position.y <= maxY && position.z >= minZ && position.z <= maxZ);
	}

	// Ray intersection utility. rO = ray origin, rD = ray direction, sO = sphere origin, sqRad = radius squared
	inline static bool QuickIntersectRaySphere(const vec3 &rO, const vec3 &rD, const vec3 &sO, const float sqRad)
	{
		vec3 L = sO - rO;
		float tca = dot(L, rD);
		float d2 = dot(L, L) - tca * tca;
		if (d2 <= sqRad) {
			float thc = sqrt(sqRad - d2);
			return true;
		}
		else return false;
	}
}