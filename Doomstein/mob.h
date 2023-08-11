//#include "globals.h"
//#include "log.h"
//#include "wall.h"
//#include "entity.h"
//#include <SFML/Graphics.hpp>
//
//#pragma once
//
//enum MobTypes {
//	TEST_MOB
//};
//
//class Mob : public Entity {
//	MobTypes type;
//
//public:
//	static Player* player;
//	static std::vector<Mob*> mobs;
//
//	Mob(sf::Vector3f position, float angle, MobTypes type)
//	{
//		this->position = position;
//		this->angle = angle;
//		this->type = type;
//
//		speed = 10.0f;
//		hitboxHeight = WALL_HEIGHT;
//		hitboxRadius = 0.4f;
//		health = INT32_MAX;
//
//		this->texture.loadFromFile("assets/textures/ico.png");
//		this->sprite.setTexture(this->texture);
//		this->sprite.setOrigin((sf::Vector2f)(this->texture.getSize() / 2u));
//	}
//
//	void update(float deltaTime, std::vector<Wall>& walls) override {
//		static std::vector<Wall*> intersected;
//		sf::Vector2f velocityXY;
//
//		sf::Vector2f forceXY = norm(player->getPositionXY() - this->getPositionXY());
//
//		acceleration.x = forceXY.x / 10.0f;
//		acceleration.y = forceXY.y / 10.0f;
//
//		for (Mob* mob: mobs)
//		{
//			if (mob == this) continue;
//			collisionBetweenEntities(mob);
//		}
//
//		velocity += acceleration * deltaTime;
//		if (vectorLength(velocity) > speed * deltaTime) velocity = norm(velocity) * speed * deltaTime;
//
//		velocityXY = { velocity.x, velocity.y };
//
//		intersected = checkCollision(walls, velocityXY);
//
//		if (intersected.size() > 0) {
//			velocityXY = handleCollision(intersected, velocityXY);
//		}
//
//		velocity.x = velocityXY.x;
//		velocity.y = velocityXY.y;
//
//		move(velocity);
//	}
//
//};
//
//Player* Mob::player;
//std::vector<Mob*> Mob::mobs;