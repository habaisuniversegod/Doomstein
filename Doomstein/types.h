#include "globals.h"
#include <SFML/Graphics.hpp>
#pragma once

struct SFPair {
	sf::Texture tx;
	sf::Sprite spr;

	SFPair() {
	}

	SFPair(sf::Vector2u size) {
		tx.create(size.x, size.y);
		spr.setTexture(tx);
	}

	SFPair(const SFPair& source) {
		tx = source.tx;
		spr = source.spr;
	}

	void loadTexture(std::string filename) {
		if (!tx.loadFromFile(filename)) {
			crash("\nTexture " + filename + " not found!");
		}
		spr.setTexture(tx);
	}
};
