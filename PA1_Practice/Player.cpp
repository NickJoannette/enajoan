#include "Player.h"

void Player::processMovement(Movement direction, float normalizedSpeed) {
	if (direction == FORWARD) {
		Velocity += 22.50f*Front * normalizedSpeed;
	}
	if (direction == BACKWARD) {
		Velocity -=  67.50f*Front * normalizedSpeed;
	}

}

void Player::update(float dt) {
	Velocity = length(Velocity) * Front;
	if (length(Velocity) > 20.0f) Velocity = 20.0f * normalize(Velocity);
	Position += Velocity;
	ModelMatrix = translate(ModelMatrix, Velocity);
}

void Player::printMetrics() {

	std::cout << " --------------------- " << std::endl;
	std::cout << "PLAYER OBJECT METRICS" << std::endl;
	std::cout << "Position: (" << Position.x << ", " << Position.y << ", " << Position.z << ")" << std::endl;
	std::cout << "Front: (" << Front.x << ", " << Front.y << ", " << Front.z << ")" << std::endl;
	std::cout << "Position: (" << Position.x << ", " << Position.y << ", " << Position.z << ")" << std::endl;
	std::cout << "Velocity: (" << Velocity.x << ", " << Velocity.y << ", " << Velocity.z << ")" << std::endl;
	std::cout << "Acceleration: (" << Acceleration.x << ", " << Acceleration.y << ", " << Acceleration.z << ")" << std::endl;
}