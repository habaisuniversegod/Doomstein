#include "wall.h"
#include "player.h"
#include <vector>
#pragma once

class World {
	friend LevelLoader;

	std::vector<Wall> walls;
	std::vector<Entity*> entities;
	Player* player;

public:
	World () {}

	World(Player* player) {
		this->walls.reserve(32767);
		this->walls.resize(0);

		this->player = player;
	}

	inline std::vector<Wall>& getWalls() {
		return walls;
	}

	void drawMinimap(sf::RenderTarget& surface, sf::Vector2f center, sf::Vector2f mapCenter, float angle, float scale = 50.0f) {
		static sf::VertexArray lines(sf::Lines, walls.size() * 2);
		static sf::CircleShape entityPointer;
		static int updateMinimapTimer = 0;
		static Matrix rotate(0, 0, 0, 0);
		rotate = Matrix::makeRotation(- PI / 2 - angle);

		if (--updateMinimapTimer < 0) {
			updateMinimapTimer = 0;
			for (int i = 0; i < walls.size(); i++)
			{
				lines[i * 2] = sf::Vertex(((walls[i].getWallLeft() - center) * scale) * rotate + mapCenter, sf::Color(230, 50, 53));
				lines[i * 2 + 1] = sf::Vertex(((walls[i].getWallRight() - center) * scale) * rotate + mapCenter,sf::Color(230, 50, 53));
			}
			entityPointer.setRadius(player->hitboxRadius * scale);
			entityPointer.setPosition((player->getPositionXY() - center) * scale + mapCenter - sf::Vector2f(entityPointer.getRadius() / 2, entityPointer.getRadius() / 2));
			entityPointer.setFillColor(sf::Color(104, 97, 228));
			surface.clear({ 255, 255, 255 });
			surface.draw(lines);
			surface.draw(entityPointer);
		}
	}

	void addMob(Entity* mob) {
		entities.push_back(mob);
	}

	void updateMobs(float deltaTime) {
		for (Entity* e : entities)
			e->update(deltaTime, walls);
	}

	void drawMobs(sf::RenderTarget& surface, Camera& camera) {
		std::sort(entities.begin(), entities.end(), [](Entity* a, Entity* b) {return a->playerDist < b->playerDist; });
		for (int i = entities.size(); i > 0;) {
			i--;
			entities[i]->draw(surface, camera.realDistances, camera.visualDistances, entities[i]->getScreenProjection(camera.position, camera.angle));
		}
	}

	void __crash__(float deltaTime) {
		static float secs = 3.0f;
		for (int i = 0; i < walls.size(); i++){
			walls[i].left += sf::Vector2f(((std::rand() % 1000) - 500) / 10000.0 * pow(3.1 - secs, 3), ((std::rand() % 1000) - 500) / 10000.0 * pow(3.1 - secs, 3));
			walls[i].right += sf::Vector2f(((std::rand() % 1000) - 500) / 10000.0 * pow(3.1 - secs, 3), ((std::rand() % 1000) - 500) / 10000.0 * pow(3.1 - secs, 3));
		}
		secs -= deltaTime;
		if (secs < 0.0f) throw "Crash!!!!!!";
	}

	~World() {
		
	}
};
