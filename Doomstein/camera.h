#include <SFML/Graphics.hpp>
#include <vector>
#include "globals.h"
#include "log.h"
#include "wall.h"
#include "maths.h"
#include "types.h"
#pragma once

class Camera {
private:
	float* visualDistances;
	float* realDistances;
	short* wallIDs;
	float* wallTextureUOffsets;

	sf::Vector3f position;
	float angle;

	inline sf::Vector2f getXY() {
		return { position.x, position.y };
	}

public:
	bool showDamageEffect = false;
	float damageEffectTimer = 0.6f;
	sf::VertexArray wallLines;

	static sf::Vector2i getMouseMove(sf::Vector2i center, sf::RenderWindow& win) {
		sf::Vector2i delta = sf::Mouse::getPosition(win) - center;
		sf::Mouse::setPosition(center, win);
		return delta;
	}

	static short checkCross(std::vector<Wall>& walls, std::vector<short>& crossed, sf::Vector2f pos1, sf::Vector2f pos2) {
		for (int i = 0; i < crossed.size(); i++)
		{
			if (isIntersect(pos1, pos2, walls[crossed[i]].getWallLeft(), walls[crossed[i]].getWallRight())) {
				return crossed[i];
			}
		}
		return -1;
	}

	static inline sf::Vector2f WS_GetSheetCoord(uint8_t id) {
		return sf::Vector2f((id % 16) * WALL_TEXTURE_SIZE, (id / 16) * WALL_TEXTURE_SIZE);
	}

	Camera(sf::Vector3f pos, float alpha, float height) {
		update(pos, alpha, height, 0, 0);

		wallLines.resize(SCREEN_WIDTH * 2);
		wallLines.setPrimitiveType(sf::Lines);

		visualDistances = new float[SCREEN_WIDTH];
		realDistances = new float[SCREEN_WIDTH];
		wallIDs = new short[SCREEN_WIDTH];
		wallTextureUOffsets = new float[SCREEN_WIDTH];
	}

	~Camera() {
		delete[] visualDistances;
		delete[] realDistances;
		delete[] wallIDs;
		delete[] wallTextureUOffsets;
	}

	const sf::Vector3f& getPosition() {
		return position;
	}

	void update(sf::Vector3f pos, float alpha, float height, bool isMoving, float deltaTime) {
		static uint64_t deltaTimes = 0;

		position = pos;
		position.z += height;

		angle = alpha;

		if (isMoving && pos.z == 0.0f) {
			deltaTimes += 10000.0 * deltaTime;
			position.z = pos.z + height + 0.06 * sin(deltaTimes / 700.0);
		}
		else deltaTimes = 0;
	}

	void rayCast(std::vector<Wall>& walls) {
		sf::Vector2f scrLeft, scrRight, scrStep;

		std::vector<short> intersectedWalls;

		scrLeft = sf::Vector2f(std::cos(this->angle - PLAYER_FOV / 2), std::sin(this->angle - PLAYER_FOV / 2)) / std::cos(PLAYER_FOV / 2) + getXY();
		scrRight = sf::Vector2f(std::cos(this->angle + PLAYER_FOV / 2), std::sin(this->angle + PLAYER_FOV / 2)) / std::cos(PLAYER_FOV / 2) + getXY();
		scrStep = (scrRight - scrLeft) / (float)SCREEN_WIDTH;

		float currentRayLength, totalLength;
		short currentWall, cross;
		sf::Vector2f scrPoint, currentRay;

		for (int ray = 0; ray < SCREEN_WIDTH; ray++)
		{
			currentRayLength = RENDER_DISTANCE;
			totalLength = 0.0f;

			scrPoint = sf::Vector2f(scrLeft + scrStep * (float)ray - getXY());
			currentRay = scrPoint * (totalLength + currentRayLength);

			intersectedWalls.clear();
			for (int i = 0; i < walls.size(); i++)
			{
				if (isIntersect(getXY(), getXY() + (scrPoint * RENDER_DISTANCE), walls[i].getWallLeft(), walls[i].getWallRight())) {
					intersectedWalls.push_back(i);
				}
			}

			if (intersectedWalls.size() == 0) {
				this->visualDistances[ray] = INFINITY;
				this->realDistances[ray] = INFINITY;
				this->wallIDs[ray] = -1;
				this->wallTextureUOffsets[ray] = NAN;
				continue;
			}

			while (currentRayLength > MIN_RAY_LENGTH) {
				currentRay = scrPoint * (totalLength + currentRayLength);
				cross = checkCross(walls, intersectedWalls, getXY(), getXY() + currentRay);

				if (cross != -1) {
					currentRayLength /= 2;
					currentWall = cross;
					continue;
				}

				totalLength += currentRayLength;
				currentRayLength /= 2;
			}

			this->visualDistances[ray] = totalLength;
			this->realDistances[ray] = vectorLength(scrPoint * totalLength);
			this->wallIDs[ray] = currentWall;
			this->wallTextureUOffsets[ray] = vectorLength(walls[currentWall].getWallLeft() - (getXY() + (scrPoint * totalLength))) / WALL_HEIGHT;
		}
	}

