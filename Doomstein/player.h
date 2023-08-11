#include <SFML/Graphics.hpp>
#include "camera.h"
#include "globals.h"
#include "entity.h"
#include "log.h"
#include "SFXLoader.h"
#pragma once

class World;

class Player: public Entity {
	friend LevelLoader;
	friend World;

private:
	Camera* attachedCamera = nullptr;
public:
	bool moving = false;
	bool jumping = false;

	inline Camera& getCamera() {
		return *attachedCamera;
	}

	Player() {};

	Player(sf::Vector3f pos, float angle) {
		attachedCamera = new Camera(pos, angle, PLAYER_HEIGHT);

		isStaticEntity = false;

		maxSpeed = 15.0f;
		maxAcceleration = 5.0f;
		maxSpeedZ = MAX_FALL_SPEED;
		jumpAcceleration = PLAYER_JUMP_ACCELERATION;

		health = 100;
		hitboxHeight = PLAYER_HEIGHT;
		hitboxRadius = PLAYER_SIZE;

		this->position = pos;
		this->angle = angle;
	}

	~Player()  {
		delete attachedCamera;
	}

	void update(float deltaTime, std::vector<Wall>& walls) override {
		static std::vector<Wall*> intersected;
		static float speedLength = 0.0f;
		static sf::Vector2f moveVec = { 0.0f, 0.0f };
		moving = false;
		jumping = false;

		//currentAcceleration = { 0.0f, 0.0f };
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			currentAcceleration = sf::Vector2f(std::cos(angle), std::sin(angle)) * maxAcceleration;
			moving = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			currentAcceleration = -sf::Vector2f(std::cos(angle), std::sin(angle)) * maxAcceleration;
			moving = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			currentAcceleration = sf::Vector2f(std::sin(angle), -std::cos(angle)) * maxAcceleration;
			moving = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			currentAcceleration = -sf::Vector2f(std::sin(angle), -std::cos(angle)) * maxAcceleration;
			moving = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			if (position.z == 0.0f) { jumping = true; 
				if (SFXLoader::objectPtr->getSound("jump")->getStatus() != sf::Sound::Playing) {
					SFXLoader::objectPtr->getSound("jump")->play();
				}
			}
		}
		if (moving && position.z == 0.0f && SFXLoader::objectPtr->getSound("walk")->getStatus() != sf::Sound::Playing) {
			SFXLoader::objectPtr->getSound("walk")->play();
		}
		// x,y movement
		if (moving) {
			currentSpeed += currentAcceleration;
			speedLength = vectorLength(currentSpeed);
			if (speedLength > maxSpeed) {
				currentSpeed = norm(currentSpeed) * maxSpeed;
			}
		}
		else {
			if (dot(currentSpeed, currentAcceleration) < 0.0f) currentSpeed = { 0.0f, 0.0f };
			if (position.z > 0.0f) currentSpeed -= 4.0f * currentSpeed * deltaTime;		// air resistance
			else currentSpeed -= currentSpeed * deltaTime * 30.0f;						// friction
		}
		moveVec = currentSpeed * deltaTime;
		intersected = checkCollision(walls, moveVec);
		if (intersected.size() > 0) {
			moveVec = handleCollision(intersected, moveVec);
		}
		// z movement
		currentAccelerationZ = FREE_FALL * deltaTime;
		if (jumping) currentAccelerationZ = jumpAcceleration;

		if (position.z > 0.0f || jumping) {
			currentSpeedZ += currentAccelerationZ;
			if (currentSpeedZ >= maxSpeedZ) currentSpeedZ = maxSpeedZ;
		}
		else {
			currentSpeedZ = 0.0f;
			position.z = 0.0f;
		}

		move(sf::Vector3f(moveVec.x, moveVec.y, currentSpeedZ * deltaTime));
	}
};

