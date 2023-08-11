#include <SFML/Graphics.hpp>
#include "maths.h"
#pragma once

class Entity {
protected:
	sf::Vector3f position;
	float angle;

	float maxSpeed;
	float maxAcceleration;
	sf::Vector2f currentSpeed;
	sf::Vector2f currentAcceleration;
	float maxSpeedZ;
	float currentSpeedZ;
	float currentAccelerationZ;
	float jumpAcceleration;

	float hitboxRadius;
	float hitboxHeight;

	bool isStaticEntity;
	int health;

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

	/*void getScreenProjection(sf::Vector3f playerPosition, float angle) {
		static sf::Vector2f delta_pos;
		static double delta_angle;
		static float offset;

		delta_pos = this->getPositionXY() - sf::Vector2f(playerPosition.x, playerPosition.y);
		delta_angle = std::atan2(delta_pos.y, delta_pos.x) - angle;

		if (delta_pos.x > 0 && (angle < PI * 2 && angle > PI) || (delta_pos.x < 0 && delta_pos.y < 0)) delta_angle += PI * 2;

		offset = 0.5f + std::tan(delta_angle) / FOV_WIDTH;
		if (std::abs(delta_angle) <= PLAYER_FOV) { 
			this->onScreen = true;
			this->screenPosition = {offset * SCREEN_WIDTH, SCREEN_HEIGHT / 2};
		}
		else {
			this->onScreen = false;
		}
		distanceToPlayer = distance(sf::Vector2f(playerPosition.x, playerPosition.y), this->getPositionXY());
	}

	void draw(sf::RenderTarget& surface, float* realDistances) {
		static float currentXDistance, height, visibleHeight, visibleDistance;
		static int index;
		static uint8_t color;

		if (onScreen) {
			index = std::min(std::max(0, (int)this->screenPosition.x), SCREEN_WIDTH);
			currentXDistance = realDistances[index];
			if (distanceToPlayer < currentXDistance && distanceToPlayer < RENDER_DISTANCE) {
				visibleDistance = distanceToPlayer / std::cos(std::atan(this->screenPosition.x / SCREEN_WIDTH - 0.5));
				height = hitboxHeight / visibleDistance;
				visibleHeight = SCREEN_HEIGHT / this->texture.getSize().y * height;
				color = farColor(visibleDistance);

				this->sprite.setScale(sf::Vector2f(visibleHeight, visibleHeight));
				this->sprite.setPosition(sf::Vector2f(screenPosition.x, screenPosition.y));
				this->sprite.setColor(sf::Color(color, color, color));

				surface.draw(sprite);
			}
		}
	}*/
};
