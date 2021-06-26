#pragma once

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace glm;

class Boid
{
public:

	std::vector<Boid> * Flock;
	std::vector<Boid> NeighbouringBoids;

	vec3 acceleration, velocity, position;
	vec3 cohesionAcceleration, alignmentAcceleration, separationAcceleration, tenacityAcceleration;
	vec3 color; // for fun

	struct ArbitraryBounds {
		float minX, minY, minZ, maxX, maxY, maxZ;
	};

	ArbitraryBounds AB;

	Boid(std::vector<Boid> * Flock) : Flock(Flock) {
		AB = ArbitraryBounds{ -22,-22,-22,22,22,22 };

		cohesionAcceleration = alignmentAcceleration = separationAcceleration = tenacityAcceleration = acceleration = vec3(0.0f);
		// Velocity set to a random value

		// Position set to a random bounded value
		position = vec3(RandomFloat(AB.minX, AB.maxX) / 3.0f, RandomFloat(AB.minY, AB.maxY)/3.0f, RandomFloat(AB.minZ, AB.maxZ) / 3.0f);
		velocity = vec3(RandomFloat(AB.minX, AB.maxX) / 3.0f, RandomFloat(AB.minY, AB.maxY) / 3.0f, RandomFloat(AB.minZ, AB.maxZ) / 3.0f);
		color = vec3(RandomFloat(0, 255), RandomFloat(0, 255), RandomFloat(0, 255));
	}

	void printBoidInfo() {

		std::cout << "----------------------------" << std::endl;
		std::cout << "Position: " << to_string(position) << std::endl;
		std::cout << "Velocity: " << to_string(velocity) << std::endl;
		std::cout << "Acceleration: " << to_string(acceleration) << std::endl;

	}

	void update(float dt, vec3 target, float alignmentStrength, float cohesionStrength, float separationStrength, float tenacityFactor) {
		// Cohese with the velocity of the flock.
		// Determine the difference
		// Quaternate towards it


		/*

		// Hey -> What is the average velocity of my flock?
		unsigned int flockSize = Flock->size();
		vec3 flockVelocitySum(0);
		for (int i = 0; i < flockSize; ++i) flockVelocitySum += Flock->at(i).velocity;
		vec3 flockAvgVelocity = flockVelocitySum/(float)flockSize;

		vec3 alignmentAcceleration = (flockAvgVelocity - velocity);
		vec3 cohesionAcceleration = (flockAvgPosition - position);

		vec3 tenacityVector = tenacityFactor * vec3(RandomFloat(AB.minX, AB.maxX), RandomFloat(AB.minY, AB.maxY), RandomFloat(AB.minZ, AB.maxZ));
		*/

		// Clear the neighoubrs
		vec3 sumVectorsTowardsNeighbors(0.0f);
		vec3 sumNeighborPositions(0.0f);
		vec3 sumNeighborVelocities(0.0f);
		vec3 tenaciousAcceleration = tenacityFactor * vec3(RandomFloat(AB.minX, AB.maxX), RandomFloat(AB.minY, AB.maxY), RandomFloat(AB.minZ, AB.maxZ));

		
		// Find the current neighbours
		float sightRadius = 85.0f;
		int c = 0;
		for (Boid B : *Flock) {
			float d = distance(B.position, position);
			if (d > 0 && d < sightRadius) {
				sumNeighborVelocities += B.velocity;
				sumNeighborPositions += B.position;
				vec3 diff = position - B.position;
				diff = normalize(diff) / d;
				separationAcceleration += diff;
				++c;
			}
		}
		
		if (c > 0) {
			separationAcceleration /= (float)c;
			separationAcceleration = normalize(separationAcceleration);
			alignmentAcceleration = normalize(sumNeighborVelocities/(float)c);
			cohesionAcceleration = normalize(sumNeighborPositions / (float)c - position);
		}

		vec3 accelerationTowardsTarget = normalize(target - position);
		acceleration = separationAcceleration * separationStrength + alignmentAcceleration * alignmentStrength + 
			cohesionAcceleration * cohesionStrength + tenacityFactor * accelerationTowardsTarget;
		velocity += 0.025f * 1.0f*acceleration;
		acceleration = separationAcceleration = cohesionAcceleration = alignmentAcceleration = vec3(0.0f);
		if (length(velocity) > 1.50f) velocity = 1.50f*normalize(velocity);
		position += 0.1f*velocity;
		//returnToBounds(position);
	}

	void returnToBounds(vec3 & position) {
		if (position.x < AB.minX) position.x = AB.maxX;
		if (position.y < AB.minY) position.y = AB.maxY;
		if (position.z < AB.minZ) position.z = AB.maxZ;	
		
		if (position.x > AB.maxX) position.x = AB.minX;
		if (position.y > AB.maxY) position.y = AB.minY;
		if (position.z > AB.maxZ) position.z = AB.minZ;
	}

	float RandomFloat(float a, float b) {
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = b - a;
		float r = random * diff;
		return a + r;
	}


private:


};