	void updateWallVertices(std::vector<Wall>& walls) {
		for (int i = 0; i < SCREEN_WIDTH; i++)
		{
			if (this->wallIDs[i] == -1) {
				wallLines[i * 2].position = sf::Vector2f(i, 0);
				wallLines[i * 2 + 1] = sf::Vector2f(i, 0);
				continue;
			}
			float deltaH = position.z - WALL_HEIGHT / 2;
			float deltaHproj = deltaH / visualDistances[i] / FOV_HEIGHT * SCREEN_HEIGHT;
			float projectionSize = (WALL_HEIGHT / visualDistances[i] / FOV_HEIGHT) * SCREEN_HEIGHT;
			float projectionPos = (SCREEN_HEIGHT / 2) + (projectionSize / 2 + deltaHproj);

			sf::Vector2f textureGridOffset = WS_GetSheetCoord(walls[this->wallIDs[i]].getWallTexID());
			float uCoord = (int)(this->wallTextureUOffsets[i] * WALL_TEXTURE_SIZE) % WALL_TEXTURE_SIZE + textureGridOffset.x;
			uint8_t fog = fogColor(visualDistances[i]);
			sf::Color col(fog, fog, fog);

			wallLines[i * 2] = sf::Vertex(sf::Vector2f(i, projectionPos - projectionSize), col, sf::Vector2f(uCoord, textureGridOffset.y));
			wallLines[i * 2 + 1] = sf::Vertex(sf::Vector2f(i, projectionPos), col, sf::Vector2f(uCoord, textureGridOffset.y + WALL_TEXTURE_SIZE));
		}
	}

	void damageEffect(sf::Shader* vignetteShader, float deltaTime) {
		vignetteShader->setUniform("damage", damageEffectTimer / 0.6f);
		if (showDamageEffect) {
			damageEffectTimer -= deltaTime;
			if (damageEffectTimer < 0.0f) {
				damageEffectTimer = 0.6f;
				showDamageEffect = false;
			}	
		}
	}

	void drawOverlay(sf::RenderTarget& surface, SFPair* weaponOverlay, float deltaTime) {
		static sf::Vector2f bobbing = { 0.0f, 0.0f };
		static uint64_t dt = 0;
		static bool initislized = false;
		if (!initislized) {
			weaponOverlay->spr.setOrigin((sf::Vector2f)weaponOverlay->tx.getSize() / 2.0f);
			weaponOverlay->spr.setScale({ 1.05f, 1.05f });
			weaponOverlay->spr.setOrigin({ 0, 0 });
			initislized = true;
		}
		dt += deltaTime * 10000.0f;
		bobbing = sf::Vector2f(4 * sin(dt / 4000.0), 4 * cos(2 * dt / 4000.0));
		weaponOverlay->spr.setPosition(bobbing);
		surface.draw(weaponOverlay->spr);
	}
};
