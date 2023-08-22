#include "globals.h"
#include "log.h"
#include "wall.h"
#include "entity.h"
#include "player.h"
#include <SFML/Graphics.hpp>

#pragma once

enum MobTypes {
	TEST_MOB
};

class Mob : public Entity {
	MobTypes type;

public:
	static Player* player;
	static std::vector<Mob*> mobs;

	Mob(sf::Vector3f position, float angle, MobTypes type, sf::Texture* texture)
	{
		this->position = position;
		this->angle = angle;
		this->type = type;
		this->texture = texture;

		maxAcceleration = maxSpeed = 0.0f;
		
		maxSpeedZ = currentAccelerationZ = jumpAcceleration = isStaticEntity = 0;
		
		hitboxHeight = 1.5f;
		hitboxRadius = 1.5f;
		health = INT32_MAX;

		this->sprite.setTexture(*this->texture);
		this->sprite.setOrigin((sf::Vector2f)(this->texture->getSize() / 2u));
	}

	void update(float deltaTime, std::vector<Wall>& walls) override {
		static uint64_t huy = std::rand();
		huy += deltaTime * 10000;
		position.z = 0.05 * std::sin(huy / 21000.0);
	}

};

Player* Mob::player;
std::vector<Mob*> Mob::mobs;