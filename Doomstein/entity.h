#include <SFML/Graphics.hpp>
#include "maths.h"
#pragma once

class World;

class Entity {
	friend World;
protected:
	sf::Vector3f position;
	float angle;

	float maxSpeed;
	float maxAcceleration;
	sf::Vector2f currentSpeed = { 0.0f, 0.0f };
	sf::Vector2f currentAcceleration = { 0.0f, 0.0f };
	float maxSpeedZ;
	float currentSpeedZ;
	float currentAccelerationZ;
	float jumpAcceleration;

	float hitboxRadius;
	float hitboxHeight;

	float playerDist;

	bool isStaticEntity;
	int health;

	sf::Texture* texture;
	sf::Sprite sprite;

public:
	std::vector<Wall*>& checkCollision(std::vector<Wall>& walls, sf::Vector2f velocity) {
		static sf::Vector2f newPos, ray;

		newPos = sf::Vector2f(position.x, position.y) + velocity;

		static std::vector<Wall*> collided;

		collided.clear();

		for (int i = 0; i < COLLISION_RAYS; i++)
		{
			ray.x = std::cos(2 * PI / COLLISION_RAYS * i) * hitboxRadius;
			ray.y = std::sin(2 * PI / COLLISION_RAYS * i) * hitboxRadius;

			for (int j = 0; j < walls.size(); j++)
				if (isIntersect(newPos, newPos + ray, walls[j].getWallLeft(), walls[j].getWallRight())) 
					collided.push_back(&walls[j]);
		}
		return collided;
	}

	sf::Vector2f handleCollision(std::vector<Wall*> intersected, sf::Vector2f velocity) {
		static float inNormalProjection;
		static sf::Vector2f comp, newVelocity;

		newVelocity = velocity;
		for (int i = 0; i < intersected.size(); i++)
		{
			inNormalProjection = project(intersected[i]->getNormal(), velocity);
			inNormalProjection /= 5.0f;
			comp = intersected[i]->getNormal() * inNormalProjection;
			newVelocity -= comp;
		}
		return newVelocity;
	}

	/*void collisionBetweenEntities(Entity* entity) {
		static sf::Vector2f comp;
		comp = this->getPositionXY() - entity->getPositionXY();
		acceleration.x += comp.x * 4;
		acceleration.y += comp.y * 4;
	}*/

	inline sf::Vector2f getPositionXY() {
		return sf::Vector2f{ position.x, position.y };
	}

	inline float getPositionZ() {
		return position.z;
	}

	inline sf::Vector3f getPosition() {
		return this->position;
	}

	inline float getAngle() {
		if (angle >= 0.0) return angle;
		else return 2 * PI + angle;
	}

	void rotate(float deltaAngle) {
		angle = std::fmod(angle + deltaAngle, 2 * PI);
	}

	void move(sf::Vector3f moveVector) {
		position += moveVector;
	}

	virtual void update(float deltaTime, std::vector<Wall>& walls) = 0;

	float getPlayerDist(sf::Vector2f player) {
		return hypot(player.x - position.x, player.y - position.y);
	}

	sf::Vector2f getScreenProjection(sf::Vector3f playerPosition, float angle) {
		static sf::Vector2f delta_pos;
		static double delta_angle;
		static float offset;

		playerDist = getPlayerDist({playerPosition.x, playerPosition.y});

		delta_pos = this->getPositionXY() - sf::Vector2f(playerPosition.x, playerPosition.y);
		delta_angle = std::atan2(delta_pos.y, delta_pos.x) - angle;

		if (delta_pos.x > 0 && (angle < PI * 2 && angle > PI) || (delta_pos.x < 0 && delta_pos.y < 0)) delta_angle += PI * 2;

		offset = 0.5f + std::tan(delta_angle) / FOV_WIDTH;
		if (std::abs(delta_angle) <= PLAYER_FOV) 
			return sf::Vector2f{ offset * SCREEN_WIDTH, playerPosition.z - position.z };
		else 
			return sf::Vector2f{ NAN, NAN };
	}
	
	void draw(sf::RenderTarget& surface, float* realDistances, float* visualDistances, sf::Vector2f projPos) {
		static float currentXDistance, height, visibleHeight, visibleDistance;
		static int index;
		static uint8_t color;

		if (projPos.x != NAN) {
			index = std::min(std::max(0, (int)projPos.x), SCREEN_WIDTH);

			currentXDistance = realDistances[index];
			if (playerDist < currentXDistance && playerDist < RENDER_DISTANCE) {
				visibleDistance = playerDist / hypot(((projPos.x / SCREEN_WIDTH - 0.5f) * FOV_WIDTH), 1);
				height = hitboxHeight / visibleDistance / FOV_HEIGHT;
				visibleHeight = SCREEN_HEIGHT / this->texture->getSize().y * height;
				color = fogColor(visibleDistance);
				projPos.y = (SCREEN_HEIGHT / 2) + (visibleHeight / 2 + (projPos.y / visibleDistance / FOV_HEIGHT * SCREEN_HEIGHT));
				this->sprite.setScale(sf::Vector2f(visibleHeight, visibleHeight));
				this->sprite.setPosition(sf::Vector2f(projPos.x, projPos.y));
				this->sprite.setColor(sf::Color(color, color, color));

				surface.draw(sprite);
			}
		}
	}
};
