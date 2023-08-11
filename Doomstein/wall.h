#include <SFML/Graphics.hpp>
#include "globals.h"
#include "maths.h"
#include "log.h"
#pragma once

class Wall {
	friend World;
protected:
	sf::Vector2f left;
	sf::Vector2f right;
	sf::Vector2f normal;

	uint8_t textureID;

	void makeNormal() {
		sf::Vector2f wallVec = (right - left);
		wallVec /= vectorLength(wallVec);
		normal = sf::Vector2f(wallVec.y, wallVec.x);
	}

public:
	Wall() {};

	Wall(sf::Vector2f wallLeft, sf::Vector2f wallRight, uint8_t textureID) {
		this->left = wallLeft;
		this->right = wallRight;
		this->textureID = textureID;
		makeNormal();
	}

	inline sf::Vector2f getWallLeft() {
		return left;
	}

	inline sf::Vector2f getWallRight() {
		return right;
	}

	inline sf::Vector2f getNormal() {
		return normal;
	}

	inline uint8_t getWallTexID() {
		return textureID;
	}
};


